/*
 * Copyright (C) 2019, Ksenia Balistreri
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "cpu.h"

#define OPCODE_TYPE_MASK 		0xf000
#define OPCODE_ALU_OP_MASK 		0x000f
#define OPCODE_EXT_OP_MASK 		0x00ff
#define OPCODE_ADDR_MASK 		0x0fff
#define OPCODE_IMM_MASK 		0x00ff
#define OPCODE_REG_X_MASK 		0x0f00
#define OPCODE_REG_Y_MASK 		0x00f0
#define OPCODE_SPRITE_H_MASK 	0x000f
#define OPCODE_KEY_MASK 		0x00ff



#define OPCODE_REG_X_ARG(a)		(a & OPCODE_REG_X_MASK) >> 8
#define OPCODE_REG_Y_ARG(a)		(a & OPCODE_REG_Y_MASK) >> 4

#define OPCODE_TYPE_NATIVE 		0x0000
#define OPCODE_TYPE_JMP 		0x1000
#define OPCODE_TYPE_CALL 		0x2000
#define OPCODE_TYPE_JEQ_IMM 	0x3000
#define OPCODE_TYPE_JNEQ_IMM 	0x4000
#define OPCODE_TYPE_JEQ 		0x5000
#define OPCODE_TYPE_MOV_IMM 	0x6000
#define OPCODE_TYPE_ADD_IMM 	0x7000
#define OPCODE_TYPE_ALU		 	0x8000
#define OPCODE_TYPE_JNEQ		0x9000
#define OPCODE_TYPE_MOV_I	 	0xa000
#define OPCODE_TYPE_JMP_V0		0xb000
#define OPCODE_TYPE_RAND		0xc000
#define OPCODE_TYPE_DRAW		0xd000
#define OPCODE_TYPE_KEY			0xe000
#define OPCODE_TYPE_EXT			0xf000

#define OPCODE_CLR_ADDR 		0x00e0
#define OPCODE_RET_ADDR 		0x00ee

#define OPCODE_ALU_OP_ASSIGN 	0x0000
#define OPCODE_ALU_OP_BIT_OR 	0x0001
#define OPCODE_ALU_OP_BIT_AND 	0x0002
#define OPCODE_ALU_OP_BIT_XOR 	0x0003
#define OPCODE_ALU_OP_ADD_X		0x0004
#define OPCODE_ALU_OP_SUB_X		0x0005
#define OPCODE_ALU_OP_BIT_MOV_R 0x0006
#define OPCODE_ALU_OP_SUBN_X 	0x0007
#define OPCODE_ALU_OP_BIT_MOV_L 0x000e

#define OPCODE_KEY_DOWN_MAP		0x009e
#define OPCODE_KEY_UP_MAP		0x00a1

#define OPCODE_TYPE_EXT_GET_DLY	0x0007
#define OPCODE_TYPE_EXT_KEY		0x000a
#define OPCODE_TYPE_EXT_DELAY	0x0015
#define OPCODE_TYPE_EXT_SOUND	0x0018
#define OPCODE_TYPE_EXT_IMM		0x001e
#define OPCODE_TYPE_EXT_FONT	0x0029
#define OPCODE_TYPE_EXT_BCD		0x0033
#define OPCODE_TYPE_EXT_DUMP	0x0055
#define OPCODE_TYPE_EXT_LOAD	0x0065



static inline uint16_t c8_cpu_fetch_instr(c8_vm_t* vm) {
	return vm->c8_memory[vm->c8_program_counter++] << 8 | vm->c8_memory[vm->c8_program_counter++];
}

static void c8_cpu_native(c8_vm_t* vm, uint16_t addr) {
	if (addr == OPCODE_CLR_ADDR) {
		memset(&vm->c8_frame_buffer, 0, FRAME_BUFFER_SIZE);
		vm->c8_draw = 1;
	} else if (addr == OPCODE_RET_ADDR) {
		vm->c8_program_counter = vm->c8_stack[--vm->c8_stack_counter];
	}
}

static void c8_cpu_jump(c8_vm_t* vm, uint16_t addr) {
	vm->c8_program_counter = addr;
}

static void c8_cpu_call(c8_vm_t* vm, uint16_t addr) {
	vm->c8_stack[vm->c8_stack_counter++] = vm->c8_program_counter;
	vm->c8_program_counter = addr;
}

static void c8_cpu_jeq_imm(c8_vm_t* vm, uint8_t x, uint8_t imm) {
	if (vm->c8_registers[x] == imm) {
		vm->c8_program_counter += 2;
	}
}

static void c8_cpu_jneq_imm(c8_vm_t* vm, uint8_t x, uint8_t imm) {
	if (vm->c8_registers[x] != imm) {
		vm->c8_program_counter += 2;
	}
}

static void c8_cpu_jeq(c8_vm_t* vm, uint8_t x, uint8_t y) {
	if (vm->c8_registers[x] == vm->c8_registers[y]) {
		vm->c8_program_counter += 2;
	}
}

static void c8_cpu_mov_imm(c8_vm_t* vm, uint8_t x, uint8_t imm) {
	vm->c8_registers[x] = imm;
}

static void c8_cpu_add_imm(c8_vm_t* vm, uint8_t x, uint8_t imm) {
	vm->c8_registers[x] += imm;
}

static void c8_cpu_alu(c8_vm_t* vm, uint8_t x, uint8_t y, uint16_t alu_op) {
	switch (alu_op) {
		case OPCODE_ALU_OP_ASSIGN:
			vm->c8_registers[x] = vm->c8_registers[y];
			break;
		case OPCODE_ALU_OP_BIT_OR:
			vm->c8_registers[x] = vm->c8_registers[x] | vm->c8_registers[y];
			break;
		case OPCODE_ALU_OP_BIT_AND:
			vm->c8_registers[x] = vm->c8_registers[x] & vm->c8_registers[y];
			break;
		case OPCODE_ALU_OP_BIT_XOR:
			vm->c8_registers[x] = vm->c8_registers[x] ^ vm->c8_registers[y];
			break;
		case OPCODE_ALU_OP_ADD_X:
			if ((vm->c8_registers[x] + vm->c8_registers[y]) > 0xff) {
				vm->c8_registers[REGISTER_VF] = 1;
			} else {
				vm->c8_registers[REGISTER_VF] = 0;
			}
			vm->c8_registers[x] = vm->c8_registers[x] + vm->c8_registers[y];
			break;
		case OPCODE_ALU_OP_SUB_X:
			if (vm->c8_registers[x] > vm->c8_registers[y]) {
				vm->c8_registers[REGISTER_VF] = 1;
			} else {
				vm->c8_registers[REGISTER_VF] = 0;
			}
			vm->c8_registers[x] -= vm->c8_registers[y];
			break;
		case OPCODE_ALU_OP_BIT_MOV_R:
			if (vm->c8_shift_hack) {
				y = x;
			}

			vm->c8_registers[REGISTER_VF] = vm->c8_registers[y] & 0x01;
			vm->c8_registers[x] = vm->c8_registers[y] >> 1;
			break;
		case OPCODE_ALU_OP_SUBN_X:
			if (vm->c8_registers[y] > vm->c8_registers[x]) {
				vm->c8_registers[REGISTER_VF] = 1;
			} else {
				vm->c8_registers[REGISTER_VF] = 0;
			}
			vm->c8_registers[x] = vm->c8_registers[y] - vm->c8_registers[x];
			break;
		case OPCODE_ALU_OP_BIT_MOV_L:
			if (vm->c8_shift_hack) {
				y = x;
			}

			vm->c8_registers[REGISTER_VF] = (vm->c8_registers[y] & 0x80) >> 7;
			vm->c8_registers[x] = vm->c8_registers[y] << 1;
			break;
		default:
			break;
	}

}

static void c8_cpu_jneq(c8_vm_t* vm, uint8_t x, uint8_t y) {
	if (vm->c8_registers[x] != vm->c8_registers[y]) {
		vm->c8_program_counter += 2;
	}
}

static void c8_cpu_mov_i(c8_vm_t* vm, uint16_t addr) {
	vm->c8_immediate = addr;
}

static void c8_cpu_jmp_v0(c8_vm_t* vm, uint16_t addr) {
	vm->c8_program_counter = addr + vm->c8_registers[0];
}

static void c8_cpu_rand(c8_vm_t* vm, uint8_t x, uint8_t imm) {
	vm->c8_registers[x] = (rand() % 256) & imm;
}

static void c8_cpu_draw(c8_vm_t* vm, uint8_t vx, uint8_t vy, uint8_t sprite_height) {
	uint8_t x = vm->c8_registers[vx];
	uint8_t y = vm->c8_registers[vy];
	vm->c8_registers[REGISTER_VF] = 0;
	vm->c8_draw = 1;

	for (int i = 0; i < sprite_height; i++) {
		if (x >= SCREEN_WIDTH || y >= SCREEN_HEIGHT) {
			continue;
		}

		if ((x % 8) != 0) {
			uint16_t pixs = (vm->c8_frame_buffer[y + i][(x / 8)] << 8) | vm->c8_frame_buffer[y + i][(x / 8) + 1];

			vm->c8_frame_buffer[y + i][(x / 8)] ^= vm->c8_memory[vm->c8_immediate + i] >> (x % 8);
			vm->c8_frame_buffer[y + i][(x / 8) + 1] ^= vm->c8_memory[vm->c8_immediate + i] << (8 - (x % 8));

			if ((pixs & ((vm->c8_frame_buffer[y + i][(x / 8)] << 8) | vm->c8_frame_buffer[y + i][(x / 8) + 1])) != pixs) {
				vm->c8_registers[REGISTER_VF] = 1;
			}
		} else {
			uint8_t pix = vm->c8_frame_buffer[y + i][x / 8];
			vm->c8_frame_buffer[y + i][x / 8] ^= vm->c8_memory[vm->c8_immediate + i];

			if((pix & vm->c8_frame_buffer[y + i][x / 8]) != pix) {
				vm->c8_registers[REGISTER_VF] = 1;
			}
		}

	}
}

static int c8_cpu_key(c8_vm_t* vm, uint8_t x, uint8_t key_state) {
	uint8_t key = (vm->c8_keypad >> vm->c8_registers[x]) & 1;

	switch(key_state) {
		case OPCODE_KEY_DOWN_MAP:
			if(key) vm->c8_program_counter += 2;
			break;
		case OPCODE_KEY_UP_MAP:
			if (!key) vm->c8_program_counter += 2;
			break;
		default:
			return -1;
	}

	return 0;
}

static void c8_cpu_key_read(c8_vm_t* vm, uint8_t x) {
	uint16_t keypad = vm->c8_keypad;

	if (keypad != 0) {
		uint8_t key = 0;

		while ((keypad & 1) != 0) {
			keypad >>= 1;
			key++;
		}

		vm->c8_registers[x] = key;
	} else {
		vm->c8_program_counter -= 2;
	}
}

static void c8_cpu_bcd(c8_vm_t* vm, uint8_t x) {
	uint8_t a = vm->c8_registers[x];
	vm->c8_memory[vm->c8_immediate] = (a / 100) % 10;
	vm->c8_memory[vm->c8_immediate + 1] = (a / 10) % 10;
	vm->c8_memory[vm->c8_immediate + 2] = a % 10;
}

static void c8_cpu_dump(c8_vm_t* vm, uint8_t x) {
	for (int i = 0; i <= x; i++) {
		vm->c8_memory[vm->c8_immediate++] = vm->c8_registers[i];
	}

	if(vm->c8_load_hack) {
		vm->c8_immediate -= x + 1;
	}
}

static void c8_cpu_load(c8_vm_t* vm, uint8_t x) {
	for (int i = 0; i <= x; i++) {
		vm->c8_registers[i] = vm->c8_memory[vm->c8_immediate++];
	}

	if(vm->c8_load_hack) {
		vm->c8_immediate -= x + 1;
	}
}

static void c8_cpu_ext(c8_vm_t* vm, uint8_t x, uint8_t opcode_ext_option) {
	switch (opcode_ext_option) {
		case OPCODE_TYPE_EXT_GET_DLY:
			vm->c8_registers[x] = vm->c8_delay_timer;
			break;
		case OPCODE_TYPE_EXT_KEY:
			c8_cpu_key_read(vm, x);
			break;
		case OPCODE_TYPE_EXT_DELAY:
			vm->c8_delay_timer = vm->c8_registers[x];
			break;
		case OPCODE_TYPE_EXT_SOUND:
			vm->c8_sound_timer = vm->c8_registers[x];
			break;
		case OPCODE_TYPE_EXT_IMM:
			vm->c8_immediate += vm->c8_registers[x];
			break;
		case OPCODE_TYPE_EXT_FONT:
			vm->c8_immediate = (vm->c8_registers[x] * 5) + FONT_ADDR;
			break;
		 case OPCODE_TYPE_EXT_BCD:
			 c8_cpu_bcd(vm, x);
			 break;
		case OPCODE_TYPE_EXT_DUMP:
			c8_cpu_dump(vm, x);
			break;
		case OPCODE_TYPE_EXT_LOAD:
			c8_cpu_load(vm, x);
			break;
		default:
			break;
	}
}

int c8_cpu_exec_instr(c8_vm_t* vm, uint16_t instr) {
	switch (instr & OPCODE_TYPE_MASK) {
	    case OPCODE_TYPE_NATIVE:
	    	c8_cpu_native(vm, instr & OPCODE_ADDR_MASK);
	        break;
	    case OPCODE_TYPE_JMP:
	    	c8_cpu_jump(vm, instr & OPCODE_ADDR_MASK);
	        break;
	    case OPCODE_TYPE_CALL:
	    	c8_cpu_call(vm, instr & OPCODE_ADDR_MASK);
	    	break;
	    case OPCODE_TYPE_JEQ_IMM:
	    	c8_cpu_jeq_imm(vm, OPCODE_REG_X_ARG(instr), instr & OPCODE_IMM_MASK);
	    	break;
	    case OPCODE_TYPE_JNEQ_IMM:
	    	c8_cpu_jneq_imm(vm, OPCODE_REG_X_ARG(instr), instr & OPCODE_IMM_MASK);
	    	break;
	    case OPCODE_TYPE_JEQ:
	    	c8_cpu_jeq(vm, OPCODE_REG_X_ARG(instr), OPCODE_REG_Y_ARG(instr));
	        break;
	    case OPCODE_TYPE_MOV_IMM:
	    	c8_cpu_mov_imm(vm, OPCODE_REG_X_ARG(instr), instr & OPCODE_IMM_MASK);
	        break;
	    case OPCODE_TYPE_ADD_IMM:
	    	c8_cpu_add_imm(vm, OPCODE_REG_X_ARG(instr), instr & OPCODE_IMM_MASK);
	    	break;
	    case OPCODE_TYPE_ALU:
	    	c8_cpu_alu(vm, OPCODE_REG_X_ARG(instr), OPCODE_REG_Y_ARG(instr), instr & OPCODE_ALU_OP_MASK);
	    	break;
	    case OPCODE_TYPE_JNEQ:
	    	c8_cpu_jneq(vm, OPCODE_REG_X_ARG(instr), OPCODE_REG_Y_ARG(instr));
	    	break;
	    case OPCODE_TYPE_MOV_I:
	    	c8_cpu_mov_i(vm, instr & OPCODE_ADDR_MASK);
	        break;
	    case OPCODE_TYPE_JMP_V0:
	    	c8_cpu_jmp_v0(vm, instr & OPCODE_ADDR_MASK);
	        break;
	    case OPCODE_TYPE_RAND:
	    	c8_cpu_rand(vm, OPCODE_REG_X_ARG(instr), instr & OPCODE_IMM_MASK);
	    	break;
	    case OPCODE_TYPE_DRAW:
	    	c8_cpu_draw(vm, OPCODE_REG_X_ARG(instr), OPCODE_REG_Y_ARG(instr), instr & OPCODE_SPRITE_H_MASK);
	    	break;
	    case OPCODE_TYPE_KEY:
	    	c8_cpu_key(vm, OPCODE_REG_X_ARG(instr), instr & OPCODE_KEY_MASK);
	    	break;
	    case OPCODE_TYPE_EXT:
	    	c8_cpu_ext(vm, OPCODE_REG_X_ARG(instr), instr & OPCODE_EXT_OP_MASK);
	    	break;
	    default:
	    	return -1;
	}

	return 0;
}

int c8_cpu_cycle(c8_vm_t* vm) {
	uint16_t instr = c8_cpu_fetch_instr(vm);
	c8_cpu_exec_instr(vm, instr);

	return 0;
}

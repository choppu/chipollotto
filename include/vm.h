/*
 * Copyright (C) 2019, Ksenia Balistreri
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 */

#ifndef _VM_H_
#define _VM_H_

#include <stdint.h>

#define MEMORY_SIZE 4096
#define REGISTERS_COUNT 16
#define REGISTER_VF 15
#define STACK_SIZE 16
#define FRAME_BUFFER_SIZE 256
#define FONT_ADDR 80
#define SCREEN_FB_WIDTH 8
#define SCREEN_HEIGHT 32
#define SCREEN_WIDTH 64

typedef struct {
	uint8_t c8_run;
	uint8_t c8_draw;
	uint8_t c8_shift_hack;
	uint8_t c8_load_hack;
	uint8_t c8_memory[MEMORY_SIZE];
	uint8_t c8_registers[REGISTERS_COUNT];
	uint8_t c8_frame_buffer[SCREEN_HEIGHT][SCREEN_FB_WIDTH];
	uint8_t c8_delay_timer;
	uint8_t c8_sound_timer;
	uint16_t c8_keypad;
	uint8_t c8_stack_counter;
	uint16_t c8_stack[STACK_SIZE];
	uint16_t c8_immediate;
	uint16_t c8_program_counter;
} c8_vm_t;

int c8_vm_run(c8_vm_t* vm);

#endif

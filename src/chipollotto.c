/*
 * Copyright (C) 2019, Ksenia Balistreri
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <SDL2/SDL.h>
#include "vm.h"

#define PROGRAMM_LOAD_ADDR 512

int read_program(const char* path, uint8_t* memory) {
	int fd = open(path, O_RDONLY);

	if (fd == -1) {
		return errno;
	}

	struct stat file_info;

	if (fstat(fd, &file_info) == -1) {
		close(fd);
		return errno;
	};

	if (read(fd, memory, file_info.st_size) == -1) {
		close(fd);
		return errno;
	};

	close(fd);

	return 0;
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		puts("Usage: chipollotto filename");
		return EXIT_FAILURE;
	}

	c8_vm_t vm;
	memset(&vm, 0, sizeof(vm));

	vm.c8_program_counter = PROGRAMM_LOAD_ADDR;

	if (read_program(argv[1], &vm.c8_memory[vm.c8_program_counter]) != 0) {
		puts("Error reading file");
		return EXIT_FAILURE;
	}

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER | SDL_INIT_EVENTS);
	c8_vm_run(&vm);
	SDL_Quit();

	return 0;
}

/*
 * Copyright (C) 2019, Ksenia Balistreri
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 */

#include <SDL2/SDL.h>
#include "keypad.h"

#define KEYMAP_SIZE 16

static SDL_Keycode c8_keymap[KEYMAP_SIZE] = {
		SDLK_x, SDLK_1, SDLK_2, SDLK_3,
		SDLK_q, SDLK_w, SDLK_e, SDLK_a,
		SDLK_s, SDLK_d, SDLK_z, SDLK_c,
		SDLK_4, SDLK_r, SDLK_f, SDLK_v
};

int c8_get_key(int key) {
	for (int i = 0; i < KEYMAP_SIZE; i++) {
		if (key == c8_keymap[i]) {
			return i;
		}
	}

	return -1;
}

int c8_keypad_scan(c8_vm_t* vm) {
	SDL_Event e;

    while (SDL_PollEvent(&e)){
        if (e.type == SDL_QUIT) {
            vm->c8_run = 0;
        } else if (e.type == SDL_KEYDOWN) {
        	int k = c8_get_key(e.key.keysym.sym);

            if (k != -1) {
            	vm->c8_keypad |= (1 << k);
            }
        } else if (e.type == SDL_KEYUP) {
        	int k = c8_get_key(e.key.keysym.sym);

        	if (k != -1) {
        		vm->c8_keypad &= ~(1 << k);
        	}
        }
    }

	return 0;
}

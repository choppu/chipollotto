/*
 * Copyright (C) 2019, Ksenia Balistreri
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 */

#include <SDL2/SDL.h>
#include "timer.h"

#define TIMER_INTERVAL_NS 16

long last_update;


void c8_timer_init() {
	last_update = SDL_GetTicks();
}

int c8_timer_update(c8_vm_t* vm) {
	uint32_t sys_time = SDL_GetTicks();

	if ((sys_time - last_update) >= TIMER_INTERVAL_NS) {
		if (vm->c8_delay_timer > 0) {
			vm->c8_delay_timer--;
		}

		if (vm->c8_sound_timer > 0) {
			vm->c8_sound_timer--;
		}

		last_update = sys_time;
	}
	return 0;
}

/*
 * Copyright (C) 2019, Ksenia Balistreri
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 */

#include <SDL2/SDL.h>
#include "display.h"

#define DISPLAY_SCALING 10
#define DISPLAY_SCREEN_WIDTH (SCREEN_WIDTH * DISPLAY_SCALING)
#define DISPLAY_SCREEN_HEIGHT (SCREEN_HEIGHT * DISPLAY_SCALING)
#define DISPLAY_COLOR_FG 0xB0E0E6
#define DISPLAY_COLOR_BG 0x2F4F4F
#define DISPLAY_SKIP_CYCLES 100

static SDL_Window* c8_display_window;
static SDL_Renderer *c8_display_renderer;
static SDL_Texture *c8_display_texture;
static int c8_skip_count;

void c8_display_init() {
	c8_skip_count = 0;
	c8_display_window = SDL_CreateWindow("Chipollotto", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, DISPLAY_SCREEN_WIDTH, DISPLAY_SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	c8_display_renderer = SDL_CreateRenderer(c8_display_window, -1, 0);
	SDL_RenderSetLogicalSize(c8_display_renderer, DISPLAY_SCREEN_WIDTH, DISPLAY_SCREEN_HEIGHT);
	c8_display_texture = SDL_CreateTexture(c8_display_renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
}

int c8_display_draw(c8_vm_t* vm) {
	if (vm->c8_draw) {
		uint32_t* pixels;
		int pitch;

		SDL_LockTexture(c8_display_texture, NULL, (void *) &pixels, &pitch);
		pitch /= sizeof(uint32_t);

		for (int i = 0; i < SCREEN_HEIGHT; i++) {
			for (int j = 0; j < SCREEN_FB_WIDTH; j++) {
				uint8_t pix8 = vm->c8_frame_buffer[i][j];

				pixels[(i * pitch) + (j * 8)] = (pix8 & 0x80) ? DISPLAY_COLOR_FG : DISPLAY_COLOR_BG;
				pixels[(i * pitch) + (j * 8) + 1] = (pix8 & 0x40) ? DISPLAY_COLOR_FG : DISPLAY_COLOR_BG;
				pixels[(i * pitch) + (j * 8) + 2] = (pix8 & 0x20) ? DISPLAY_COLOR_FG : DISPLAY_COLOR_BG;
				pixels[(i * pitch) + (j * 8) + 3] = (pix8 & 0x10) ? DISPLAY_COLOR_FG : DISPLAY_COLOR_BG;
				pixels[(i * pitch) + (j * 8) + 4] = (pix8 & 0x08) ? DISPLAY_COLOR_FG : DISPLAY_COLOR_BG;
				pixels[(i * pitch) + (j * 8) + 5] = (pix8 & 0x04) ? DISPLAY_COLOR_FG : DISPLAY_COLOR_BG;
				pixels[(i * pitch) + (j * 8) + 6] = (pix8 & 0x02) ? DISPLAY_COLOR_FG : DISPLAY_COLOR_BG;
				pixels[(i * pitch) + (j * 8) + 7] = (pix8 & 0x01) ? DISPLAY_COLOR_FG : DISPLAY_COLOR_BG;
			}
		}

		SDL_UnlockTexture(c8_display_texture);
		SDL_RenderClear(c8_display_renderer);

		vm->c8_draw = 0;
		c8_skip_count = 0;
	}

	if (c8_skip_count-- == 0) {
		SDL_RenderCopy(c8_display_renderer, c8_display_texture, NULL, NULL);
		SDL_RenderPresent(c8_display_renderer);
		c8_skip_count = DISPLAY_SKIP_CYCLES;
	}

	return 0;
}

void c8_display_destroy() {
	SDL_DestroyTexture(c8_display_texture);
	SDL_DestroyRenderer(c8_display_renderer);
	SDL_DestroyWindow(c8_display_window);
}

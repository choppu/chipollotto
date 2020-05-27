/*
 * Copyright (C) 2019, Ksenia Balistreri
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 */

#include <SDL2/SDL.h>
#include <math.h>
#include "audio.h"

#define SAMPLING_RATE 11250
#define SOUND_DURATION 16
#define SAMPLE_COUNT (SAMPLING_RATE / 1000) * SOUND_DURATION
#define TONE 440

static float samples[SAMPLE_COUNT];
static SDL_AudioDeviceID device;
static uint8_t last_timer_val;

static void c8_audio_generate_samples() {
	float phase;
	for (int i = 0; i < SAMPLE_COUNT; i++) {
		samples[i] = sinf(phase);
		phase = phase + ((2 * M_PI * TONE) / SAMPLING_RATE);

		if (phase > (2 * M_PI)) {
			phase = phase - (2 * M_PI);
		}
	}
}

void c8_audio_init() {
	SDL_AudioSpec want, have;

	c8_audio_generate_samples();
	last_timer_val = 0;

	SDL_memset(&want, 0, sizeof(want));
	want.freq = SAMPLING_RATE;
	want.format = AUDIO_F32SYS;
	want.channels = 1;
	want.samples = SAMPLE_COUNT;
	want.callback = NULL;

	device = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
	SDL_PauseAudioDevice(device, 0);
}


int c8_audio_play(c8_vm_t* vm) {
	if(vm->c8_sound_timer != last_timer_val) {
		SDL_QueueAudio(device, samples, sizeof(float) * SAMPLE_COUNT);
		last_timer_val = vm->c8_sound_timer;
	}

	return 0;
}

void c8_audio_destroy() {
	SDL_CloseAudioDevice(device);
}

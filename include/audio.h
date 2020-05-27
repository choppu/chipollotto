/*
 * Copyright (C) 2019, Ksenia Balistreri
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 */

#ifndef _AUDIO_H_
#define _AUDIO_H_

#include <stdint.h>
#include "vm.h"

void c8_audio_init();
int c8_audio_play(c8_vm_t* vm);
void c8_audio_destroy();

#endif

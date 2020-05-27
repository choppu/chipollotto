/*
 * Copyright (C) 2019, Ksenia Balistreri
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 */

#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include <stdint.h>
#include "vm.h"

void c8_display_init();
int c8_display_draw(c8_vm_t* vm);
void c8_display_destroy();

#endif

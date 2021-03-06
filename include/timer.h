/*
 * Copyright (C) 2019, Ksenia Balistreri
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 */

#ifndef _TIMER_H_
#define _TIMER_H_

#include <stdint.h>
#include "vm.h"

int c8_timer_update(c8_vm_t* vm);
void c8_timer_init();

#endif

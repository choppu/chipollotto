/*
 * Copyright (C) 2019, Ksenia Balistreri
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 */

#ifndef _KEYPAD_H_
#define _KEYPAD_H_

#include <stdint.h>
#include "vm.h"

int c8_get_key(int key);
int c8_keypad_scan(c8_vm_t* vm);

#endif

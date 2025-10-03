#pragma once

#include "types.h"

#define NUM_VALUES          16

#define GPIOC_MASK          0xf8
#define GPIOD_MASK          0x0c

#define GPIOC_OFF           GPIOC_MASK
#define GPIOD_OFF           GPIOD_MASK

#define GPIOC_LOADING       0xe8 /* dash */
#define GPIOC_CRASH         0x78 /* top segment */
#define GPIOC_DEBUG         0xb8 /* bottom segment */

void display_configure(void);
void display_values_init(void);
void display_value(uint8_t value);

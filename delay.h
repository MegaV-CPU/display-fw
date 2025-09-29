#pragma once

#include "types.h"

static inline void delay_ms(uint16_t ms) {
    for (uint16_t i = 0; i < ms; i++) {
        // Tuned loops
        for (uint8_t j = 0; j < 255; j++);
        for (uint8_t j = 0; j < 141; j++);
    }
}

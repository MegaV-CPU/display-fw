#pragma once

#include "types.h"

#define FAULT_CODE_IS_ERROR     (1 << 7)

#define FATAL_ERROR(FAULT_CODE) do { \
        fault_display_code(FAULT_CODE); \
        SW_RESET(); \
    } while (0)
#define ASSERT(COND, FAULT_CODE) do { \
        if (!(COND)) { \
            FATAL_ERROR(FAULT_CODE); \
        } \
    } while (0)

typedef enum {
    FAULT_CODE_NONE = 0,
    FAULT_CODE_ILLEGAL_INSTR = 3,
    FAULT_CODE_WATCHDOG = 5,
    FAULT_CODE_PROG_ERROR = FAULT_CODE_IS_ERROR | 7,
    FAULT_CODE_CLOCK_SWITCH_ERROR = FAULT_CODE_IS_ERROR | 9,
} fault_code_t;

void fault_start_watchdog(void);
void fault_display_code(fault_code_t code);
void fault_process_reset_code(void);

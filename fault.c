#include "fault.h"
#include "delay.h"
#include "display.h"
#include "types.h"

void fault_start_watchdog(void) {
    IWDG->KR = IWDG_KEY_ENABLE;
    IWDG->KR = IWDG_KEY_ACCESS;
    IWDG->PR = 6; // (~1 second timeout)
    IWDG_REFRESH();
}

void fault_display_code(fault_code_t code) {
    const uint8_t is_error = code & FAULT_CODE_IS_ERROR;
    const uint8_t value = code & ~FAULT_CODE_IS_ERROR;

    IWDG_REFRESH();
    GPIOC->ODR = GPIOC_OFF;
    delay_ms(500);

    IWDG_REFRESH();
    for (uint8_t i = 0; i < value; i++) {
        GPIOC->ODR = is_error ? (GPIOC_CRASH & GPIOC_DEBUG) : GPIOC_CRASH;
        delay_ms(200);

        GPIOC->ODR = GPIOC_OFF;
        delay_ms(200);

        IWDG_REFRESH();
    }

    delay_ms(500);
    IWDG_REFRESH();
}

void fault_process_reset_code(void) {
    // Get the fault code based on the reset register value and display it if applicable
    fault_code_t fault_code = FAULT_CODE_NONE;
    if (RST_SR & RST_SR_ILLOPF) {
        fault_code = FAULT_CODE_ILLEGAL_INSTR;
    } else if (RST_SR & RST_SR_IWDGF) {
        fault_code = FAULT_CODE_WATCHDOG;
    }
    RST_SR = RST_SR_CLEAR_VALUE;
    if (fault_code) {
        fault_display_code(fault_code);
    }
}

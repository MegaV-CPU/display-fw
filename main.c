#include "clock.h"
#include "delay.h"
#include "display.h"
#include "fault.h"
#include "types.h"

#define INITIAL_VALUE_COUNT     50
#define INPUT_VALUE() \
    ((GPIOD->IDR & 0x70) >> 4) | ((GPIOA->IDR & 0x2) << 2)

void main(void) {
    // Configure all the peripherals
    fault_start_watchdog();
    display_configure();
    clock_init();

    // Check the reset code and initialize the display values
    fault_process_reset_code();
    display_values_init();

    // Switch to the LSI clock for power saving
    clock_switch_to_lsi();

    // Cycle through all the values at first boot
    for (uint8_t value = 0; value < NUM_VALUES; value++) {
        for (uint8_t i = 0; i < INITIAL_VALUE_COUNT; i++) {
            display_value(value);
            IWDG_REFRESH();
        }
    }

    // Main loop
    while (1) {
        display_value(INPUT_VALUE());
        IWDG_REFRESH();
    }
}

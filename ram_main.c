#include "config.h"
#include "display.h"
#include "types.h"

#pragma codeseg RAM_SEG
void ram_main(void) {
    for (;;) {
#if MOCK_INPUT
        static uint8_t mock_value = 0;
        static uint8_t mock_update_delay = 0;
        if (++mock_update_delay == 5) {
            mock_value = (mock_value + 1) & 0xf;
            mock_update_delay = 0;
        }
        const uint8_t input_value = mock_value;
#else
        const uint8_t input_value = ((GPIOD->IDR & 0x70) >> 4) | ((GPIOA->IDR & 0x2) << 2);
#endif
        for (uint8_t step = 0; step < NUM_STEPS; step++) {
            GPIOC->ODR = g_gpioc_value[input_value][step];
            GPIOD->ODR = g_gpiod_value[input_value][step];
        }

        // Kick the watchdog
        IWDG->KR = IWDG_KEY_REFRESH;
    }
}

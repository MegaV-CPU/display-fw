#include "delay.h"
#include "display.h"
#include "types.h"

#define FAULT_CODE_IS_ERROR     (1 << 7)
#define INITIAL_VALUE_DELAY     100

#define FATAL_ERROR(FAULT_CODE) do { \
        display_fault_code(FAULT_CODE); \
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

static void display_fault_code(uint8_t code) {
    const uint8_t is_error = code & FAULT_CODE_IS_ERROR;
    code &= ~FAULT_CODE_IS_ERROR;
    IWDG_REFRESH();
    GPIOC->ODR = GPIOC_OFF;
    delay_ms(500);
    IWDG_REFRESH();
    for (uint8_t i = 0; i < code; i++) {
        GPIOC->ODR = is_error ? (GPIOC_CRASH & GPIOC_DEBUG) : GPIOC_CRASH;
        delay_ms(200);
        GPIOC->ODR = GPIOC_OFF;
        delay_ms(200);
        IWDG_REFRESH();
    }
    delay_ms(500);
    IWDG_REFRESH();
}

static void process_reset_fault_code(void) {
    // Get the boot code based on the reset register value and display it if applicable
    fault_code_t fault_code = FAULT_CODE_NONE;
    if (RST_SR & RST_SR_ILLOPF) {
        fault_code = FAULT_CODE_ILLEGAL_INSTR;
    } else if (RST_SR & RST_SR_IWDGF) {
        fault_code = FAULT_CODE_WATCHDOG;
    }
    RST_SR = RST_SR_CLEAR_VALUE;
    if (fault_code) {
        display_fault_code(fault_code);
    }
}

static void switch_to_lsi(void) {
    // Check if we need to set the option byte
    if (!(OPT3->OPT & OPT3_LSI_EN)) {
        // Unlock the data memory
        FLASH->DUKR = FLASH_RASS_KEY2;
        FLASH->DUKR = FLASH_RASS_KEY1;
        while (!(FLASH->IAPSR & FLASH_IAPSR_DUL));

        // Enable write access to the option bytes
        FLASH->CR2 |= FLASH_CR2_OPT;
        FLASH->NCR2 &= ~FLASH_CR2_OPT;

        // Set the value and its complement
        const uint8_t value = OPT3->OPT | OPT3_LSI_EN;
        OPT3->OPT = value;
        OPT3->NOPT = ~value;

        // Wait for the operation to complete or write protection bit to be set
        while (!(FLASH->IAPSR & (FLASH_IAPSR_EOP | FLASH_IAPSR_SR_PG_DIS)));
        ASSERT(!(FLASH->IAPSR & FLASH_IAPSR_SR_PG_DIS), FAULT_CODE_PROG_ERROR);

        // Disable write access to the option bytes
        FLASH->CR2 &= ~FLASH_CR2_OPT;
        FLASH->NCR2 |= FLASH_CR2_OPT;

        // Lock the memory
        FLASH->IAPSR &= ~FLASH_IAPSR_DUL;

        // Validate
        ASSERT(OPT3->OPT & OPT3_LSI_EN, FAULT_CODE_PROG_ERROR);
    }

    // Wait for the LSI to be ready
    while (!(CLK->ICKR & CLK_ICKR_LSIRDY));

    // Start switching to the LSI
    IWDG_REFRESH();
    CLK->SWCR = CLK_SWCR_SWEN;
    CLK->SWR = CLK_SWR_LSI;

    // Wait for the switching to complete
    while (CLK->SWCR & CLK_SWCR_SWBSY);
    while (CLK->CMSR != CLK_SWR_LSI);
}

void main(void) {
    // Configure the display pins
    GPIOC->ODR = GPIOC_OFF;
    GPIOD->ODR = GPIOD_OFF;
    GPIOC->DDR = GPIOC_MASK;
    GPIOD->DDR = GPIOD_MASK;

    process_reset_fault_code();

    // Show a dash while we boot up
    GPIOC->ODR = GPIOC_LOADING;

    // Configure and enable the watchdog (1 second timeout)
    IWDG->KR = IWDG_KEY_ENABLE;
    IWDG->KR = IWDG_KEY_ACCESS;
    IWDG->PR = 6;
    IWDG_REFRESH();

    // Disable all peripheral clocks
    CLK->PCKENR1 = 0;
    CLK->PCKENR2 = 0;

    // Run our initialization code
    display_values_init();

    // Switch to the LSI clock
    switch_to_lsi();

    uint8_t initial_value = 0;
    uint8_t initial_value_delay = 0;
    while (1) {
        uint8_t input_value;
        if (initial_value < NUM_VALUES) {
            // Cycle through all the values at first boot
            input_value = initial_value;
            if (++initial_value_delay == INITIAL_VALUE_DELAY) {
                initial_value++;
                initial_value_delay = 0;
            }
        } else {
            input_value = ((GPIOD->IDR & 0x70) >> 4) | ((GPIOA->IDR & 0x2) << 2);
        }
        for (uint8_t step = 0; step < NUM_STEPS; step++) {
            GPIOC->ODR = g_gpioc_value[input_value][step];
            GPIOD->ODR = g_gpiod_value[input_value][step];
        }
        IWDG_REFRESH();
    }
}

#include "delay.h"
#include "display.h"
#include "types.h"

typedef enum {
    BOOT_CODE_NORMAL = 0,
    BOOT_CODE_ILLEGAL_INSTR = 3,
    BOOT_CODE_WATCHDOG = 5,
} boot_code_t;

volatile uint16_t RAM_SEG_LEN;
static uint8_t m_ram_code_data[255];
static void (*m_ram_main_ptr)(void);

static inline void get_ram_seg_len(void) {
    __asm__("pushw x");
    __asm__("ldw x, #l_RAM_SEG");
    __asm__("ldw _RAM_SEG_LEN, x");
    __asm__("popw x");
}

static void copy_ram_main(void) {
    get_ram_seg_len();
    if (RAM_SEG_LEN > sizeof(m_ram_code_data)) while (1);

    // Copy the ram_main function into m_ram_code_data
    extern void ram_main(void);
    for (uint16_t i = 0; i < RAM_SEG_LEN; i++) {
        m_ram_code_data[i] = ((uint8_t*)ram_main)[i];
    }
    m_ram_main_ptr = (void(*)(void))m_ram_code_data;
}

void main(void) {
    // Disable all peripheral clocks
    CLK_PCKENR1 = 0;
    CLK_PCKENR2 = 0;

    // Configure the display pins
    GPIOC->ODR = GPIOC_OFF;
    GPIOD->ODR = GPIOD_OFF;
    GPIOC->DDR = GPIOC_MASK;
    GPIOD->DDR = GPIOD_MASK;

    // Get the boot code based on the reset register value and signal it if applicable
    boot_code_t boot_code = BOOT_CODE_NORMAL;
    if (RST_SR & RST_SR_ILLOPF) {
        RST_SR = RST_SR_ILLOPF;
        boot_code = BOOT_CODE_ILLEGAL_INSTR;
    }
    if (RST_SR & RST_SR_IWDGF) {
        RST_SR = RST_SR_IWDGF;
        boot_code = BOOT_CODE_WATCHDOG;
    }
    for (uint8_t i = 0; i < boot_code; i++) {
        GPIOC->ODR = GPIOC_CRASH;
        delay_ms(200);
        GPIOC->ODR = GPIOC_OFF;
        delay_ms(200);
    }

    // Show a dash while we boot up
    GPIOC->ODR = GPIOC_LOADING;

    // Configure and enable the watchdog (1 second timeout)
    IWDG->KR = IWDG_KEY_ENABLE;
    IWDG->KR = IWDG_KEY_ACCESS;
    IWDG->PR = 6;
    IWDG->KR = IWDG_KEY_REFRESH;

    display_values_init();
    copy_ram_main();

    // Run the main code from RAM
    m_ram_main_ptr();

    // Should never get here
    GPIOC->ODR = GPIOC_CRASH;
    for (;;) {}
}

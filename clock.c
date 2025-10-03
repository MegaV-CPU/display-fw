#include "clock.h"

#include "fault.h"
#include "types.h"

static void program_lsi_en_option_byte(void) {
    if (OPT3->OPT & OPT3_LSI_EN) {
        return;
    }

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

void clock_init(void) {
    // Disable all peripheral clocks
    CLK->PCKENR1 = 0;
    CLK->PCKENR2 = 0;
}

void clock_switch_to_lsi(void) {
    program_lsi_en_option_byte();

    // Wait for the LSI to be ready
    while (!(CLK->ICKR & CLK_ICKR_LSIRDY));

    // Start switching to the LSI
    IWDG_REFRESH();
    CLK->SWCR = CLK_SWCR_SWEN;
    CLK->SWR = CLK_SWR_LSI;

    // Wait for the switching to complete
    while (CLK->SWCR & CLK_SWCR_SWBSY);
    while (CLK->CMSR != CLK_SWR_LSI);

    // Disable the HSI
    CLK->ICKR &= ~CLK_ICKR_HSIEN;
    ASSERT(!(CLK->ICKR & CLK_ICKR_HSIEN), FAULT_CODE_CLOCK_SWITCH_ERROR);
}

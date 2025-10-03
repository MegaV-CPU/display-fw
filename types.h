#pragma once

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;

#define RST_SR                  (*(volatile uint8_t*)0x50B3)
#define RST_SR_ILLOPF           (1 << 2)
#define RST_SR_IWDGF            (1 << 1)
#define RST_SR_CLEAR_VALUE      0x1f

typedef struct {
    volatile uint8_t ICKR;
    volatile uint8_t ECKR;
    uint8_t _reserved1;
    volatile uint8_t CMSR;
    volatile uint8_t SWR;
    volatile uint8_t SWCR;
    volatile uint8_t CLKDIVR;
    volatile uint8_t PCKENR1;
    volatile uint8_t CSSR;
    volatile uint8_t CCOR;
    volatile uint8_t PCKENR2;
    uint8_t _reserved2;
    volatile uint8_t HSITRIMR;
    volatile uint8_t SWIMCCR;
} clk_t;

#define CLK                     ((clk_t*)0x50C0)

#define CLK_ICKR_LSIRDY         (1 << 4)
#define CLK_SWCR_SWEN           (1 << 1)
#define CLK_SWCR_SWBSY          (1 << 0)
#define CLK_SWR_LSI             0xD2

typedef struct {
    volatile uint8_t ODR;
    volatile uint8_t IDR;
    volatile uint8_t DDR;
    volatile uint8_t CR1;
    volatile uint8_t CR2;
} gpio_port_t;

#define GPIOA                   ((gpio_port_t*)0x5000)
#define GPIOC                   ((gpio_port_t*)0x500A)
#define GPIOD                   ((gpio_port_t*)0x500F)

typedef struct {
    volatile uint8_t KR;
    volatile uint8_t PR;
    volatile uint8_t RLR;
} iwdg_t;

#define IWDG                    ((iwdg_t*)0x50E0)

#define IWDG_KEY_ENABLE         0xCC
#define IWDG_KEY_REFRESH        0xAA
#define IWDG_KEY_ACCESS         0x55

#define IWDG_REFRESH() \
    IWDG->KR = IWDG_KEY_REFRESH

typedef struct {
    volatile uint8_t OPT;
    volatile uint8_t NOPT;
} option_byte_t;

#define OPT3                    ((option_byte_t*)0x4805)
#define OPT3_LSI_EN             (1 << 3)

typedef struct {
  volatile uint8_t CR1;
  volatile uint8_t CR2;
  volatile uint8_t NCR2;
  volatile uint8_t FPR;
  volatile uint8_t NFPR;
  volatile uint8_t IAPSR;
  uint8_t _reserved1;
  uint8_t _reserved2;
  volatile uint8_t PUKR;
  uint8_t _reserved3;
  volatile uint8_t DUKR;
} flash_t;

#define FLASH                   ((flash_t*)0x505A)
#define FLASH_RASS_KEY1         0x56
#define FLASH_RASS_KEY2         0xAE
#define FLASH_MEMTYPE_DATA      0xF7
#define FLASH_CR2_OPT           (1 << 7)
#define FLASH_IAPSR_DUL         (1 << 3)
#define FLASH_IAPSR_EOP         (1 << 2)
#define FLASH_IAPSR_SR_PG_DIS   (1 << 0)

#define SW_RESET() do { \
        (*(volatile uint8_t*)0x50D1) = 0x80; \
        while (1); /* shouldn't get here */ \
    } while (0)

#pragma once

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;

#define RST_SR              (*(volatile uint8_t*)0x50B3)
#define RST_SR_ILLOPF       (1 << 2)
#define RST_SR_IWDGF        (1 << 1)

#define CLK_PCKENR1         (*(volatile uint8_t*)0x50C7)
#define CLK_PCKENR2         (*(volatile uint8_t*)0x50CA)

typedef struct {
    volatile uint8_t ODR;
    volatile uint8_t IDR;
    volatile uint8_t DDR;
    volatile uint8_t CR1;
    volatile uint8_t CR2;
} gpio_port_t;

#define GPIOA               ((gpio_port_t*)0x5000)
#define GPIOC               ((gpio_port_t*)0x500A)
#define GPIOD               ((gpio_port_t*)0x500F)

typedef struct {
    volatile uint8_t KR;
    volatile uint8_t PR;
    volatile uint8_t RLR;
} iwdg_t;

#define IWDG                ((iwdg_t*)0x50E0)

#define IWDG_KEY_ENABLE     0xcc
#define IWDG_KEY_REFRESH    0xaa
#define IWDG_KEY_ACCESS     0x55

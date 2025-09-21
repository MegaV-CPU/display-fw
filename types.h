#pragma once

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;

typedef struct {
    volatile uint8_t ODR;
    volatile uint8_t IDR;
    volatile uint8_t DDR;
    volatile uint8_t CR1;
    volatile uint8_t CR2;
} gpio_port_t;

#define GPIOA ((gpio_port_t*)0x5000)
#define GPIOC ((gpio_port_t*)0x500A)
#define GPIOD ((gpio_port_t*)0x500F)

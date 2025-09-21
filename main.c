#include "types.h"

#define SEGMENT_ON_TIME_MS  1

#define GPIOC_MASK          0xf8
#define GPIOD_MASK          0x0c

#define GPIOC_OFF           GPIOC_MASK
#define GPIOD_OFF           GPIOD_MASK

#define MASK_A              (1 << 0)
#define MASK_B              (1 << 1)
#define MASK_C              (1 << 2)
#define MASK_D              (1 << 3)
#define MASK_E              (1 << 4)
#define MASK_F              (1 << 5)
#define MASK_G              (1 << 6)

static const uint8_t GPIOC_SEGMENT_ON_VALUES[7] = {
    0x78, // A
    GPIOC_OFF, // B
    GPIOC_OFF, // C
    0xb8, // D
    0xd8, // E
    0xf0, // F
    0xe8, // G
};

static const uint8_t GPIOD_SEGMENT_ON_VALUES[7] = {
    GPIOD_OFF, // A
    0x08, // B
    0x04, // C
    GPIOD_OFF, // D
    GPIOD_OFF, // E
    GPIOD_OFF, // F
    GPIOD_OFF, // G
};

static const uint8_t SEGMENT_MASK[16] = {
    [0x0] = MASK_A | MASK_B | MASK_C | MASK_D | MASK_E | MASK_F,
    [0x1] = MASK_B | MASK_C,
    [0x2] = MASK_A | MASK_B | MASK_D | MASK_E | MASK_G,
    [0x3] = MASK_A | MASK_B | MASK_C | MASK_D | MASK_G,
    [0x4] = MASK_B | MASK_C | MASK_F | MASK_G,
    [0x5] = MASK_A | MASK_C | MASK_D | MASK_F | MASK_G,
    [0x6] = MASK_A | MASK_C | MASK_D | MASK_E | MASK_F | MASK_G,
    [0x7] = MASK_A | MASK_B | MASK_C,
    [0x8] = MASK_A | MASK_B | MASK_C | MASK_D | MASK_E | MASK_F | MASK_G,
    [0x9] = MASK_A | MASK_B | MASK_C | MASK_D | MASK_F | MASK_G,
    [0xa] = MASK_A | MASK_B | MASK_C | MASK_D | MASK_E | MASK_G,
    [0xb] = MASK_C | MASK_D | MASK_E | MASK_F | MASK_G,
    [0xc] = MASK_A | MASK_D | MASK_E | MASK_F,
    [0xd] = MASK_B | MASK_C | MASK_D | MASK_E | MASK_G,
    [0xe] = MASK_A | MASK_D | MASK_E | MASK_F | MASK_G,
    [0xf] = MASK_A | MASK_B | MASK_E | MASK_F | MASK_G,
};

static void delay_us(uint16_t us) {
    // TODO: Tune this
    for (uint16_t i = 0; i < us; i++);
}

static void delay_ms(uint16_t ms) {
    // TODO: Tune this
    for (uint16_t i = 0; i < ms; i++) delay_us(1000);
}

static inline void display_off(void) {
    GPIOC->ODR = GPIOC_OFF;
    GPIOD->ODR = GPIOD_OFF;
}

static void draw_value(uint8_t value) {
    const uint8_t ON_TIME_MS = 1;
    const uint8_t mask = SEGMENT_MASK[value];
    for (uint8_t i = 0; i < 7; i++) {
        if (mask & (1 << i)) {
            GPIOC->ODR = GPIOC_SEGMENT_ON_VALUES[i];
            GPIOD->ODR = GPIOD_SEGMENT_ON_VALUES[i];
        } else {
            display_off();
        }
        delay_ms(ON_TIME_MS);
    }
    display_off();
}

static inline uint8_t read_input(void) {
    return ((GPIOD->IDR & 0x70) >> 4) | ((GPIOA->IDR & 0x2) << 2);
}

void main() {
    // Configure the display pins
    display_off();
    GPIOC->DDR = GPIOC_MASK;
    GPIOC->DDR = GPIOD_MASK;

    while (1) {
        GPIOC->ODR &= ~(1 << 3);
        delay_ms(500);
        GPIOC->ODR |= (1 << 3);
        delay_ms(500);
    }
}

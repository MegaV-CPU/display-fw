#include "display.h"

#define NUM_STEPS           4
#define STEP_ON_TIME        20

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
static const uint8_t SEGMENT_MASK[NUM_VALUES] = {
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
    [0xa] = MASK_A | MASK_B | MASK_C | MASK_E | MASK_F | MASK_G,
    [0xb] = MASK_C | MASK_D | MASK_E | MASK_F | MASK_G,
    [0xc] = MASK_A | MASK_D | MASK_E | MASK_F,
    [0xd] = MASK_B | MASK_C | MASK_D | MASK_E | MASK_G,
    [0xe] = MASK_A | MASK_D | MASK_E | MASK_F | MASK_G,
    [0xf] = MASK_A | MASK_E | MASK_F | MASK_G,
};

static uint8_t m_gpioc_value[NUM_VALUES][NUM_STEPS];
static uint8_t m_gpiod_value[NUM_VALUES][NUM_STEPS];

void display_configure(void) {
    // Configure the display pins
    GPIOC->ODR = GPIOC_OFF;
    GPIOD->ODR = GPIOD_OFF;
    GPIOC->DDR = GPIOC_MASK;
    GPIOD->DDR = GPIOD_MASK;

    // Show a dash while we boot up
    GPIOC->ODR = GPIOC_LOADING;
}

void display_values_init(void) {
    // Precompute all the values
    for (uint8_t value = 0; value < NUM_VALUES; value++) {
        const uint8_t mask = SEGMENT_MASK[value];
        uint8_t segment = 0;
        for (uint8_t step = 0; step < NUM_STEPS; step++) {
            uint8_t c_value = GPIOC_OFF;
            uint8_t d_value = GPIOD_OFF;
            const uint8_t step_last_segment = ((step + 1) * 7) / NUM_STEPS - 1;
            while (segment <= step_last_segment) {
                if (mask & (1 << segment)) {
                    c_value &= GPIOC_SEGMENT_ON_VALUES[segment];
                    d_value &= GPIOD_SEGMENT_ON_VALUES[segment];
                }
                segment++;
            }
            m_gpioc_value[value][step] = c_value;
            m_gpiod_value[value][step] = d_value;
        }
    }
}

void display_value(uint8_t value) {
    for (uint8_t step = 0; step < NUM_STEPS; step++) {
        GPIOC->ODR = m_gpioc_value[value][step];
        GPIOD->ODR = m_gpiod_value[value][step];

        for (uint8_t i = 0; i < STEP_ON_TIME; i++) {}

        GPIOC->ODR = GPIOC_OFF;
        GPIOD->ODR = GPIOD_OFF;
    }
}

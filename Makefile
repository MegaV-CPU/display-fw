MCU = stm8s003f3
CC = sdcc
STM8FLASH = stm8flash

SRC = \
	main.c \
	ram_main.c \
	display.c

BUILD_DIR = build
OBJ = $(addprefix $(BUILD_DIR)/, $(SRC:.c=.rel))
TARGET = $(BUILD_DIR)/main.ihx
CFLAGS = -mstm8 --out-fmt-ihx

all: $(TARGET)

$(BUILD_DIR):
	@mkdir -p $@

$(BUILD_DIR)/%.rel: %.c | $(BUILD_DIR)
	@echo "Compiling $(notdir $@)"
	@$(CC) $(CFLAGS) -I./ -c $^ -o $@

$(TARGET): $(OBJ)
	@echo "Linking $(notdir $@)"
	@$(CC) $(CFLAGS) -o $(TARGET) $^

flash: $(TARGET)
	@echo "Flashing $(notdir $<)"
	@sudo $(STM8FLASH) -c stlinkv2 -p $(MCU) -s flash -w $(TARGET)

clean:
	@rm -rf $(BUILD_DIR)

.PHONY: clean

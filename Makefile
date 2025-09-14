MCU = stm8s003f3
SDCC = sdcc
STM8FLASH = stm8flash

SRC = \
	main.c
OBJ = $(addprefix build/, $(SRC:.c=.rel))
OUTPUT_BINARY = build/main.ihx

SDCCFLAGS = --std-c99 -mstm8

build/:
	@mkdir -p $@

build/%.rel: %.c build/
	@$(SDCC) -c $(SDCFLAGS) $<

build/%.ihx: $(OBJ)
	@$(SDCC) $(SDCCFLAGS) --out-fmt-ihx $<

build: $(OUTPUT_BINARY)

flash: $(OUTPUT_BINARY)
	@$(STM8FLASH) -c stlinkv2 -p $(MCU) -w $<

clean:
	@rm -rf build/

.DEFAULT_GOAL := build
.PHONY: build flash clean

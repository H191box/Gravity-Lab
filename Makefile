# Gravity Lab - GBA Racing Puzzle Game
# Makefile for devkitARM

# Toolchain
CC      = arm-none-eabi-gcc
AS      = arm-none-eabi-as
OBJCOPY = arm-none-eabi-objcopy

# devkitARM paths
LIBGBA  := $(shell $(CC) -print-file-name=libgba.a)
LIBC    := $(shell $(CC) -print-file-name=libc.a)

# Project paths
SRCDIR  = src
BUILDDIR = build
TARGET  = gravity_lab

# Source files
C_SRCS  = $(wildcard $(SRCDIR)/main/*.c) \
          $(wildcard $(SRCDIR)/engine/*.c) \
          $(wildcard $(SRCDIR)/game/*.c) \
          $(wildcard $(SRCDIR)/assets/*.c) \
          $(wildcard $(SRCDIR)/data/*.c)

# Object files
C_OBJS  = $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(C_SRCS))

# GBA header object (minimal GBA ROM header)
HEADER_OBJ = $(BUILDDIR)/gba_header.o

# All objects
OBJS = $(HEADER_OBJ) $(C_OBJS)

# Output
ELF  = $(BUILDDIR)/$(TARGET).elf
ROM  = $(BUILDDIR)/$(TARGET).gba

# Compiler flags
CFLAGS  = -mthumb-interwork -mthumb -O2 -Wall -Wextra \
          -I$(SRCDIR) -Iinclude \
          -ffreestanding -nostdlib -std=c99

ASFLAGS = -mthumb-interwork -mthumb

LDFLAGS = -mthumb-interwork -mthumb -T linker.ld \
          -nostdlib -lgba $(LIBGBA)

# Default target
all: $(ROM)

# Link ELF
$(ELF): $(OBJS) linker.ld
	@mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBGBA)

# Copy to GBA ROM
$(ROM): $(ELF)
	$(OBJCOPY) -O binary $< $@
	@echo "=== ROM built: $@ ==="
	@ls -la $@

# Compile C sources
$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

# GBA header (fixed ROM entry point)
$(HEADER_OBJ): $(BUILDDIR)/gba_header.s
	$(AS) $(ASFLAGS) -o $@ $<

$(BUILDDIR)/gba_header.s:
	@mkdir -p $(dir $@)
	@echo '.section .rom_header' > $@
	@echo '.word 0x24000000' >> $@       # GBA ROM entry point
	@echo '.word 0x00000000' >> $@       # Logo (placeholder)
	@echo '.ascii "GRAVLAB  "' >> $@     # Game title
	@echo '.word 0x00000000' >> $@       # Game code
	@echo '.word 0x00000000' >> $@       # Maker code
	@echo '.byte 0x96' >> $@             # Fixed value
	@echo '.byte 0x00' >> $@             # Main unit code
	@echo '.byte 0x00' >> $@             # Device type
	@echo '.byte 0x07' >> $@             # Reserved
	@echo '.byte 0x00' >> $@             # Software version
	@echo '.byte 0x00' >> $@             # Complement check
	@echo '.word 0x00000000' >> $@       # Reserved

# Create linker script
linker.ld:
	@echo "OUTPUT_ARCH(arm)" > $@
	@echo "ENTRY(_start)" >> $@
	@echo "SECTIONS {" >> $@
	@echo "  . = 0x08000000;" >> $@
	@echo "  .text : { *(.rom_header) *(.text .text.*) }" >> $@
	@echo "  .rodata : { *(.rodata .rodata.*) }" >> $@
	@echo "  .data : { *(.data .data.*) }" >> $@
	@echo "  .bss : { *(.bss .bss.*) __bss_end = .; }" >> $@
	@echo "  . = 0x0E000000;" >> $@
	@echo "  .sram (NOLOAD) : { *(.sram .sram.*) }" >> $@
	@echo "}" >> $@

# Clean
clean:
	rm -rf $(BUILDDIR)

# Rebuild
rebuild: clean all

.PHONY: all clean rebuild
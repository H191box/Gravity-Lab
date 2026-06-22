# ============================================================
#  Gravity Lab - GBA Racing Puzzle Game
#  Makefile for devkitARM (devkitPro)
#
#  PATRÓN basado en H191box/gems — usa gba.specs de devkitPro
#  que maneja automáticamente: ROM header, entry stub ARM→Thumb,
#  BSS zeroing, .data copy, e IRQ/VBlank init via libgba.
# ============================================================

# --- Toolchain (devkitPro) ---
DEVKITPRO ?= /opt/devkitpro
DEVKITARM ?= $(DEVKITPRO)/devkitARM
LIBGBA    ?= $(DEVKITPRO)/libgba

CC      := $(DEVKITARM)/bin/arm-none-eabi-gcc
OBJCOPY := $(DEVKITARM)/bin/arm-none-eabi-objcopy

# --- Project paths ---
SRCDIR  = src
BUILDDIR = build
TARGET  = gravity_lab

# --- Sources ---
SOURCES := $(wildcard $(SRCDIR)/main/*.c) \
           $(wildcard $(SRCDIR)/engine/*.c) \
           $(wildcard $(SRCDIR)/game/*.c) \
           $(wildcard $(SRCDIR)/assets/*.c) \
           $(wildcard $(SRCDIR)/data/*.c)

OBJECTS := $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(SOURCES))

# --- Output ---
ROM = $(TARGET).gba

# -------------------------------------------------------
#  COMPILER FLAGS
#  -mcpu=arm7tdmi: target GBA CPU exactly
#  -ffunction-sections -fdata-sections: dead code elimination
# -------------------------------------------------------
CFLAGS := \
	-mthumb \
	-mthumb-interwork \
	-mcpu=arm7tdmi \
	-O2 \
	-ffunction-sections \
	-fdata-sections \
	-fno-common \
	-I$(LIBGBA)/include \
	-I$(SRCDIR) \
	-I$(SRCDIR)/engine \
	-I$(SRCDIR)/game \
	-I$(SRCDIR)/assets \
	-I$(SRCDIR)/data \
	-Iinclude \
	-Wall -Wextra \
	-std=gnu99

# -------------------------------------------------------
#  LINKER FLAGS
#  CRÍTICO: -specs=gba.specs maneja todo el boot:
#    - ROM header (Nintendo logo, entry point)
#    - ARM→Thumb stub
#    - .data ROM→EWRAM copy
#    - .bss zeroing
#    - Stack pointer setup
#  -lgba: proporciona IRQ handlers, VBlank, etc.
# -------------------------------------------------------
LDFLAGS := \
	-mthumb \
	-mthumb-interwork \
	-mcpu=arm7tdmi \
	-specs=gba.specs \
	-L$(LIBGBA)/lib \
	-lgba \
	-Wl,--gc-sections \
	-Wl,-Map,$(BUILDDIR)/$(TARGET).map

# -------------------------------------------------------
#  BUILD RULES
# -------------------------------------------------------
all: $(BUILDDIR) $(ROM)

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(ROM): $(OBJECTS)
	$(CC) -mthumb -mthumb-interwork -mcpu=arm7tdmi -specs=gba.specs -Wl,--gc-sections -Wl,-Map,$(BUILDDIR)/$(TARGET).map -o $(BUILDDIR)/$(TARGET).elf $(OBJECTS) -L$(LIBGBA)/lib -lgba
	$(OBJCOPY) -O binary $(BUILDDIR)/$(TARGET).elf $(ROM)
	@echo "=== ROM built: $(ROM) ==="
	@ls -la $(ROM)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

# -------------------------------------------------------
#  UTILITIES
# -------------------------------------------------------
clean:
	rm -rf $(BUILDDIR) $(ROM)

rebuild: clean all

.PHONY: all clean rebuild

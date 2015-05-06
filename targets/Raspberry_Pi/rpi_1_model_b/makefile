################################################################################
#
#       Autor:      Jan Johansson (ejanjoh)
#       Copyright:  Copyright (c) Jan Johansson (ejanjoh). All rights reserved.
#       Created:    2013-04-09
#       Updated:    2015-05-04
#
#       bOS version history mapped on changes in this file:
#       ---------------------------------------------------
#       ver2:    Adjusted for bOS
#
################################################################################


# Toolchain:
TOOLCHAIN           ?=~/bin/tools/cross/linaro/gcc-arm-none-eabi-4_7-2013q3/bin/

ARM_AS              ?= $(TOOLCHAIN)arm-none-eabi-as
ARM_CC              ?= $(TOOLCHAIN)arm-none-eabi-gcc
ARM_LD              ?= $(TOOLCHAIN)arm-none-eabi-ld
ARM_OC              ?= $(TOOLCHAIN)arm-none-eabi-objcopy
ARM_OD              ?= $(TOOLCHAIN)arm-none-eabi-objdump

HEX_DUMP            ?=~/bin/tools/hexdump

# Main directories:
TARGET              = ./
GENERIC             = ./../../../

# Subdirectories @ TARGET:
BUILD_FILES         = $(TARGET)build/
EXTRA               = $(TARGET)extra/
ASM_FILES_HW_DEP    = $(TARGET)src/asm/
C_FILES_HW_DEP      = $(TARGET)src/c/
HEADER_FILES_HW_DEP = $(TARGET)src/header/

# Subdirectories @ GENERIC:
C_FILES_GEN         = $(GENERIC)src/c/
HEADER_FILES_GEN    = $(GENERIC)src/header/

# Linker script:
LINKER              = kernel.ld

# Output files:
IMAGE               = kernel.img
LIST                = kernel.list
MAP                 = kernel.map
ELF                 = kernel.elf
HEX                 = kernel.hexdump.txt
OBJ                 := $(patsubst $(ASM_FILES_HW_DEP)%.s,$(BUILD_FILES)%.o,$(wildcard $(ASM_FILES_HW_DEP)*.s))
OBJ                 += $(patsubst $(C_FILES_HW_DEP)%.c,$(BUILD_FILES)%.o,$(wildcard $(C_FILES_HW_DEP)*.c))
OBJ                 += $(patsubst $(C_FILES_GEN)%.c,$(BUILD_FILES)%.o,$(wildcard $(C_FILES_GEN)*.c))

# Build flags:
#DEPENDFLAGS := -MD -MP                                                 <--- No
INCLUDES    := -I $(HEADER_FILES_HW_DEP)
INCLUDES    += -I $(HEADER_FILES_GEN)
#BASEFLAGS   := -O2 -nostdlib -pedantic -pedantic-errors                <--- No
BASEFLAGS   := -nostdlib
BASEFLAGS   += -nostartfiles -ffreestanding -nodefaultlibs
BASEFLAGS   += -fno-builtin -fomit-frame-pointer -mcpu=arm1176jzf-s
WARNFLAGS   := -Wall -Wextra -Wshadow -Wcast-align -Wwrite-strings
WARNFLAGS   += -Wredundant-decls -Winline
WARNFLAGS   += -Wno-attributes -Wno-deprecated-declarations
WARNFLAGS   += -Wno-div-by-zero -Wno-endif-labels -Wfloat-equal
WARNFLAGS   += -Wformat=2 -Wno-format-extra-args -Winit-self
WARNFLAGS   += -Winvalid-pch -Wmissing-format-attribute
WARNFLAGS   += -Wmissing-include-dirs -Wno-multichar
WARNFLAGS   += -Wredundant-decls -Wshadow
WARNFLAGS   += -Wno-sign-compare -Wswitch -Wsystem-headers -Wundef
WARNFLAGS   += -Wno-pragmas -Wno-unused-but-set-parameter
WARNFLAGS   += -Wno-unused-but-set-variable -Wno-unused-result
WARNFLAGS   += -Wwrite-strings -Wdisabled-optimization -Wpointer-arith
WARNFLAGS   += -Werror
ASFLAGS     := $(INCLUDES) $(DEPENDFLAGS) -D__ASSEMBLY__
CFLAGS      := $(INCLUDES) $(DEPENDFLAGS) $(BASEFLAGS) $(WARNFLAGS) -std=c99
CFLAGS      += --save-temps
LINKFLAGS   := --no-undefined

# Rule to make it all:
all: $(IMAGE) $(LIST)

# Rule to create the list file:
$(LIST) : $(ELF)
	$(ARM_OD) -D $(ELF) > $(LIST)

# Rule to create the target image file:
$(IMAGE) : $(ELF)
	$(ARM_OC) $(ELF) -O binary $(IMAGE)
	$(HEX_DUMP) $(IMAGE) > $(HEX)

# Rule to link the object files to an elf file:
$(ELF) : $(OBJ) $(LINKER)
	$(ARM_LD) $(LINKFLAGS) $(OBJ) -Map $(MAP) -o $(ELF) -T $(LINKER)

# Rule to compile the C-files (hardware dependent C-files):
$(BUILD_FILES)%.o : $(C_FILES_HW_DEP)%.c
	$(ARM_CC) $(CFLAGS) -c $< -o $@
	-rm -f $(TARGET)*.o
	-mv    $(TARGET)*.i $(EXTRA)
	-mv    $(TARGET)*.s $(EXTRA)

# Rule to compile the C-files (generic C-files):
$(BUILD_FILES)%.o : $(C_FILES_GEN)%.c
	$(ARM_CC) $(CFLAGS) -c $< -o $@
	-rm -f $(TARGET)*.o
	-mv    $(TARGET)*.i $(EXTRA)
	-mv    $(TARGET)*.s $(EXTRA)

# Rule to assembly the asm-files:
$(BUILD_FILES)%.o : $(ASM_FILES_HW_DEP)%.s
	$(ARM_CC) $(ASFLAGS) -c $< -o $@
	#$(ARM_AS) -c $< -o $@

# Rule to clean up a previous build:
clean : 
	-rm -f $(BUILD_FILES)*.o 
	-rm -f $(BUILD_FILES)*.d
	-rm -f $(EXTRA)*.i
	-rm -f $(EXTRA)*.s 
	-rm -f $(ELF)
	-rm -f $(IMAGE)
	-rm -f $(LIST)
	-rm -f $(MAP)
	-rm -f $(HEX)

# Rule to copy the kernel.img file
copy :
	cp kernel.img /Volumes/RaspberryPi/

# Rule to unmount the microSD card
umount :
	diskutil unmount /Volumes/RaspberryPi/
	
	
	

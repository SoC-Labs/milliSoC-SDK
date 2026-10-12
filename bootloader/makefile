#//-----------------------------------------------------------------------------
#// customised Cortex-M0 'nanosoc' controller
#// A joint work commissioned on behalf of SoC Labs, under Arm Academic Access license.
#//
#// Contributors
#//
#// David Flynn (d.w.flynn@soton.ac.uk)
#//
#// Copyright � 2021-3, SoC Labs (www.soclabs.org)
#//-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# The confidential and proprietary information contained in this file may
# only be used by a person authorised under and to the extent permitted
# by a subsisting licensing agreement from Arm Limited or its affiliates.
#
#            (C) COPYRIGHT 2010-2013 Arm Limited or its affiliates.
#                ALL RIGHTS RESERVED
#
# This entire notice must be reproduced on all copies of this file
# and copies of this file may only be made by a person if such person is
# permitted to do so under the terms of a subsisting license agreement
# from Arm Limited or its affiliates.
#
#      SVN Information
#
#      Checked In          : $Date: 2017-10-10 15:55:38 +0100 (Tue, 10 Oct 2017) $
#
#      Revision            : $Revision: 371321 $
#
#      Release Information : Cortex-M System Design Kit-r1p1-00rel0
#-----------------------------------------------------------------------------
#
# Cortex-M System Design Kit software compilation make file
#
#-----------------------------------------------------------------------------
CPU_PRODUCT = CORTEX_M85

TARGET = arm-none-eabi
# Shared software directory
SOFTWARE_DIR = $(CURDIR)/../
CMSIS_DIR    = $(SOFTWARE_DIR)/libs/CMSIS
CORE_DIR     = $(CMSIS_DIR)/include

DEVICE_DIR   = $(SOFTWARE_DIR)/libs/Device/M85/exectb_mcu


# Program file
BOOTLOADER     = bootloader
TEST_DIR 	 = $(SOFTWARE_DIR)/bootloader
BUILD_DIR    = $(SOFTWARE_DIR)/build/bootloader

# Endian Option
COMPILE_BIGEND = 0

# Configuration
USER_DEFINE    = -DCORTEX_M85

DEPS_LIST       = makefile

# Tool chain : ds5 / gcc / keil
TOOL_CHAIN      = ds6

ifeq ($(TOOL_CHAIN),ds5)
    CPU_TYPE        = --cpu=Pelican.fp
endif

ifeq ($(TOOL_CHAIN),ds6)
    CPU_TYPE        = -mcpu=cortex-m85
endif

ifeq ($(TOOL_CHAIN),gcc)
    CPU_TYPE        = -mcpu=cortex-m7
endif


# Startup code directory for DS-5
ifeq ($(TOOL_CHAIN),ds5)
 STARTUP_DIR  = $(DEVICE_DIR)/Source/ARM
endif

ifeq ($(TOOL_CHAIN),ds6)
 STARTUP_DIR  = $(DEVICE_DIR)/Source/ARM
endif

# Startup code directory for gcc
ifeq ($(TOOL_CHAIN),gcc)
 STARTUP_DIR  = $(DEVICE_DIR)/Source/GCC
endif

STARTUP_FILE = boot_exectb_mcu
SYSTEM_FILE  = system_exectb_mcu

# ---------------------------------------------------------------------------------------
# DS-5 options

# MicroLIB option
COMPILE_MICROLIB = 0


ifeq ($(TOOL_CHAIN),ds6)
  ARM_TARGET = --target=arm-$(TARGET)
  CC_TOOL   = armclang
  ASM_TOOL = armclang -masm=armasm $(ARM_TARGET) -c
else
  CC_TOOL   = armcc
  ASM_TOOL = armasm
  ARM_TARGET = -O3 -Otime --c99
endif

ARM_CC_OPTIONS   = $(ARM_TARGET) -c -D_FPU -mthumb -I $(DEVICE_DIR)/Include -I $(CORE_DIR) -I $(SOFTWARE_DIR)/libs/common/CMSDK \
		   -I $(SOFTWARE_DIR)/libs/Drivers/Include  \
		   -I $(SOFTWARE_DIR)/libs/common/retarget $(USER_DEFINE)

ARM_ASM_OPTIONS  = -g
ARM_LINK_OPTIONS += --scatter /home/dwn1c21/SoC-Labs/millisoc_project/millisoc_sdk/scatter/M85/armclang_bootrom.scat


ifeq ($(COMPILE_BIGEND),1)
 # Big Endian
 ARM_CC_OPTIONS   += --bigend
 ARM_ASM_OPTIONS  += --bigend
 ARM_LINK_OPTIONS += --be8
endif

ifeq ($(COMPILE_MICROLIB),1)
 # MicroLIB
 ARM_CC_OPTIONS   += --library_type=microlib
 ARM_ASM_OPTIONS  += --library_type=microlib --pd "__MICROLIB SETA 1"
 ARM_LINK_OPTIONS += --library_type=microlib
endif


# ---------------------------------------------------------------------------------------
# gcc options

GNG_CC      = $(TARGET)-gcc
GNU_OBJDUMP = $(TARGET)-objdump
GNU_OBJCOPY = $(TARGET)-objcopy

LINKER_SCRIPT_PATH = $(SOFTWARE_DIR)/common/scripts
LINKER_SCRIPT = $(LINKER_SCRIPT_PATH)/cmsdk_bootloader.ld

# GCC optimization specs
GCC_SPEC_OPTS := --specs=nano.specs -Wl,--gc-sections

GNU_CC_FLAGS = -g -O3 -Os -mthumb -flto -mthumb-interwork $(CPU_TYPE) $(GCC_SPEC_OPTS)

ifeq ($(COMPILE_BIGEND),1)
 # Big Endian
 GNU_CC_FLAGS   += -mbig-endian
endif
# ---------------------------------------------------------------------------------------
all: all_$(TOOL_CHAIN)

# ---------------------------------------------------------------------------------------
# DS-5
all_ds5 : $(BUILD_DIR)/$(BOOTLOADER).hex $(BUILD_DIR)/$(BOOTLOADER).lst
all_ds6 : $(BUILD_DIR)/$(BOOTLOADER).hex $(BUILD_DIR)/$(BOOTLOADER).lst


$(BUILD_DIR)/$(BOOTLOADER).o :  $(SOFTWARE_DIR)/bootloader/$(BOOTLOADER).c $(DEPS_LIST)
	mkdir -p $(BUILD_DIR)
	$(CC_TOOL) $(ARM_CC_OPTIONS) $(CPU_TYPE) $< -o  $@

$(BUILD_DIR)/$(SYSTEM_FILE).o : $(DEVICE_DIR)/Source/$(SYSTEM_FILE).c $(DEPS_LIST)
	$(CC_TOOL) $(ARM_CC_OPTIONS) $(CPU_TYPE) $< -o  $@

$(BUILD_DIR)/$(STARTUP_FILE).o : $(STARTUP_DIR)/$(STARTUP_FILE).c $(DEPS_LIST)
	$(CC_TOOL) $(ARM_CC_OPTIONS) $(CPU_TYPE) $< -o  $@

$(BUILD_DIR)/SL_QPSI.o: $(SOFTWARE_DIR)/libs/Drivers/Source/SL_QSPI.c $(DEPS_LIST)
	$(CC_TOOL) $(ARM_CC_OPTIONS) $(CPU_TYPE) $< -o  $@

$(BUILD_DIR)/ada_dma_command_lib.o: $(SOFTWARE_DIR)/libs/Drivers/Source/ada_dma_command_lib.c $(DEPS_LIST)
	$(CC_TOOL) $(ARM_CC_OPTIONS) $(CPU_TYPE) $< -o  $@

$(BUILD_DIR)/retarget.o : $(SOFTWARE_DIR)/libs/common/retarget/retarget.c $(DEPS_LIST)
	$(CC_TOOL) $(ARM_CC_OPTIONS) $(CPU_TYPE) $< -o  $@

$(BUILD_DIR)/uart_stdout.o : $(SOFTWARE_DIR)/libs/common/retarget/uart_stdout.c $(DEPS_LIST)
	$(CC_TOOL) $(ARM_CC_OPTIONS) $(CPU_TYPE) $< -o  $@

$(BUILD_DIR)/$(BOOTLOADER).ELF : $(BUILD_DIR)/$(BOOTLOADER).o $(BUILD_DIR)/$(SYSTEM_FILE).o $(BUILD_DIR)/$(STARTUP_FILE).o $(BUILD_DIR)/retarget.o $(BUILD_DIR)/uart_stdout.o $(BUILD_DIR)/ada_dma_command_lib.o $(BUILD_DIR)/SL_QPSI.o $(DEPS_LIST)
	armlink $(ARM_LINK_OPTIONS) $(BUILD_DIR)/$(BOOTLOADER).o $(BUILD_DIR)/$(SYSTEM_FILE).o  $(BUILD_DIR)/$(STARTUP_FILE).o $(BUILD_DIR)/retarget.o $(BUILD_DIR)/uart_stdout.o $(BUILD_DIR)/ada_dma_command_lib.o $(BUILD_DIR)/SL_QPSI.o -o $@

$(BUILD_DIR)/$(BOOTLOADER).hex : $(BUILD_DIR)/$(BOOTLOADER).ELF $(DEPS_LIST)
	fromelf --vhx --8x1 $< --output $@

$(BUILD_DIR)/$(BOOTLOADER).lst : $(BUILD_DIR)/$(BOOTLOADER).ELF $(DEPS_LIST)
	fromelf -c -d -e -s $< --output $@

# ---------------------------------------------------------------------------------------
# gcc

all_gcc:
	$(GNG_CC) $(GNU_CC_FLAGS) $(STARTUP_DIR)/$(STARTUP_FILE).s \
		$(SOFTWARE_DIR)/common/bootloader/$(BOOTLOADER).c \
		$(DEVICE_DIR)/Source/$(SYSTEM_FILE).c \
		-I $(DEVICE_DIR)/Include -I $(CORE_DIR) \
		-L $(LINKER_SCRIPT_PATH) \
		-D__STACK_SIZE=0x200 \
		-D__HEAP_SIZE=0x1000 \
		$(USER_DEFINE) -T $(LINKER_SCRIPT) -o $(BOOTLOADER).o
	# Generate disassembly code
	$(GNU_OBJDUMP) -S $(BOOTLOADER).o > $(BOOTLOADER).lst
	# Generate binary file
	$(GNU_OBJCOPY) -S $(BOOTLOADER).o -O binary $(BOOTLOADER).bin
	# Generate hex file
	$(GNU_OBJCOPY) -S $(BOOTLOADER).o --adjust-vma -0x10000000 --gap-fill 00 -O verilog $(BOOTLOADER).hex
	# Format the hex file to match fromelf output
	od -v -A n -t x1 --width=1  $(BOOTLOADER).bin > $(BOOTLOADER).hex

# Note:
# Objcopy use --adjust-vma so that the Verilog hex address start at address 0 instead of actual address 0x10000000
#
# If the version of object copy you are using does not support verilog hex file output,
# you can generate the hex file from binary file using the following command
#       od -v -A n -t x1 --width=1  $(TESTNAME).bin > $(TESTNAME).hex


# ---------------------------------------------------------------------------------------
# Keil MDK

all_keil:
	@echo "Please compile your project code and press ENTER when ready"
	@read dummy

# ---------------------------------------------------------------------------------------
# Binary

all_bin: $(BOOTLOADER).bin
	# Generate hex file from binary
	od -v -A n -t x1 --width=1  $(BOOTLOADER).bin > $(BOOTLOADER).hex

# ---------------------------------------------------------------------------------------
# Clean
clean :
	@rm -rf *.o
	@if [ -e $(BOOTLOADER).hex ] ; then \
	  rm -rf $(BOOTLOADER).hex ; \
	fi
	@if [ -e $(BOOTLOADER).lst ] ; then \
	  rm -rf $(BOOTLOADER).lst ; \
	fi
	@if [ -e $(BOOTLOADER).ELF ] ; then \
	  rm -rf $(BOOTLOADER).ELF ; \
	fi
	@if [ -e $(BOOTLOADER).bin ] ; then \
	  rm -rf $(BOOTLOADER).bin ; \
	fi
	@rm -rf *.crf
	@rm -rf *.plg
	@rm -rf *.tra
	@rm -rf *.htm
	@rm -rf *.map
	@rm -rf *.dep
	@rm -rf *.d
	@rm -rf *.lnp
	@rm -rf *.bak
	@rm -rf *.lst
	@rm -rf *.axf
	@rm -rf *.sct
	@rm -rf *.__i
	@rm -rf *._ia


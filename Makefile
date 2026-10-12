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
CPU_TYPE ?= CM7
TARGET = arm-none-eabi
# Shared software directory
SOFTWARE_DIR = $(CURDIR)
CMSIS_DIR    = $(SOFTWARE_DIR)/libs/CMSIS
CORE_DIR     = $(CMSIS_DIR)/include

ifeq ($(CPU_TYPE),CM7)
	DEVICE_DIR   = $(SOFTWARE_DIR)/libs/Device/M7/cm7ikmcu
else ifeq ($(CPU_TYPE), CM55)
	DEVICE_DIR   = $(SOFTWARE_DIR)/libs/Device/M55/cm7ikmcu
else ifeq ($(CPU_TYPE), CM85)
	DEVICE_DIR   = $(SOFTWARE_DIR)/libs/Device/M85/exectb_mcu
endif

# Program file
TESTNAME     ?= hello
TEST_DIR 	 = $(SOFTWARE_DIR)/apps/$(TESTNAME)
BUILD_DIR    = $(SOFTWARE_DIR)/build
# Endian Option
COMPILE_BIGEND = 0

# Configuration
ifeq ($(CPU_TYPE), CM7)
	USER_DEFINE    = -DCORTEX_M7
else ifeq ($(CPU_TYPE), CM55)
	USER_DEFINE    = -DCORTEX_M55
else ifeq ($(CPU_TYPE), CM85)
	USER_DEFINE    = -DCORTEX_M85
endif


DEPS_LIST       = 

# Tool chain : ds5 / gcc / keil
TOOL_CHAIN      = ds6

ifeq ($(TOOL_CHAIN),ds5)
	ifeq ($(CPU_TYPE),CM55)
    	CPU_OPTS        = --cpu=Cortex-M55.fp.sp
	else ifeq ($(CPU_TYPE),CM85)
    	CPU_OPTS        = --cpu=Cortex-M85.fp.sp
	endif
endif

ifeq ($(TOOL_CHAIN),ds6)
	ifeq ($(CPU_TYPE),CM55)
		CPU_OPTS        = -mcpu=cortex-m55
	else ifeq ($(CPU_TYPE),CM85)
		CPU_OPTS        = -mcpu=cortex-m85
	endif
endif

ifeq ($(TOOL_CHAIN),gcc)
    CPU_OPTS        = -mcpu=cortex-m7
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

ifeq ($(CPU_TYPE),CM7)
	STARTUP_FILE = boot_cm7ikmcu
	SYSTEM_FILE  = system_cm7ikmcu
else ifeq ($(CPU_TYPE),CM55)
	STARTUP_FILE = boot_cm55ikmcu
	SYSTEM_FILE  = system_cm55ikmcu
else ifeq ($(CPU_TYPE),CM85)
	STARTUP_FILE = boot_exectb_mcu
	SYSTEM_FILE  = system_exectb_mcu
endif

# ---------------------------------------------------------------------------------------
# DS-5 options

# MicroLIB option
COMPILE_MICROLIB = 0


#ARM_CC_OPTIONS   = -c -O3 -g -Otime -I $(DEVICE_DIR)/Include  -I $(CORE_DIR) \
#		   -I $(SOFTWARE_DIR)/common/retarget $(USER_DEFINE)
#ARM_ASM_OPTIONS  = -g
#ARM_LINK_OPTIONS = "--keep=$(STARTUP_FILE).o(RESET)" "--first=$(STARTUP_FILE).o(RESET)" \
#		   --rw_base 0x30000000 --ro_base 0x00000000 --map  --info sizes


ifeq ($(TOOL_CHAIN),ds6)
  ARM_TARGET = --target=arm-$(TARGET)
  CC_TOOL   = armclang -O1
  ASM_TOOL = armclang -masm=armasm $(ARM_TARGET) -c
else
  CC_TOOL   = armcc -O3
  ASM_TOOL = armasm
  ARM_TARGET = -O3 -Otime
endif

ARM_CC_OPTIONS   = $(ARM_TARGET) -c -D_FPU -mthumb -I $(DEVICE_DIR)/Include -I $(CORE_DIR) -I $(SOFTWARE_DIR)/libs/common/CMSDK \
		   -I $(SOFTWARE_DIR)/libs/Drivers/Include  \
		   -I $(SOFTWARE_DIR)/libs/common/retarget $(USER_DEFINE)
ARM_ASM_OPTIONS  = 
ARM_LINK_OPTIONS = "--keep=$(STARTUP_FILE).o(vectors)" "--first=$(STARTUP_FILE).o(vectors)" \
		--rw_base 0x20000000 --ro_base 0x000000 --map --datacompressor=off --info=inline

ARM_LINK_OPTIONS += --scatter /home/dwn1c21/SoC-Labs/millisoc_project/millisoc_sdk/scatter/M85/armclang.scat

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
LINKER_SCRIPT = $(LINKER_SCRIPT_PATH)/cmsdk_cm0.ld

# GCC optimization specs
GCC_SPEC_OPTS := --specs=nano.specs -Wl,--gc-sections

GNU_CC_FLAGS = -g -O3 -mthumb $(CPU_OPTS) $(GCC_SPEC_OPTS)

ifeq ($(COMPILE_BIGEND),1)
 # Big Endian
 GNU_CC_FLAGS   += -mbig-endian
endif

# ---------------------------------------------------------------------------------------
all: all_$(TOOL_CHAIN)

# ---------------------------------------------------------------------------------------
# DS-5
all_ds5 : $(BUILD_DIR)/$(TESTNAME)/$(TESTNAME).hex $(BUILD_DIR)/$(TESTNAME)/$(TESTNAME).lst
all_ds6 : $(BUILD_DIR)/$(TESTNAME)/$(TESTNAME).hex $(BUILD_DIR)/$(TESTNAME)/$(TESTNAME).lst

$(BUILD_DIR)/$(TESTNAME)/$(TESTNAME).o :  $(TEST_DIR)/$(TESTNAME).c $(DEPS_LIST)
	mkdir -p $(BUILD_DIR)/$(TESTNAME)
	$(CC_TOOL) $(ARM_CC_OPTIONS) $(CPU_OPTS) $< -o  $@

$(BUILD_DIR)/common/$(SYSTEM_FILE).o : $(DEVICE_DIR)/Source/$(SYSTEM_FILE).c $(DEPS_LIST)
	mkdir -p $(BUILD_DIR)/common
	$(CC_TOOL) $(ARM_CC_OPTIONS) $(CPU_OPTS) $< -o  $@

$(BUILD_DIR)/common/retarget.o : $(SOFTWARE_DIR)/libs/common/retarget/retarget.c $(DEPS_LIST)
	$(CC_TOOL) $(ARM_CC_OPTIONS) $(CPU_OPTS) $< -o  $@

$(BUILD_DIR)/common/uart_stdout.o : $(SOFTWARE_DIR)/libs/common/retarget/uart_stdout.c $(DEPS_LIST)
	$(CC_TOOL) $(ARM_CC_OPTIONS) $(CPU_OPTS) $< -o  $@

$(BUILD_DIR)/common/$(STARTUP_FILE).o : $(STARTUP_DIR)/$(STARTUP_FILE).c $(DEPS_LIST)
	$(CC_TOOL) $(ARM_CC_OPTIONS) $(CPU_OPTS) $< -o  $@

$(BUILD_DIR)/$(TESTNAME)/$(TESTNAME).ELF : $(BUILD_DIR)/$(TESTNAME)/$(TESTNAME).o $(BUILD_DIR)/common/$(SYSTEM_FILE).o $(BUILD_DIR)/common/$(STARTUP_FILE).o $(BUILD_DIR)/common/retarget.o $(BUILD_DIR)/common/uart_stdout.o
	armlink $(ARM_LINK_OPTIONS) $(BUILD_DIR)/$(TESTNAME)/$(TESTNAME).o $(BUILD_DIR)/common/$(SYSTEM_FILE).o  $(BUILD_DIR)/common/$(STARTUP_FILE).o $(BUILD_DIR)/common/retarget.o $(BUILD_DIR)/common/uart_stdout.o -o $@

$(BUILD_DIR)/$(TESTNAME)/$(TESTNAME).hex : $(BUILD_DIR)/$(TESTNAME)/$(TESTNAME).ELF
	fromelf --bin $< --output $(BUILD_DIR)/$(TESTNAME)/$(TESTNAME).bin
	fromelf --vhx --8x1 $< --output $@

$(BUILD_DIR)/$(TESTNAME)/$(TESTNAME).lst : $(BUILD_DIR)/$(TESTNAME)/$(TESTNAME).ELF
	fromelf -c -d -e -s -z -v $< --output $@

# ---------------------------------------------------------------------------------------
# gcc
all_gcc:
	$(GNG_CC) $(GNU_CC_FLAGS) $(STARTUP_DIR)/$(STARTUP_FILE).s \
		$(TESTNAME).c \
		$(SOFTWARE_DIR)/common/retarget/retarget.c \
		$(SOFTWARE_DIR)/common/retarget/uart_stdout.c \
		$(DEVICE_DIR)/Source/$(SYSTEM_FILE).c \
		-I $(DEVICE_DIR)/Include -I $(CORE_DIR) \
                -I $(SOFTWARE_DIR)/common/retarget  \
		-L $(LINKER_SCRIPT_PATH) \
		-D__STACK_SIZE=0x200 \
		-D__HEAP_SIZE=0x1000 \
		$(USER_DEFINE) -T $(LINKER_SCRIPT) -o $(TESTNAME).o
	# Generate disassembly code
	$(GNU_OBJDUMP) -S $(TESTNAME).o > $(TESTNAME).lst
	# Generate binary file
	$(GNU_OBJCOPY) -S $(TESTNAME).o -O binary $(TESTNAME).bin
	# Generate hex file
	$(GNU_OBJCOPY) -S $(TESTNAME).o -O verilog $(TESTNAME).hex

# Note:
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

all_bin: $(TESTNAME).bin
	# Generate hex file from binary
	od -v -A n -t x1 --width=1  $(TESTNAME).bin > $(TESTNAME).hex

# ---------------------------------------------------------------------------------------
# Clean
clean :
	@rm -rf $(BUILD_DIR)


# Bootrom Parameters:
# Boot Loader image
BOOTLOADER        ?= bootloader
BOOTROM_ADDRW     ?= 12
BOOTROM_HEX       ?= $(BUILD_DIR)/bootloader/$(BOOTLOADER).hex
BOOTROM_BUILD_DIR ?= $(SOCLABS_PROJECT_DIR)/system/src/bootrom

# code target is used by other simulation targets (e.g. run_mti, sim_mti)
# Before simulation, bootrom and testcode need to be compiled.
code : testcode bootrom

# Boot ROM generation
#--------------------------------------------------------------------------------
# Output files and directories
BOOTROM_SV_DIR := $(BOOTROM_BUILD_DIR)/verilog
BOOTROM_BIN_DIR:= $(BOOTROM_BUILD_DIR)/bintxt

# Output files
BOOTROM_SV     := $(BOOTROM_SV_DIR)/bootrom.sv
BOOTROM_BIN    := $(BOOTROM_BIN_DIR)/bootrom.bintxt

# Intermediate files
BOOTLOADER_HEX := $(BUILD_DIR)/bootloader/$(BOOTLOADER).hex
BOOTROM_GEN_PY := $(SOFTWARE_DIR)/bootloader/bootrom_gen.py

bootrom: $(BOOTROM_SV) $(BOOTROM_BIN) $(BOOTROM_SIM_HEX)

# Generate the Boot ROM Verilog and Binary Text files from the bootloader hex file
$(BOOTROM_SV) $(BOOTROM_BIN): $(BOOTROM_HEX) $(BOOTROM_GEN_PY) | $(BOOTROM_SV_DIR) $(BOOTROM_BIN_DIR)
	@echo "Generating Boot ROM Verilog and Binary Text files"
	@echo "Toolchain: $(TOOL_CHAIN)"
	@python3 $(BOOTROM_GEN_PY) -a $(BOOTROM_ADDRW) -i $(BOOTROM_HEX) -t $(TOOL_CHAIN) -v $(BOOTROM_SV) -b $(BOOTROM_BIN)

# Create output directories if they do not exist
$(BOOTROM_SV_DIR) $(BOOTROM_BIN_DIR):
	@mkdir -p $@

# Compile bootloader in the testcodes/bootloader directory
$(BOOTROM_HEX):
	$(MAKE) -C $(SOFTWARE_DIR)/$(BOOTLOADER) all_$(TOOL_CHAIN) $(SW_MAKE_OPTIONS)

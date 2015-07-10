# for home
CROSS_COMPILE	= /home/korena/Development/Tools/gcc-arm-none-eabi-4_8-2014q3/bin/arm-none-eabi-
# for work
#CROSS_COMPILE	=arm-none-eabi-


AS        = $(CROSS_COMPILE)as
LD        = $(CROSS_COMPILE)ld
CC        = $(CROSS_COMPILE)gcc
AR        = $(CROSS_COMPILE)ar
NM        = $(CROSS_COMPILE)nm
STRIP     = $(CROSS_COMPILE)strip
OBJCOPY   = $(CROSS_COMPILE)objcopy
OBJDUMP   = $(CROSS_COMPILE)objdump
HEX  = $(OBJCOPY) -O ihex
BIN  = $(OBJCOPY) -O binary -S

PRJ_ROOT_DIR := $(shell pwd)
PROJECT_HOME := $(shell pwd)

ASM_SRC_DIR = $(PROJECT_HOME)/asm
SRC_DIR = $(PROJECT_HOME)/src

HOST_TOOLS_DIR= $(PROJECT_HOME)/host_tools
HOST_SCRIPTS_DIR=$(PROJECT_HOME)/host_scripts

# add include directories here ...
INCLUDES_DIR = $(PROJECT_HOME)/include

##### bootloader related (second stage onwards) #####
BL2_ROOT_DIR = $(PROJECT_HOME)/bl2
BL2_ASM_DIR = $(BL2_ROOT_DIR)/asm
BL2_INC_DIR = $(BL2_ROOT_DIR)/include
BL2_C_SRC_DIR = $(BL2_ROOT_DIR)/src


TARGET_BIN_DIR = $(PROJECT_HOME)/target_bin
HOST_BIN_DIR = $(PROJECT_HOME)/host_bin

# Target Name
TARGET_NAME := Tiny210_SDK
PROJECT_TYPE := embedded_app

# Version Number
MAJOR := 1
MINOR := 00


IMGMAKE  =$(HOST_BIN_DIR)/imageMaker

BL1_BIN  = target_bin/BL1.bin
BL1_ELF  = target_bin/BL1.elf
BL1_LDS   = linker.lds


#BL1 source files expression ...
BL1_SRCs_ASM += $(wildcard $(ASM_SRC_DIR)/*.s)
BL1_SRCs_C += $(wildcard $(SRC_DIR)/*.c)
OBJS_BL1 = $(BL1_SRCs_ASM:.s=.o) $(BL1_SRCs_C:.c=.o) 
# BL1 include files expression ...
ASM_INCLUDES += $(wildcard $(INCLUDES_DIR)/*.inc)
C_INCLUDES += $(wildcard $(INCLUDES_DIR)/*.h)




BL2_BIN = $(TARGET_BIN_DIR)/BL2.bin
BL2_ELF = $(TARGET_BIN_DIR)/BL2.elf
BL2_LDS   = $(BL2_ROOT_DIR)/BL2_linker.lds

#BL2 source files expressions ...
BL2_SRCs_ASM += $(wildcard $(BL2_ASM_DIR)/*.s)
BL2_SRCs_C += $(wildcard $(BL2_C_SRC_DIR)/*.c)
OBJS_BL2 = $(BL2_SRCs_ASM:.s=.o) $(BL2_SRCs_C:.c=.o)

CFLAGS =  -Os -nostdlib
CFLAGS_HOST = -Os -Wall

all:  $(IMGMAKE) $(BL1_BIN).boot  $(BL1_BIN) $(BL2_BIN) 

$(BL1_BIN):      $(OBJS_BL1)
	$(LD) -T $(BL1_LDS) -o $(BL1_ELF) -Map BL1.map $(OBJS_BL1)
	$(OBJCOPY) -O binary $(BL1_ELF) $(BL1_BIN)

$(BL1_BIN).boot:	$(BL1_BIN) $(IMGMAKE)
	$(IMGMAKE) ./$(BL1_BIN) ./$(BL1_BIN).boot

$(BL2_BIN):   $(OBJS_BL2)
	$(LD) -T $(BL2_LDS) -o $(BL2_ELF) -Map $(BL2_ROOT_DIR)/BL2.map $(OBJS_BL2)
	$(OBJCOPY) -O binary $(BL2_ELF) $(BL2_BIN)

$(IMGMAKE):	
	gcc $(CFLAGS_HOST) -o $(IMGMAKE) $(HOST_TOOLS_DIR)/imageMaker.c

fuse:	$(BL1_BIN).boot $(BL2_BIN)
	. $(HOST_SCRIPTS_DIR)/fuseBin.sh
	
%.o:	%.c
	$(CC) -c $(CFLAGS) -I . $(INCLUDES_DIR)  $< -o $@ 

%.o:	%.s
	$(AS) -c $(AFLAGS) -I  $(ASM_INCLUDES)  $< -o $@

%elf: $(OBJS_BL1)
	$(CC) $(OBJS_BL1) $(LDFLAGS) $(LIBS) -o $@

%hex: %elf
	$(HEX) $< $@
        
%bin: %elf
	$(BIN)  $< $@

readFileBytes:
	gcc $(CFLAGS_HOST) -o $(HOST_BIN_DIR)/readBytes $(HOST_TOOLS_DIR)/readFileBytes.c

dep:
	gccmakedep $(OBJS_BL1:.o=.c) $(OBJS_BL1:.o=.s) $(OBJS_BL2:.o=.c) $(OBJS_BL2:.o=.s)

clean:
	rm -rf $(OBJS) $(HOST_BIN_DIR)/*  $(TARGET_BIN_DIR)/*.bin $(TARGET_BIN_DIR)/*.elf $(TARGET_BIN_DIR)/*.boot $(TARGET_BIN_DIR)/*.o *.map *.o $(ASM_SRC_DIR)/*.o SRC_DIR/*.o $(BL2_ASM_DIR)/*.o $(BL2_ROOT_DIR)/*.map 


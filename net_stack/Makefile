# for home
#CROSS_COMPILE	= /home/korena/Compilers/gcc-arm-none-eabi-5_2-2015q4/bin/arm-none-eabi-
LIBS_DIR = -L /home/korena/Compilers/gcc-arm-none-eabi-5_2-2015q4/lib/gcc/arm-none-eabi/5.2.1/
LIBS_DIR += -L /home/korena/Compilers/gcc-arm-none-eabi-5_2-2015q4/lib/
LIBS_DIR += -L /home/korena/Compilers/gcc-arm-none-eabi-5_2-2015q4/arm-none-eabi/lib/
# for work
CROSS_COMPILE	=arm-none-eabi-


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


##### host tools #################
HOST_TOOLS_DIR= $(PROJECT_HOME)/host_tools
HOST_SCRIPTS_DIR=$(PROJECT_HOME)/host_scripts

##### BL1 related (first stage) #####
BL1_ASM_SRC_DIR = $(PROJECT_HOME)/asm
BL1_C_SRC_DIR = $(PROJECT_HOME)/src
BL1_INCLUDES_DIR = $(PROJECT_HOME)/include


##### bootloader related (second stage onwards) #####
BL2_ROOT_DIR = $(PROJECT_HOME)/bl2
BL2_ASM_SRC_DIR = $(BL2_ROOT_DIR)/asm
BL2_INCLUDES_DIR = $(BL2_ROOT_DIR)/include
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
IMGMAKEBL2 = $(HOST_BIN_DIR)/bl2ImageMaker
BL1_BIN  = target_bin/BL1.bin
BL1_ELF  = target_bin/BL1.elf
BL1_LDS   = linker.lds


#BL1 source files expression ...
BL1_SRCs_ASM += $(wildcard $(BL1_ASM_SRC_DIR)/*.s)
BL1_SRCs_C += $(wildcard $(BL1_C_SRC_DIR)/*.c)
OBJS_BL1 = $(BL1_SRCs_ASM:.s=.o) $(BL1_SRCs_C:.c=.o) 
# BL1 include files expression ...
BL1_ASM_INCLUDES += $(wildcard $(BL1_INCLUDES_DIR)/*.inc)
BL1_C_INCLUDES += $(wildcard $(BL1_INCLUDES_DIR)/*.h)


BL2_BIN = $(TARGET_BIN_DIR)/BL2.bin
BL2_ELF = $(TARGET_BIN_DIR)/BL2.elf
BL2_LDS = $(BL2_ROOT_DIR)/BL2_linker.lds

#BL2 source files expressions ...
BL2_SRCs_ASM += $(wildcard $(BL2_ASM_SRC_DIR)/*.s)
BL2_SRCs_C += $(wildcard $(BL2_C_SRC_DIR)/*.c)
BL2_OBJS_ASM = $(BL2_SRCs_ASM:.s=.o) 
BL2_OBJS_C = $(BL2_SRCs_C:.c=.o)

CFLAGS = -O2  -Wall -march=armv7-a
LDFLAGS = -nostdlib  
LIBS = -lgcc -lc 
AFLAGS = -march=armv7-a
CFLAGS_HOST = -Os -Wall

all:  $(IMGMAKE) $(IMGMAKEBL2)  $(BL1_BIN).boot  $(BL1_BIN) $(BL2_BIN).img $(BL2_BIN) 

$(BL1_BIN):      $(OBJS_BL1)
	$(LD) -T $(BL1_LDS) -o $(BL1_ELF) -Map BL1.map $(OBJS_BL1)
	$(OBJCOPY) -O binary $(BL1_ELF) $(BL1_BIN)

$(BL1_BIN).boot:	$(BL1_BIN) $(IMGMAKE)
	$(IMGMAKE) ./$(BL1_BIN) ./$(BL1_BIN).boot

$(BL2_BIN):   $(BL2_OBJS_ASM) $(BL2_OBJS_C)
	$(LD) -T $(BL2_LDS) -o $(BL2_ELF) -Map $(BL2_ROOT_DIR)/BL2.map $(BL2_OBJS_ASM) $(BL2_OBJS_C) $(LDFLAGS) $(LIBS_DIR) $(LIBS)
	$(OBJCOPY) -O binary $(BL2_ELF) $(BL2_BIN)

$(BL2_BIN).img:	$(BL2_BIN) $(IMGMAKEBL2)
	$(IMGMAKEBL2) $(BL2_BIN) $(BL2_BIN).img

$(IMGMAKE):	
	gcc $(CFLAGS_HOST) -o $(IMGMAKE) $(HOST_TOOLS_DIR)/imageMaker.c

$(IMGMAKEBL2):	
	gcc $(CFLAGS_HOST) -o $(IMGMAKEBL2) $(HOST_TOOLS_DIR)/bl2ImageMaker.c

fuse:	$(BL1_BIN).boot $(BL2_BIN)
	. $(HOST_SCRIPTS_DIR)/fuseBin.sh
	
$(BL1_C_SRC_DIR)/%.o:	$(BL1_C_SRC_DIR)/%.c
	$(CC) -c $(CFLAGS) -I $(BL1_INCLUDES_DIR)  $< -o $@ 

$(BL1_ASM_SRC_DIR)/%.o:	$(BL1_ASM_SRC_DIR)/%.s
	$(AS) -c $(AFLAGS) -I $(BL1_INCLUDES_DIR)  $< -o $@

$(BL2_ASM_SRC_DIR)/%.o:	$(BL2_ASM_SRC_DIR)/%.s
	$(AS) -c $(AFLAGS) -I $(BL2_INCLUDES_DIR)  $< -o $@

$(BL2_C_SRC_DIR)/%.o:	$(BL2_C_SRC_DIR)/%.c 
	$(CC) -c $(CFLAGS) -I $(BL2_INCLUDES_DIR)  $< -o $@ 

$(BL1_ELF): $(OBJS_BL1)
	$(CC) $(OBJS_BL1) $(CFLAGS) -o $@


$(BL2_ELF): $(BL2_OBJS_ASM) $(BL2_OBJS_C)
	$(CC) $(BL2_OBJS_ASM) $(BL2_OBJS_C) $(CFLAGS) -o $@


readFileBytes:
	gcc $(CFLAGS_HOST) -o $(HOST_BIN_DIR)/readBytes $(HOST_TOOLS_DIR)/readFileBytes.c

#dep:
#	gccmakedep $(OBJS_BL1:.o=.c) $(OBJS_BL1:.o=.s) $(OBJS_BL2:.o=.c) $(OBJS_BL2:.o=.s)

clean:
	rm -rf $(OBJS) $(HOST_BIN_DIR)/*  $(TARGET_BIN_DIR)/*.bin $(TARGET_BIN_DIR)/*.elf $(TARGET_BIN_DIR)/*.boot $(TARGET_BIN_DIR)/*.o *.map *.o $(BL1_ASM_SRC_DIR)/*.o $(BL1_C_SRC_DIR)/*.o $(BL2_ASM_SRC_DIR)/*.o $(BL2_C_SRC_DIR)/*.o $(BL2_ROOT_DIR)/*.map $(TARGET_BIN_DIR)/*.img


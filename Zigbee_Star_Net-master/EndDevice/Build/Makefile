#########################################################################
#
# MODULE:   802.15.4 AN1174_154_Coord
#
# DESCRIPTION: MakeFile
#
############################################################################
# 
# This software is owned by Jennic and/or its supplier and is protected
# under applicable copyright laws. All rights are reserved. We grant You,
# and any third parties, a license to use this software solely and
# exclusively on Jennic products. You, and any third parties must reproduce
# the copyright and warranty notice and any other legend of ownership on each
# copy or partial copy of the software.
# 
# THIS SOFTWARE IS PROVIDED "AS IS". JENNIC MAKES NO WARRANTIES, WHETHER
# EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE,
# ACCURACY OR LACK OF NEGLIGENCE. JENNIC SHALL NOT, IN ANY CIRCUMSTANCES,
# BE LIABLE FOR ANY DAMAGES, INCLUDING, BUT NOT LIMITED TO, SPECIAL,
# INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON WHATSOEVER.
# 
# Copyright Jennic Ltd 2009. All rights reserved
#
#############################################################################
# Subversion variables
# $HeadURL: https://www.collabnet.nxp.com/svn/lprf_apps/Application_Notes/JN-AN-1069-IEEE-802.15.4-Serial-Cable-Replacement/Trunk/EndDevice/Build/Makefile $
# $Revision: 18327 $
# $LastChangedBy: nxp29759 $
# $LastChangedDate: 2017-02-10 09:37:11 +0000 (Fri, 10 Feb 2017) $
# $Id: Makefile 18327 2017-02-10 09:37:11Z nxp29759 $ 
#
#############################################################################

# Application target name

TARGET = EndDevice

#  Note: Target name must be the same as the subfolder name
##############################################################################
#User definable make parameters that may be overwritten from the command line

# Default target device is the JN5148
JENNIC_CHIP ?= JN5168
JENNIC_CHIP_FAMILY ?=JN516x
JENNIC_PCB ?= DEVKIT4 


ifeq ($(JENNIC_CHIP_FAMILY), JN517x)
JENNIC_SDK              = JN-SW-4263
JENNIC_PCB              = DEVKIT5
ENDIAN                  = LITTLE_ENDIAN_PROCESSOR
DISABLE_LTO = 1
else 
ifeq ($(JENNIC_CHIP_FAMILY), JN516x)
JENNIC_SDK              = JN-SW-4163
JENNIC_PCB              = DEVKIT4
ENDIAN 	                = BIG_ENDIAN
endif
endif

##############################################################################
# Default DK2 development kit target hardware

JENNIC_PCB ?= DEVKIT5

##############################################################################
# Select the network stack (e.g. MAC, ZBPRO)

JENNIC_STACK ?= MAC

##############################################################################
# Debug options define DEBUG for HW debug
#DEBUG ?=HW

#
#
# Define which UART to use for debug
DEBUG_PORT ?= UART0

##############################################################################
# Define TRACE to use with DBG module
#TRACE ?=1

##############################################################################
# Path definitions
# Select definitions for either single or multiple targets

# Use if application directory contains multiple targets
##SDK_BASE_DIR       ?= $(abspath ../../../../sdk/$(JENNIC_SDK)/)
SDK_BASE_DIR       ?= /c/NXP/bstudio_nxp/sdk/$(JENNIC_SDK)
APP_BASE            = $(abspath ../..)
APP_BLD_DIR         = $(APP_BASE)/$(TARGET)/Build
APP_SRC_DIR         = $(APP_BASE)/$(TARGET)/Source
APP_COMMON_SRC_DIR  = $(APP_BASE)/Common/Source
APP_COMMON_SRC_DEV_DRV_DIR   = $(APP_COMMON_SRC_DIR)/dev_drv
APP_COMMON_SRC_HAL  = $(APP_COMMON_SRC_DIR)/hal
APP_COMMON_SRC_HW_DRV  = $(APP_COMMON_SRC_DIR)/hw_drv

# Use if application directory contains single target
#SDK_BASE_DIR         	= $(abspath ../../..)
#APP_BASE            	= $(abspath ..)
#APP_BLD_DIR          	= $(APP_BASE)/Build
#APP_SRC_DIR          	= $(APP_BASE)/Source

# Compiler makefile defines 
CFLAGS += -DLED_CTRL_CARRIER_BOARD=

##############################################################################
# Application Source files

# Note: Path to source file is found using vpath below, so only .c filename is required
APPSRC += ed_enddevice.c
APPSRC += ed_app.c
APPSRC += dbg.c
APPSRC += node.c
#APPSRC += rnd.c
APPSRC += uart.c
APPSRC += queue.c
APPSRC += AppQueueApi.c
APPSRC += EEPROM.c
APPSRC += common_init.c

APPSRC += hw_gpio.c
APPSRC += hw_timer.c

APPSRC += drv_led.c
APPSRC += drv_adc.c
APPSRC += drv_flash.c
APPSRC += drv_key.c
APPSRC += drv_rgb.c

APPSRC += app_adc.c
APPSRC += app_led.c
APPSRC += app_flash.c
#APPSRC += app_key.c
APPSRC += app_childTable.c

APPSRC += delay.c
APPSRC += usr_timer.c

APPSRC += epd_block.c
#APPSRC += drv_lcd.c
#APPSRC += appHwIndHandle.c
APPSRC += epd_drv.c
APPSRC += epd_spi_drv.c
APPSRC += epd_draw_drv.c
APPSRC += epd_api.c

APPSRC += ed_esl.c
APPSRC += sleep.c
APPSRC += msg.c
APPSRC += ed_node_Simulate.c
APPSRC += sjson.c
APPSRC += sjsonApp.c
APPSRC += intToStr.c
APPSRC += sjson_app.c

##############################################################################
# Additional Application Source directories
# Define any additional application directories outside the application directory
# e.g. for AppQueueApi

ADDITIONAL_SRC_DIR += $(COMPONENTS_BASE_DIR)/AppQueueApi/Source

##############################################################################
# Standard Application header search paths

INCFLAGS += -I$(APP_SRC_DIR)
INCFLAGS += -I$(APP_SRC_DIR)/..
INCFLAGS += -I$(APP_COMMON_SRC_DIR)

INCFLAGS += -I$(APP_COMMON_SRC_HAL)
INCFLAGS += -I$(APP_COMMON_SRC_DEV_DRV_DIR)
INCFLAGS += -I$(APP_COMMON_SRC_HW_DRV)

# Application specific include files
INCFLAGS += -I$(COMPONENTS_BASE_DIR)/JennicLogo/Include
INCFLAGS += -I$(COMPONENTS_BASE_DIR)/Utilities/Include
INCFLAGS += -I$(COMPONENTS_BASE_DIR)/AppQueueApi/Include 

##############################################################################
# Application libraries
# Specify additional Component libraries

#APPLIBS += 

##############################################################################

# You should not need to edit below this line

##############################################################################
##############################################################################
# Configure for the selected chip or chip family

include $(SDK_BASE_DIR)/Chip/Common/Build/config.mk
include $(SDK_BASE_DIR)/Platform/Common/Build/Config.mk
include $(SDK_BASE_DIR)/Stack/Common/Build/Config.mk

##############################################################################

APPOBJS = $(APPSRC:.c=.o)

##############################################################################
# Application dynamic dependencies

APPDEPS = $(APPOBJS:.o=.d)

#########################################################################
# Linker

# Add application libraries before chip specific libraries to linker so
# symbols are resolved correctly (i.e. ordering is significant for GCC)

LDLIBS := $(addsuffix _$(JENNIC_CHIP_FAMILY),$(APPLIBS)) $(LDLIBS)

#########################################################################
# Dependency rules

.PHONY: all clean
# Path to directories containing application source 
vpath % $(APP_SRC_DIR):$(APP_COMMON_SRC_DIR):$(ADDITIONAL_SRC_DIR):$(APP_COMMON_SRC_DEV_DRV_DIR):$(APP_COMMON_SRC_HAL):$(APP_COMMON_SRC_HW_DRV)


all: $(TARGET)_$(JENNIC_CHIP)$(BIN_SUFFIX).bin

-include $(APPDEPS)
%.d:
	rm -f $*.o


%.o: %.S
	$(info Assembling $< ...)
	$(CC) -c -o $(subst Source,Build,$@) $(CFLAGS) $(INCFLAGS) $< -MD -MF $*.d -MP
	@echo

%.o: %.c 
	$(info Compiling $< ...)
	$(CC) -c -o $(subst Source,Build,$@) $(CFLAGS) $(INCFLAGS) $< -MD -MF $*.d -MP
	@echo

$(TARGET)_$(JENNIC_CHIP)$(BIN_SUFFIX).elf: $(APPOBJS) $(addsuffix _$(JENNIC_CHIP_FAMILY).a,$(addprefix $(COMPONENTS_BASE_DIR)/Library/lib,$(APPLIBS))) 
	$(info Linking $@ ...)
	$(CC) -Wl,--gc-sections -Wl,-u_AppColdStart -Wl,-u_AppWarmStart $(LDFLAGS) -T$(LINKCMD) -o $@ $(APPOBJS) -Wl,--start-group  $(addprefix -l,$(LDLIBS)) -Wl,--end-group -Wl,-Map,$(TARGET)_$(JENNIC_CHIP)$(BIN_SUFFIX).map 
	${SIZE} $@
	@echo

$(TARGET)_$(JENNIC_CHIP)$(BIN_SUFFIX).bin: $(TARGET)_$(JENNIC_CHIP)$(BIN_SUFFIX).elf 
	$(info Generating binary ...)
	$(OBJCOPY) -S -O binary $< $@
	
#########################################################################

clean:
	rm -f $(APPOBJS) $(APPDEPS) $(TARGET)_$(JENNIC_CHIP)*.bin $(TARGET)_$(JENNIC_CHIP)*.elf $(TARGET)_$(JENNIC_CHIP)*.map

#########################################################################

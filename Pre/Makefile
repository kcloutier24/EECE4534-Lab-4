# Makefile for out of tree compilation of kernel modules
# For details, see:
# https://www.kernel.org/doc/Documentation/kbuild/modules.txt

obj-m := gmirror.o
ccflags-y += -g -DDEBUG
ARCH ?= arm
ESL_KERNEL_VERSION ?= 5.10.0-esl
ifeq ($(OECORE_TARGET_SYSROOT),)
$(error "Can't determine SDK path. Verify that you have sourced the environment correctly.")
endif
KERNEL_SRC ?= $(OECORE_TARGET_SYSROOT)/lib/modules/$(ESL_KERNEL_VERSION)/build
CROSS_COMPILE ?= arm-linux-esl-gnueabi-
ESL_ZEDBOARD_IP?=qemu
SRC := $(shell pwd)
UPLOAD_PATH ?= /home/root/

MODULE_OBJ:=$(obj-m:.o=.ko)
MODULE_ARGS ?=

all: modules

# use the makefile in kernel sources for compilation magic 
modules clean modules_install help:
	@$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNEL_SRC) M=$(SRC) $@

modules_upload: $(MODULE_OBJ) 
	scp $(MODULE_OBJ) root@$(ESL_ZEDBOARD_IP):$(UPLOAD_PATH)

insmod: modules_upload
	@ssh root@$(ESL_ZEDBOARD_IP) "rmmod $(MODULE_OBJ) >/dev/null; insmod $(UPLOAD_PATH)$(MODULE_OBJ) $(MODULE_ARGS)"

.PHONY: modules_upload clean

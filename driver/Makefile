#
# NTV2 v4l2 driver makefile
#
# Copyright (C) 2016 AJA Video Systems, Inc.  
#

USER			= $(shell whoami)
PWD				= $(shell pwd)
CPU_ARCH		?= $(shell uname -m)
LINUX_DISTRO	?= GENERIC
DRIVERDIR		?= $(PWD)
KDIR			?= /lib/modules/$(shell uname -r)/build
BINDIR			= .
CC				= gcc
DRIVERNAME		= ntv2video

DRIVERTARGET	= $(DRIVERNAME).o
INCLUDES		= -I. -I$(DRIVERDIR)
ARCH_CFLAGS		?= -D$(LINUX_DISTRO) -D$(CPU_ARCH)
EXTRA_CFLAGS	+= $(ARCH_CFLAGS) $(INCLUDES) -Wall -Wno-unused-function
LDFLAGS			:= 
export LDFLAGS

DRIVERSRCS 		=	ntv2_driver.c \
					ntv2_params.c \
					ntv2_device.c \
					ntv2_video.c \
					ntv2_vb2ops.c \
					ntv2_v4l2ops.c \
					ntv2_audio.c \
					ntv2_pcmops.c \
					ntv2_channel.c \
					ntv2_register.c \
					ntv2_pci.c \
					ntv2_nwldma.c \
					ntv2_xlxdma.c \
					ntv2_konareg.c \
					ntv2_timecode.c \
					ntv2_features.c \
					ntv2_videoops.c \
					ntv2_audioops.c \
					ntv2_input.c \
					ntv2_konai2c.c \
					ntv2_hdmiin.c \
					ntv2_hdmiin4.c \
					ntv2_hdmiedid.c \
					ntv2_serial.c \
					ntv2_mixops.c \
					ntv2_chrdev.c

DRIVEROBJS		=	$(patsubst %.c,%.o,$(DRIVERSRCS))

DRIVERINCS		=	ntv2_common.h \
					ntv2_params.h \
					ntv2_device.h \
					ntv2_video.h \
					ntv2_vb2ops.h \
					ntv2_v4l2ops.h \
					ntv2_audio.h \
					ntv2_pcmops.h \
					ntv2_channel.h \
					ntv2_register.h \
					ntv2_pci.h \
					ntv2_nwldma.h \
					ntv2_nwlreg.h \
					ntv2_xlxdma.h \
					ntv2_xlxreg.h \
					ntv2_konareg.h \
					ntv2_timecode.h \
					ntv2_features.h \
					ntv2_videoops.h \
					ntv2_audioops.h \
					ntv2_input.h \
					ntv2_konai2c.h \
					ntv2_hdmiin.h \
					ntv2_hdmiin4.h \
					ntv2_hdmiedid.h \
					ntv2_hinreg.h \
					ntv2_serial.h \
					ntv2_mixops.h \
					ntv2_chrdev.h

obj-m			:= $(DRIVERTARGET)
$(DRIVERNAME)-y	:= $(DRIVEROBJS)

.PHONY: default
default: all 
	$(MAKE) -C $(KDIR) M=$(DRIVERDIR) DRIVERDIR=$(DRIVERDIR) modules

.PHONY: all
all: $(DRIVERSRCS) $(DRIVERINCS)

.PHONY: load
load: mod var nod

# Load kernel modules
.PHONY: mod
mod:
	@if [ $$USER != 'root' ]; then echo "Load requires elevated privileges (try again with sudo?)" && exit 127;fi
	/sbin/modprobe v4l2-common
	/sbin/modprobe videobuf2-core
	/sbin/modprobe videobuf2-vmalloc
	find /lib/modules/$(shell uname -r) -name videobuf2-v4l2.ko -exec /sbin/modprobe videobuf2-v4l2 \;
	/sbin/rmmod ntv2video.ko && /sbin/insmod ntv2video.ko || /sbin/insmod ntv2video.ko

# find ntv2dev major number
.PHONY: var
var:
	$(eval MAJOR = $(shell cat /proc/devices | awk '$$2=="ntv2dev" {print $$1}'))

# remove and insert nodes
.PHONY: nod
nod:
	rm -f /dev/ajantv2[0-3]
	mknod /dev/ajantv20 c $(MAJOR) 0
	mknod /dev/ajantv21 c $(MAJOR) 1
	mknod /dev/ajantv22 c $(MAJOR) 2
	mknod /dev/ajantv23 c $(MAJOR) 3
	chmod 666 /dev/ajantv2[0-3]
	rm -f /dev/oem2k[0-3]
	mknod /dev/oem2k0 c $(MAJOR) 0
	mknod /dev/oem2k1 c $(MAJOR) 1
	mknod /dev/oem2k2 c $(MAJOR) 2
	mknod /dev/oem2k3 c $(MAJOR) 3
	chmod 666 /dev/oem2k[0-3]
	@echo "Load complete!"

.PHONY: unload
unload:
	@if [ $$USER != 'root' ]; then echo "Unload requires elevated privileges (try again with sudo?)" && exit 127;fi
	/sbin/rmmod ntv2video.ko
	rm -f /dev/ajantv2[0-3]
	rm -f /dev/oem2k[0-3]
	@echo "Unload complete!"

.PHONY: clean
clean:
	rm -f $(BINDIR)/*.ko *.ko *.o .*o.cmd *.o.ur-safe *.mod.c *~ errors.txt semantic.cache
	rm -rf .tmp_versions
	rm -f *.markers *.symvers *.order

.PHONY: cleandeps
cleandeps: clean

.PHONY: realclean
realclean: clean


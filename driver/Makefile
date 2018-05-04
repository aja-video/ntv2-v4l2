#
# NTV2 v4l2 driver makefile
#
# Copyright (C) 2016 AJA Video Systems, Inc.  
#

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

default: all 
	$(MAKE) -C $(KDIR) M=$(DRIVERDIR) DRIVERDIR=$(DRIVERDIR) modules

all: $(DRIVERSRCS) $(DRIVERINCS)

.PHONY: clean cleandeps realclean

clean:
	rm -f $(BINDIR)/*.ko *.ko *.o .*o.cmd *.o.ur-safe *.mod.c *~ errors.txt semantic.cache
	rm -rf .tmp_versions
	rm -f *.markers *.symvers *.order

cleandeps: clean
realclean: clean


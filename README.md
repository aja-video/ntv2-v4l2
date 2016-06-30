# AJA NTV2 V4L2/ALSA

## Overview

This repository contains the AJA NTV2 V4L2/ALSA driver.

## Version (tag)

	v1.0.0		initial release - video/audio capture
	v1.1.0		add serial port for rovocam
	v1.2.0		add audio mixer

## Requirements

Builds in 3.10.0 <= linux (64 bit) <= 4.3.0 (approximately)

The driver has been tested on Ubuntu 14 and Centos 7.

AJA Video IO board support:
- Kona 4
- Corvid 44
- Corvid 88
- Corvid HB-R

## Building the Driver

The v4l2/alsa driver is located in /driver.  A simple 'make' will build
the driver.  If there are build errors they may be due to v4l2 interface
changes.  To manage this there are several kernel version #ifdef(s)
in ntv2_common.h.  The kernel versions are approximate; you may need
to adjust them for your kernel.

## Loading the Driver

To load the driver use:	'sudo ./load'.  The script modprobe(s) several
dependencies before loading the ntv2video.ko driver.  There is also an
unload script.  Unloading the driver requires exiting any application
that has opened the driver including the system audio mixer (pulseaudio).

## Running Some Tests

	qv4l2		/* simple v4l2 capture */
	cheese		/* can do frame grabs */
	audacity	/* capture audio */

	vlc alsa:// --input-slave v4l2://	/* capture video and audio */
	ffplay -f v4l2 -i /dev/video0		/* capture video */

## License

Copyright 2016 AJA Video Systems Inc. All rights reserved.

This program is free software; you may redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; version 2 of the License.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

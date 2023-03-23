![enter image description here](https://www.aja.com/media/images/press/aja/AJA_Logo_small.png)
# NTV2 V4L2/ALSA Kernel Driver

## Overview
This repository contains the V4L2/ALSA driver for use with the AJA's NTV2 Video I/O boards.

The driver has been tested on several Ubuntu versions and Centos 7. 
### Note
The v4l2-ntv2 driver is designed to be a standalone driver. It is impossible to use this driver in conjunction with AJA's main ntv2 kernel driver. Attempting to load both the ntv2 driver and the v4l2 driver will result in both drivers unable to load correctly.
## Version (tag)

    v1.0.0    initial release - video/audio capture
    v1.1.0    add serial port for rovocam
    v1.2.0    add audio mixer
    v1.2.1    support kernels to 4.6
    v1.2.2    support kernels to 4.14
    v2.0.0    support Kona HDMI and Kona 1
              UHD/4K SDI two sample interleave input
              SDI embedded LTC timecode
    v2.0.1    support kernels to 5.1
    v2.0.2    support kernels to 5.4
    v2.0.3    support kernels to 5.6
	
## Requirements
 - Linux Kernel (64 bit)  v3.10.0 to 5.15.0 (approximately)
 - `make` package for your distribution
## Supported AJA Products
- Kona 4
- Corvid 44
- Corvid 88
- Corvid HB-R
- Kona HDMI
- Kona 1
## Building the Driver
The v4l2 / alsa driver is located in `/driver`.  A simple `make` will build
the driver.  If there are build errors they may be due to v4l2 interface
changes.  To manage this there are several kernel version #ifdef(s)
in ntv2_common.h.  The kernel versions are approximate; you may need
to adjust them for your kernel.
## Installation
Installation is fairly straight forward. You will need to pull this repo with git, run one make command, and then use the load script to load the driver into the kernel. Note that this driver will need to be loaded on every reboot. 

	git clone https://github.com/aja-video/ntv2-v4l2.git
	cd ~/ntv2-v4l2/driver
	make
	sudo ./load


### Note to Centos 7 users
Recent updates of Centos 7 require that `NTV2_USE_SND_CARD_NEW` to be defined in
ntv2_common.h.  Uncomment (remove //) this line near the end of the file:

    //#define NTV2_USE_SND_CARD_NEW

If any system with a pre 3.16 kernel crashes when the driver loads this may also be the solution.

## Load / Unload Scrips
The `load` script modprobe(s) several dependencies before loading the ntv2video.ko driver.  There is also an unload script. Unloading the driver requires exiting any application that has opened the driver including the system audio mixer (pulseaudio).

## Example Use
List the available input devices with `v4l2-ctl`.

    v4l2-ctl --list-devices

You can capture the content off of an SDI input with `ffplay`.

     ffplay -f v4l2 -i /dev/video0 
Note that you can also replace `ffplay` with `ffmpeg` and some additional options to capture the video to disk.

    ffmpeg -f v4l2 -i /dev/video0 -c:v h264 example.mp4

You can also point VLC to the v4l2 video interface and also the alsa audio interface to capture audio and video.

    vlc alsa:// --input-slave v4l2://

### Honorable mentions 
 - You can use `qv4l2` to perform simple v4l2 capture. 
 - `Cheese` is a simple camera application that allows users to capture video or still images. 
 - `Audacity` can be used in conjunction with the alsa driver to capture audio.

   

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


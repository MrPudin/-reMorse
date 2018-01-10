#
# makefile
# Mircobit Makefile
#

.PHONY: all install remove clean
.DEFAULT: all

DIR:=$(shell yt --plain target | head -n 1 | cut -f 1 -d' ')
TARGET:=$(shell yt --plain ls | head -n 1 | cut -f 1 -d' ')


all: 
	yt target bbc-microbit-classic-gcc
	yt build

install: 
	cp build/$(DIR)/source/$(TARGET)-combined.hex  /Volumes/MICROBIT

clean:
	yt clean

remove:
	rm /mnt/MICROBIT/$(TARGET)-combined.hex

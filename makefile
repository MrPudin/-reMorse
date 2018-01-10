#
# makefile
# Mircobit Makefile
#

.PHONY: all install remove clean
.DEFAULT: all

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
MOUNT := /mnt
endif
ifeq ($(UNAME_S),Darwin)
MOUNT := /Volumes
endif

DIR:=$(shell yt --plain target | head -n 1 | cut -f 1 -d' ')
TARGET:=$(shell yt --plain ls | head -n 1 | cut -f 1 -d' ')


all: 
	yt target bbc-microbit-classic-gcc
	yt build

install: 
	cp build/$(DIR)/source/$(TARGET)-combined.hex  $(MOUNT)/MICROBIT/

clean:
	yt clean

remove:
	rm $(MOUNT)/MICROBIT/$(TARGET)-combined.hex

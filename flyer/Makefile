#
# Makefile:
#	wiringPi - A "wiring" library for the Raspberry Pi
#	https://projects.drogon.net/wiring-pi
#
#	Copyright (c) 2012-2015 Gordon Henderson
#################################################################################
# This file is part of wiringPi:
#	Wiring Compatable library for the Raspberry Pi
#
#    wiringPi is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Lesser General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    wiringPi is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with wiringPi.  If not, see <http://www.gnu.org/licenses/>.
#################################################################################

#DEBUG	= -g -O0
DEBUG	= -O3
CC	= g++
INCLUDE	= -I/usr/local/include
CFLAGS	= $(DEBUG) -Wall $(INCLUDE) -Winline -pipe

LDFLAGS	= -L/usr/local/lib
LDLIBS    = -lwiringPi -lwiringPiDev -lpthread -lm -lcrypt -lrt

PKG_OPENCV = `pkg-config --cflags --libs opencv`

# Should not alter anything below this line
###############################################################################

OBJECTS       = main.o \
				serial.o \
				queue.o \
				VideoProcess.o \
				LineCircleDetect.o\
				crc16.o

flyer: $(OBJECTS)
		$(CC) $(PKG_OPENCV) $(OBJECTS) $(INLCUDE) $(LDLIBS) -o $@ 


queue: queue.o
	$(CC) -o $@ queue.o $(PKG_OPENCV) $(INCLUDE)  $(LDLIBS)  -std=c++11
main: main.o
	$(CC) -o $@ main.o $(PKG_OPENCV) $(INCLUDE)  $(LDLIBS)  -std=c++14
serial:	serial.o
	$(CC) -o $@ serial.o $(PKG_OPENCV) $(INCLUDE)  $(LDLIBS)  -std=c++11
VideoProcess:	VideoProcess.o
	$(CC) -o $@ VideoProcess.o $(PKG_OPENCV) $(INCLUDE)  $(LDLIBS)  -std=c++11

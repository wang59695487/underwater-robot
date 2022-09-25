#DEBUG	= -g -O0
DEBUG	= -O3
CC	= g++
INCLUDE	= -I/usr/local/include
CFLAGS	= $(DEBUG) -Wall $(INCLUDE) -Winline -pipe

LDFLAGS	= -L/usr/local/lib
LDLIBS    = -lwiringPi -lwiringPiDev -lpthread -lm -lcrypt -lrt

PKG_OPENCV = `pkg-config --cflags --libs opencv`

# Should not alter anything below this line

OBJECTS       = main.o 


test: $(OBJECTS)
		$(CC) $(PKG_OPENCV) $(OBJECTS) $(INLCUDE) $(LDLIBS) -o $@ 


main: main.o
	$(CC) -o $@ main.o $(PKG_OPENCV) $(INCLUDE)  $(LDLIBS)  -std=c++14


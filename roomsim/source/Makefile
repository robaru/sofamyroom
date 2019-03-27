# Makefile for LIBROOMSIM and stand-alone version of ROOMSIM 
# Expects gcc and ar to be on the search path
CC = gcc
AR = ar
CFLAGS = -O2 -Wall -pedantic -I. \
	 -Ilibroomsim/include \
	 -Ilibfftw/win32/include \
	 -Ilibsfmt \
	 -DMEXP=19937

ROOMSIM = ../roomsim.exe
LIBROOMSIM = libroomsim/libroomsim.lib
LIBFFTW = libfftw/win32/lib/libfftw3-3.lib

EXESRCS = main.c build.c 
EXEOBJS = $(EXESRCS:.c=.obj)
EXELIBS = $(LIBROOMSIM) $(LIBFFTW)

LIBSRCS = $(wildcard libroomsim/source/*.c) libsfmt/SFMT.c
LIBOBJS = $(LIBSRCS:.c=.obj)

all:	$(ROOMSIM)

$(ROOMSIM):	$(EXEOBJS) $(EXELIBS)
	$(CC) -o $@ $^ 

$(LIBROOMSIM):	$(LIBOBJS)
	$(AR) rcs $@ $^

clean:
	$(RM) $(EXEOBJS) $(LIBOBJS) $(LIBROOMSIM)

realclean: clean
	$(RM) $(ROOMSIM) 

%.obj:	%.c
	$(CC) $(CFLAGS) -c $< -o $@


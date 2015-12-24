#
# Makefile
# Acoustics Final Project
#

CC      = g++
CFLAGS  = -g -Wall 

LIBS = -lportaudio -lncurses

EXE  = voiceSynthesizer

HDRS = phonemes.h FIR-filter-class/filt.h

SRCS = main.cpp phonemes.cpp FIR-filter-class/filt.cpp

all: $(EXE)

$(EXE): $(SRCS) $(HDRS)
	$(CC) $(CFLAGS) -o $@ $(SRCS) $(LIBS)

clean:
	rm -f *~ core $(EXE) *.o
	rm -rf delay.dSYM 

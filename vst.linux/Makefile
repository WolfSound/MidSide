#========================================
# S e t t i n g s
#========================================
CC = g++
DYN = .so
EXT =
SRC = ../vst.src/
VST = /usr/local/lib/vstsdk2.4/
SDK = $(VST)public.sdk/source/vst2.x/
FFLAGS = -fmessage-length=0 -finline-functions -fvisibility=hidden -fPIC -fno-exceptions -fno-rtti
MFLAGS = -mmmx
CFLAGS = -Wall -Os -s -c $(MFLAGS) $(FFLAGS) -I$(VST) -I$(SDK)
LFLAGS = -Wall -s -shared -lpthread
#========================================
# O b j e c t s
#========================================
# (common)
#----------------------------------------
OBJM = vstplugmain
OBJA = audioeffect
OBJX = audioeffectx
OBJC = $(OBJM).o $(OBJA).o $(OBJX).o
$(OBJL).o : $(OBJL).cpp
$(CC) $(CFLAGS) $(OBJL).cpp
$(OBJM).o : $(SDK)$(OBJM).cpp
$(CC) $(CFLAGS) $(SDK)$(OBJM).cpp
$(OBJX).o : $(SDK)$(OBJX).cpp
$(CC) $(CFLAGS) $(SDK)$(OBJX).cpp
$(OBJA).o : $(SDK)$(OBJA).cpp
$(CC) $(CFLAGS) $(SDK)$(OBJA).cpp
#----------------------------------------
# (output)
#----------------------------------------
OBJE = MidSide
OBJD = $(OBJE:=$(DYN))
#========================================
# B u i l d
#========================================
# all / clean
#----------------------------------------
all: $(OBJE)
clean:
rm -f *.o $(OBJD)
cleanobj:
rm -f *.o
#----------------------------------------
# individual
#----------------------------------------
MidSide: $(OBJC) MidSide.o
$(CC) $(LFLAGS) $(OBJC) MidSide.o -o MidSide$(DYN)
MidSide.o: $(SRC)MidSide.cpp $(SRC)MidSide.h
$(CC) $(CFLAGS) $(SRC)MidSide.cpp

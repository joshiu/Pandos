#$Id: Makefile,v 1.2 2004/05/01 14:53:48 morsiani Exp morsiani $
# Makefile for mips-linux
#
# Edited for uMPS v3 by MikeyG on 2020/04/14

INCDIR = /usr/local/include/umps3/umps
SUPDIR = /usr/local/share/umps3
LIBDIR = /usr/local/lib/umps3

DEFS = ../h/const.h ../h/types.h ../h/asl.h ../h/pcb.h $(INCDIR)/libumps.h Makefile

CFLAGS = -ffreestanding -ansi -Wall -c -mips1 -mabi=32 -mfp32 -mno-gpopt -G 0 -fno-pic -mno-abicalls

LDAOUTFLAGS = -G 0 -nostdlib -T $(SUPDIR)/umpsaout.ldscript
LDCOREFLAGS =  -G 0 -nostdlib -T $(SUPDIR)/umpscore.ldscript


CC = mipsel-linux-gnu-gcc
LD = mipsel-linux-gnu-ld
AS = mipsel-linux-gnu-as -KPIC

EF = umps3-elf2umps

#main target
all: kernel.core.umps 

kernel.core.umps: kernel
	$(EF) -k kernel

kernel: p1test.o asl.o pcb.o
	$(LD) $(LDCOREFLAGS) $(LIBDIR)/crtso.o p1test.o asl.o pcb.o $(LIBDIR)/libumps.o -o kernel

%.o: %.c $(DEFS)
	$(CC) $(CFLAGS) $<


clean:
	rm -f *.o term*.umps kernel


distclean: clean
	-rm kernel.*.umps

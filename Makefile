# KallistiOS 0.80
#
# userland/2ndmix/Makefile
# (c)2000 Dan Potter
#   
# $Id: Makefile,v 1.1 2000/11/09 05:11:08 bard Exp $

PROG = main
OBJS = main.o math.o matrix.o sqrt.o 3dutil.o 3dsloader.o

include ../Makefile.prefab

#INCS += -Imatrix -O
#LIBS += -Lsupport -Lxingmp3 -lxingmp3 -lm -lk -lgcc 
# LIBS += -Lsupport -Lxingmp3 -lxingmp3 -lk -lgcc


CC=gcc

PROJECT_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

includepath=$(DM_HOME)/include
libpath=$(DM_HOME)/bin
vpath=./

CFLAGS=-I$(includepath) -I$(PROJECT_DIR)../include/ -DDM64 -Wall
LINKFLAGS=-L$(libpath) -ldmdpi -Wall

CFILES=${wildcard *.c}
OBJ=${patsubst %.c, %.o, ${CFILES}}
TARGET=${patsubst %.o, %, ${OBJ}}


.PHONY : all clean rebuild

all : $(TARGET)

$(TARGET) : % : %.o
	$(CC) $(CFLAGS) -o $@ $< $(LINKFLAGS)

clean:
	rm -rf $(TARGET) $(OBJ)

rebuild : clean all

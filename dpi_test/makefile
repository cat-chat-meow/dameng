CC=gcc

includepath=$(DM_HOME)/include
libpath=$(DM_HOME)/bin
vpath=./

CFLAGS=-I$(includepath) -DDM64 -Wall
LINKFLAGS=-L$(libpath) -ldmdpi -Wall

%.o:%.c
	$(CC) -g -c $(CFLAGS) $< -o $@

object_file1=dpi_conn.o
object_file2=dpi_dml.o
object_file3=dpi_bind.o
object_file4=dpi_lob.o

object_files=dpi_conn.o dpi_dml.o dpi_bind.o dpi_lob.o

final_objects=dpi_conn dpi_dml dpi_bind dpi_lob

all : $(final_objects)

.PHONY : all clean rebuild

dpi_conn : $(object_file1)
	$(CC) -o $@ $(object_file1) -g $(LINKFLAGS)
	@echo make ok.

dpi_dml : $(object_file2)
	$(CC) -o $@ $(object_file2) -g $(LINKFLAGS)
	@echo make ok.

dpi_bind : $(object_file3)
	$(CC) -o $@ $(object_file3) -g $(LINKFLAGS)
	@echo make ok.

dpi_lob : $(object_file4)
	$(CC) -o $@ $(object_file4) -g $(LINKFLAGS)
	@echo make ok.

clean :
	@rm -rf $(object_files)
	@rm -rf $(final_objects)

rebuild : clean all

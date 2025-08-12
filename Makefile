#implicit deffinitions for gcc and flags
CC = gcc
CFLAGS = -Wall -03
CFLAGSDEBUG = -g -Wall -03

src_files = gm_creature.c gm_helper.c gm_init.c gm_KNN.c gm_main.c gm_routine.c
header_files = gm_creature.h gm_helper.h gm_init.h gm_KNN.h gm_main.h gm_routine.h
object_files = gm_creature.o gm_helper.o gm_init.o gm_KNN.o gm_main.o gm_routine.o

#compiles the object files into an executable
all: $(object_files)
	$(CC) $(CFLAGS) $^ -o GM.out

#compiles the object files into an debugable executable
debug: $(object_files)
	$(CC) $(CFLAGSDEBUG) $^ -o Debug.out

#compiles the source files into object files
$(object_files): $(src_files) $(header_files)
	$(CC) $(CFLAGS) -c $^

#if we are missing any .c or .h files inform the user
$(src_files) $(header_files):
	@echo "Missing source file(s)!"
	@echo "Files requred: $(src_files) $(header_files)"

#deletes the object files
clean:
	rm -f $(object_files)
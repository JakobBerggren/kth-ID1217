CC = gcc
LIBS =
CFLAGS = -O -fopenmp

SOURCE = src
BUILD = build
RESULT = result

TARGETS = jacobi_seq jacobi_parallel

DEFINES = NONE

all: $(TARGETS)
.PHONY: all

#build
jacobi_seq: $(SOURCE)/jacobi_seq.c
	@mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -o $(BUILD)/$@ $(SOURCE)/$@.c $(LIBS)

jacobi_parallel: $(SOURCE)/jacobi_parallel.c
	@mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -o $(BUILD)/$@ $(SOURCE)/$@.c $(LIBS)


#clean
clean: 
	rm -f *.o *.exe $(TARGETS)
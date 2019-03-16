CC = gcc
LIBS =
CFLAGS = -O -fopenmp

SOURCE = src
BUILD = build
RESULT = result

TARGETS = jacobi_seq jacobi_parallel

BENCHMARKS = benchmark-jacobi_seq benchmark-jacobi_parallel

DEFINES = NONE

all: $(TARGETS) $(BENCHMARKS) clean
.PHONY: all

#build
jacobi_seq: $(SOURCE)/jacobi_seq.c
	@mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -o $(BUILD)/$@ $(SOURCE)/$@.c $(LIBS)

jacobi_parallel: $(SOURCE)/jacobi_parallel.c
	@mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -o $(BUILD)/$@ $(SOURCE)/$@.c $(LIBS)

#run
benchmark-jacobi_seq:
	@mkdir -p $(RESULT)
	./$(BUILD)/jacobi_seq 10 100 4 > $(RESULT)/$@-result.md
benchmark-jacobi_parallel:
	@mkdir -p $(RESULT)
	./$(BUILD)/jacobi_parallel 10 100 4 > $(RESULT)/$@-result.md


#clean
clean: 
	rm -f *.o *.exe *.out $(TARGETS)

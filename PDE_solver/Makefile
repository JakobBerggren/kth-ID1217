CC = gcc
LIBS =
CFLAGS = -O -fopenmp

SOURCE = src
BUILD = build
RESULT = result

TARGETS = jacobi_seq jacobi_parallel multigrid_seq multigrid_parallel

BENCHMARKS = benchmark-multigrid_seq

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

multigrid_seq: $(SOURCE)/multigrid_seq.c
	@mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -o $(BUILD)/$@ $(SOURCE)/$@.c $(LIBS)

multigrid_parallel: $(SOURCE)/multigrid_parallel.c
	@mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -o $(BUILD)/$@ $(SOURCE)/$@.c $(LIBS)

#run
benchmark-jacobi_seq:
	@mkdir -p $(RESULT)
	@rm -f $(RESULT)/$@-result.md
	./$(BUILD)/jacobi_seq 100 1000000 >> $(RESULT)/$@-result.md
	./$(BUILD)/jacobi_seq 100 1000000 >> $(RESULT)/$@-result.md
	./$(BUILD)/jacobi_seq 100 1000000 >> $(RESULT)/$@-result.md
	./$(BUILD)/jacobi_seq 100 1000000 >> $(RESULT)/$@-result.md
	./$(BUILD)/jacobi_seq 100 1000000 >> $(RESULT)/$@-result.md

	./$(BUILD)/jacobi_seq 200 200000 >> $(RESULT)/$@-result.md
	./$(BUILD)/jacobi_seq 200 200000 >> $(RESULT)/$@-result.md
	./$(BUILD)/jacobi_seq 200 200000 >> $(RESULT)/$@-result.md
	./$(BUILD)/jacobi_seq 200 200000 >> $(RESULT)/$@-result.md
	./$(BUILD)/jacobi_seq 200 200000 >> $(RESULT)/$@-result.md
	
benchmark-jacobi_parallel:
	@mkdir -p $(RESULT)
	@rm -f $(RESULT)/$@-result.md
	./$(BUILD)/jacobi_parallel 100 1000000 1 >> $(RESULT)/$@-result.md
	./$(BUILD)/jacobi_parallel 100 1000000 1 >> $(RESULT)/$@-result.md
	./$(BUILD)/jacobi_parallel 100 1000000 1 >> $(RESULT)/$@-result.md
	./$(BUILD)/jacobi_parallel 100 1000000 1 >> $(RESULT)/$@-result.md
	./$(BUILD)/jacobi_parallel 100 1000000 1 >> $(RESULT)/$@-result.md

	./$(BUILD)/jacobi_parallel 100 1000000 2 >> $(RESULT)/$@-result.md
	./$(BUILD)/jacobi_parallel 100 1000000 2 >> $(RESULT)/$@-result.md
	./$(BUILD)/jacobi_parallel 100 1000000 2 >> $(RESULT)/$@-result.md
	./$(BUILD)/jacobi_parallel 100 1000000 2 >> $(RESULT)/$@-result.md
	./$(BUILD)/jacobi_parallel 100 1000000 2 >> $(RESULT)/$@-result.md

	./$(BUILD)/jacobi_parallel 100 1000000 3 >> $(RESULT)/$@-result.md
	./$(BUILD)/jacobi_parallel 100 1000000 3 >> $(RESULT)/$@-result.md
	./$(BUILD)/jacobi_parallel 100 1000000 3 >> $(RESULT)/$@-result.md
	./$(BUILD)/jacobi_parallel 100 1000000 3 >> $(RESULT)/$@-result.md
	./$(BUILD)/jacobi_parallel 100 1000000 3 >> $(RESULT)/$@-result.md

	./$(BUILD)/jacobi_parallel 100 1000000 4 >> $(RESULT)/$@-result.md
	./$(BUILD)/jacobi_parallel 100 1000000 4 >> $(RESULT)/$@-result.md
	./$(BUILD)/jacobi_parallel 100 1000000 4 >> $(RESULT)/$@-result.md
	./$(BUILD)/jacobi_parallel 100 1000000 4 >> $(RESULT)/$@-result.md
	./$(BUILD)/jacobi_parallel 100 1000000 4 >> $(RESULT)/$@-result.md
	
	./$(BUILD)/jacobi_parallel 200 200000 1 >> $(RESULT)/$@-result.md
	./$(BUILD)/jacobi_parallel 200 200000 1 >> $(RESULT)/$@-result.md
	./$(BUILD)/jacobi_parallel 200 200000 1 >> $(RESULT)/$@-result.md
	./$(BUILD)/jacobi_parallel 200 200000 1 >> $(RESULT)/$@-result.md
	./$(BUILD)/jacobi_parallel 200 200000 1 >> $(RESULT)/$@-result.md

	./$(BUILD)/jacobi_parallel 200 200000 2 >> $(RESULT)/$@-result.md
	./$(BUILD)/jacobi_parallel 200 200000 2 >> $(RESULT)/$@-result.md
	./$(BUILD)/jacobi_parallel 200 200000 2 >> $(RESULT)/$@-result.md
	./$(BUILD)/jacobi_parallel 200 200000 2 >> $(RESULT)/$@-result.md
	./$(BUILD)/jacobi_parallel 200 200000 2 >> $(RESULT)/$@-result.md
	
	./$(BUILD)/jacobi_parallel 200 200000 3 >> $(RESULT)/$@-result.md
	./$(BUILD)/jacobi_parallel 200 200000 3 >> $(RESULT)/$@-result.md
	./$(BUILD)/jacobi_parallel 200 200000 3 >> $(RESULT)/$@-result.md
	./$(BUILD)/jacobi_parallel 200 200000 3 >> $(RESULT)/$@-result.md
	./$(BUILD)/jacobi_parallel 200 200000 3 >> $(RESULT)/$@-result.md

	./$(BUILD)/jacobi_parallel 200 200000 4 >> $(RESULT)/$@-result.md
	./$(BUILD)/jacobi_parallel 200 200000 4 >> $(RESULT)/$@-result.md
	./$(BUILD)/jacobi_parallel 200 200000 4 >> $(RESULT)/$@-result.md
	./$(BUILD)/jacobi_parallel 200 200000 4 >> $(RESULT)/$@-result.md
	./$(BUILD)/jacobi_parallel 200 200000 4 >> $(RESULT)/$@-result.md

benchmark-multigrid_seq:
	@mkdir -p $(RESULT)
	@rm -f $(RESULT)/$@-result.md
	./$(BUILD)/multigrid_seq 12 10000000 >> $(RESULT)/$@-result.md
	./$(BUILD)/multigrid_seq 12 10000000 >> $(RESULT)/$@-result.md
	./$(BUILD)/multigrid_seq 12 10000000 >> $(RESULT)/$@-result.md
	./$(BUILD)/multigrid_seq 12 10000000 >> $(RESULT)/$@-result.md
	./$(BUILD)/multigrid_seq 12 10000000 >> $(RESULT)/$@-result.md



	./$(BUILD)/multigrid_seq 24 4000000 >> $(RESULT)/$@-result.md
	./$(BUILD)/multigrid_seq 24 4000000 >> $(RESULT)/$@-result.md
	./$(BUILD)/multigrid_seq 24 4000000 >> $(RESULT)/$@-result.md
	./$(BUILD)/multigrid_seq 24 4000000 >> $(RESULT)/$@-result.md
	./$(BUILD)/multigrid_seq 24 4000000 >> $(RESULT)/$@-result.md
	

benchmark-multigrid_parallel:
	@mkdir -p $(RESULT)
	@rm -f $(RESULT)/$@-result.md
	./$(BUILD)/multigrid_parallel 12 10000000 1 >> $(RESULT)/$@-result.md
	./$(BUILD)/multigrid_parallel 12 10000000 1 >> $(RESULT)/$@-result.md
	./$(BUILD)/multigrid_parallel 12 10000000 1 >> $(RESULT)/$@-result.md
	./$(BUILD)/multigrid_parallel 12 10000000 1 >> $(RESULT)/$@-result.md
	./$(BUILD)/multigrid_parallel 12 10000000 1 >> $(RESULT)/$@-result.md

	./$(BUILD)/multigrid_parallel 12 10000000 2 >> $(RESULT)/$@-result.md
	./$(BUILD)/multigrid_parallel 12 10000000 2 >> $(RESULT)/$@-result.md
	./$(BUILD)/multigrid_parallel 12 10000000 2 >> $(RESULT)/$@-result.md
	./$(BUILD)/multigrid_parallel 12 10000000 2 >> $(RESULT)/$@-result.md
	./$(BUILD)/multigrid_parallel 12 10000000 2 >> $(RESULT)/$@-result.md

	./$(BUILD)/multigrid_parallel 12 10000000 3 >> $(RESULT)/$@-result.md
	./$(BUILD)/multigrid_parallel 12 10000000 3 >> $(RESULT)/$@-result.md
	./$(BUILD)/multigrid_parallel 12 10000000 3 >> $(RESULT)/$@-result.md
	./$(BUILD)/multigrid_parallel 12 10000000 3 >> $(RESULT)/$@-result.md
	./$(BUILD)/multigrid_parallel 12 10000000 3 >> $(RESULT)/$@-result.md

	./$(BUILD)/multigrid_parallel 12 10000000 4 >> $(RESULT)/$@-result.md
	./$(BUILD)/multigrid_parallel 12 10000000 4 >> $(RESULT)/$@-result.md
	./$(BUILD)/multigrid_parallel 12 10000000 4 >> $(RESULT)/$@-result.md
	./$(BUILD)/multigrid_parallel 12 10000000 4 >> $(RESULT)/$@-result.md
	./$(BUILD)/multigrid_parallel 12 10000000 4 >> $(RESULT)/$@-result.md

	./$(BUILD)/multigrid_parallel 24 40000000 1 >> $(RESULT)/$@-result.md
	./$(BUILD)/multigrid_parallel 24 40000000 1 >> $(RESULT)/$@-result.md
	./$(BUILD)/multigrid_parallel 24 40000000 1 >> $(RESULT)/$@-result.md
	./$(BUILD)/multigrid_parallel 24 40000000 1 >> $(RESULT)/$@-result.md
	./$(BUILD)/multigrid_parallel 24 40000000 1 >> $(RESULT)/$@-result.md

	./$(BUILD)/multigrid_parallel 24 40000000 2 >> $(RESULT)/$@-result.md
	./$(BUILD)/multigrid_parallel 24 40000000 2 >> $(RESULT)/$@-result.md
	./$(BUILD)/multigrid_parallel 24 40000000 2 >> $(RESULT)/$@-result.md
	./$(BUILD)/multigrid_parallel 24 40000000 2 >> $(RESULT)/$@-result.md
	./$(BUILD)/multigrid_parallel 24 40000000 2 >> $(RESULT)/$@-result.md

	./$(BUILD)/multigrid_parallel 24 40000000 3 >> $(RESULT)/$@-result.md
	./$(BUILD)/multigrid_parallel 24 40000000 3 >> $(RESULT)/$@-result.md
	./$(BUILD)/multigrid_parallel 24 40000000 3 >> $(RESULT)/$@-result.md
	./$(BUILD)/multigrid_parallel 24 40000000 3 >> $(RESULT)/$@-result.md
	./$(BUILD)/multigrid_parallel 24 40000000 3 >> $(RESULT)/$@-result.md

	./$(BUILD)/multigrid_parallel 24 40000000 4 >> $(RESULT)/$@-result.md
	./$(BUILD)/multigrid_parallel 24 40000000 4 >> $(RESULT)/$@-result.md
	./$(BUILD)/multigrid_parallel 24 40000000 4 >> $(RESULT)/$@-result.md
	./$(BUILD)/multigrid_parallel 24 40000000 4 >> $(RESULT)/$@-result.md
	./$(BUILD)/multigrid_parallel 24 40000000 4 >> $(RESULT)/$@-result.md


#clean
clean: 
	rm -f *.o *.exe *.out $(TARGETS)

CC = mpicc

EXEC = mpiexec

BUILD = build

TARGETS = distributedPairing

all: $(TARGETS)
.PHONY: all

distributedPairing: ./distributedPairing.c
	@mkdir -p $(BUILD)
	$(CC) -o $(BUILD)/$@ $@.c 

run-distributedPairing: distributedPairing
	mpiexec -np 4 $(BUILD)/distributedPairing
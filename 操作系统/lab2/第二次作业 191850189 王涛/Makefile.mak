CXX = g++
NASM = nasm
NASMFLAGS = -f elf32

EXEC = main

all: $(EXEC)

$(EXEC): asm_print.o main.o
	$(CXX) $^ -o $@

asm_print.o: asm_print.asm
	$(NASM) $(NASMFLAGS) $< -o $@

main.o: main.cpp
	$(CXX) -c $< -o $@

clean:
	rm asm_print.o
	rm main.o
	rm main
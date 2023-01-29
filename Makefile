all: compile

compile:
	make clean
	./assembler.linux jsr_test.asm output.obj
	gcc -std=c99 -g -o simulate lc3bsim2.c
	./simulate output.obj

clean:
	rm -f simulate
	rm -f *.obj
	rm -f dumpsim

debug:
	make clean
	./assembler.linux *.asm output.obj
	gcc -std=c99 -g -o simulate lc3bsim2.c
	gdb --args ./simulate output.obj
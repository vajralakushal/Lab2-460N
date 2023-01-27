all: compile

compile:
	gcc -std=c99 -g -o simulate lc3bsim2.c

clean:
	rm simulate
	rm output.obj
	rm dumpsim
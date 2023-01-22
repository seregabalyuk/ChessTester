
all: bin/tester

RES_TESTER = src/tester.c src/rulechecker.c src/dialog.c

bin/tester: bin $(RES_TESTER)
	gcc -o bin/tester $(RES_TESTER)


bin: 
	mkdir bin

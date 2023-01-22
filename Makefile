
all: bin/tester bin/chess

RES_TESTER = src/tester.c src/rulechecker.c src/dialog.c

bin/tester: bin $(RES_TESTER)
	gcc -o bin/tester $(RES_TESTER)

bin/chess: bin src/easy.cpp
	g++ -o bin/chess src/easy.cpp

bin: 
	mkdir bin

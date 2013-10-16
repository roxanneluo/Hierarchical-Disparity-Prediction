all : checke checke1
	g++ code/silly.cpp -o bin/silly.out -O2 -Wall
checke :
	g++ code/checker.cpp -o bin/checker.out -O2 -Wall
checke1:
	g++ code/checker1.cpp -o bin/checker1.out -O2 -Wall
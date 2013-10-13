all : checke
	g++ code/silly.cpp -o bin/silly.out -O2 -Wall
checke :
	g++ code/checker.cpp -o bin/checker.out -O2 -Wall
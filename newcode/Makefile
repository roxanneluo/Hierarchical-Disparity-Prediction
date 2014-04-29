all :
	echo hehe

compile-mst :
	g++ silly0.cpp -O2

compile-dpf-mst :
	g++ silly1.cpp -O2 -msse3 -std=c++0x
	
compile-st :
	g++ silly2.cpp -O2
	
compile-dpf-st :
	g++ silly3.cpp -O2 -msse3 -std=c++0x

run-default :
	time ./a.out	

run-default-shrink :
	time ./a.out skL2.ppm skR2.ppm 4 64 
	
run-aloe :
	time ./a.out Aloe/left.ppm Aloe/right.ppm 135 2 left_aloe.pgm right_aloe.pgm

run-baby2 :
	time ./a.out Baby2/left.ppm Baby2/right.ppm 150 2 left_baby2.pgm right_baby2.pgm

run-old :
	time ./old.out Aloe/left.ppm Aloe/right.ppm 135 2 left_aloe.pgm right_aloe.pgm


all : compile-mst compile-dpf-mst compile-st compile-dpf-st compile-rt compile-dpf-rt checker0

compile-checker:
	g++ checker_nonocc.cpp -O2 -o ../bin/checker.out 
compile-mst :
	g++ silly0.cpp -O2
	cp a.out ../bin/mst.out
compile-dpf-mst :
	g++ silly1.cpp -O2 -msse3 -std=c++0x
	cp a.out ../bin/dpf-mst.out
compile-st :
	g++ silly2.cpp -O2
	cp a.out ../bin/st.out
compile-dpf-st :
	g++ silly3.cpp -O2 -msse3 -std=c++0x
	cp a.out ../bin/dpf-st.out
compile-rt :
	g++ silly4.cpp -O2 
	cp a.out ../bin/rt.out
compile-dpf-rt:
	g++ silly5.cpp -O2 -msse3 -std=c++0x
	cp a.out ../bin/dpf-rt.out
run-default :
	time ./a.out	

run-default-shrink :
	time ./a.out skL2.ppm skR2.ppm 4 64 
	
run-aloe :
	time ./a.out Aloe/left.ppm Aloe/right.ppm 135 2 left_aloe.pgm right_aloe.pgm

run-baby2 :
	time ./a.out Baby2/left.ppm Baby2/right.ppm 150 2 left_baby2.pgm right_baby2.pgm
run-baby1 :
	time ./a.out Baby1/left.ppm Baby1/right.ppm 300 1 left_baby1.pgm right_baby1.pgm
run-old :
	time ./old.out Aloe/left.ppm Aloe/right.ppm 135 2 left_aloe.pgm right_aloe.pgm


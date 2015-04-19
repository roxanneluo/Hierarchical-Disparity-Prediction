all : compile-mst compile-dpf-mst compile-st compile-dpf-st compile-rt compile-dpf-rt  compile-checker compile-ppm-checker

compile-checker:
	g++-4.8 checker_nonocc.cpp -O2 -o ../bin/checker.out 
compile-ppm-checker:
	g++-4.8 ppm_checker_nonocc.cpp -O2 -o ../bin/ppm-checker.out 
compile-mst :
	g++-4.8 silly0.cpp -O2 -o ../bin/mst.out
compile-dpf-mst :
	g++-4.8 silly1.cpp -O2 -msse3 -std=c++0x -o ../bin/dpf-mst-mean-mst-GMM-0.5.out
compile-st :
	g++-4.8 silly2.cpp -O2 -o ../bin/st.out
compile-dpf-st :
	g++-4.8 silly3.cpp -O2 -msse3 -std=c++0x -o ../bin/dpf-st-mean-mst-GMM-0.5.out
compile-rt :
	g++-4.8 silly4.cpp -O2 -o ../bin/rt.out
compile-dpf-rt:
	g++-4.8 silly5.cpp -O2 -msse3 -std=c++0x -o ../bin/dpf-rt-mean-mst-GMM-0.5.out
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


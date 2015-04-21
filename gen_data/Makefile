all: compile-gen-concur-dpf-mst compile-gen-concur-dpf-st compile-gen-concur-dpf-rt

compile-gen-concur-dpf-mst:
	g++-4.8 gen_concur1.cpp -msse3 -std=c++0x -ggdb -o ../bin/gen-concur-dpf-mst.out
compile-gen-concur-dpf-st:
	g++-4.8 gen_concur3.cpp -msse3 -std=c++0x -ggdb -o ../bin/gen-concur-dpf-st.out
compile-gen-concur-dpf-rt:
	g++-4.8 gen_concur5.cpp -msse3 -std=c++0x -ggdb -o ../bin/gen-concur-dpf-rt.out

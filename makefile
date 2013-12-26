all : silly0 silly1 checke0 checke1 checke2 checker_print_err MST_blind silly_MSF Prob_Gen cnt_Gen SgL_Gen gen_interval_test MSF2 Stable_RAND Stable_MST silly2
silly0:
	g++ code/silly.cpp -o bin/Original.out -O2 -Wall -std=c++0x
checke0 :
	g++ code/checker.cpp -o bin/checker.out -O2 -Wall -std=c++0x
silly1 :
	g++ code/silly1.cpp -o bin/RandTree.out -O2 -Wall -std=c++0x
checke1 :
	g++ code/checker1.cpp -o bin/checker1.out -O2 -Wall -std=c++0x
checke2 :
	g++ code/checker_supportmap.cpp -o bin/checker_support.out -O2 -Wall -std=c++0x
checker_print_err :
	g++ code/checker2.cpp -o bin/checker_print_err.out -O2 -Wall -std=c++0x
MST_blind :
	g++ code/MST_blind.cpp -o bin/MST_blind.out -O2 -Wall -std=c++0x
silly_MSF :
	g++ code/silly_MSF.cpp -o bin/MSF.out -O2 -Wall -std=c++0x -msse3
Prob_Gen :
	g++ code/code-probability/prob_data_generator.cpp -o bin/Prob_Gen.out -O2 -Wall -std=c++0x -msse3
cnt_Gen:
	g++ code/gen_support_cnt.cpp -o bin/cnt_Gen.out -O2 -Wall -std=c++0x -msse3
SgL_Gen:
	g++ code/code-probability/gen_small_given_large_matrix.cpp -o bin/SgL_Gen.out -O2 -Wall -std=c++0x
gen_interval_test :
	g++ code/gen_interval_test.cpp -o bin/gen_interval_test.out -O2 -Wall -std=c++0x
MSF2:
	g++ code/silly_MSF2.cpp -o bin/MSF2.out -O2 -Wall -std=c++0x -msse3
Stable_RAND:
	g++ code/stablepoints_rand.cpp -o bin/Stable_RAND.out -O2 -Wall -std=c++0x
Stable_MST:
	g++ code/stablepoints_MST.cpp -o bin/Stable_MST.out -O2 -Wall -std=c++0x
silly2 :
	g++ code/silly2.cpp -o bin/RandTreeEdgeAware.out -O2 -Wall -std=c++0x

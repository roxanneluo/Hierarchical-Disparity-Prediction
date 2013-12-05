#ifndef GEN_INTERVAL_H
#define GEN_INTERVAL_H
#include <vector>
#include "gridcode.h"
#include "forests2.h"
#include "arithmetics.h"
#include "iomanager.h"
#include "timekeeper.h"
#include "filter.h"
#include "filename.h"
#include "normrnd.h"

inline int dcmp(double a, double b) {
  if (a - b > 1e-9) return 1;
	if (a - b < -1e-9) return -1;
	return 0;
}

void save_large_given_small(Grid<double>& prob_matrix,
		                        const char* dataset) {
	char path[100] = "large_given_small_ref_2_gmm/";
  strcat(path, dataset);
	strcat(path, "_LgS.txt");
  FILE* file = fopen(path, "w");
	for (int i = 0; i < prob_matrix.height; ++i) {
	  for (int j = 0; j < prob_matrix.width; ++j) {
		  fprintf(file, "%.9lf ", prob_matrix[i][j]);
		}
		fprintf(file, "\n");
	}
	fclose(file);
}
void save_initial_cnt(Array1<double>& initial_prob,
											char* dataset) {
	char path[100] = "all_prob_gen/Prob_Gen_";
	strcat(path, dataset);
	strcat(path, "__cnt_c++.txt");
	FILE* file = fopen(path, "w");
	for (int i = 0; i < initial_prob.length; ++i)
		fprintf(file, "%.5lf ", initial_prob[i]);
	fprintf(file, "\n");
	fclose(file);	
}
void gen_support_prob(Grid<unsigned char>& support,
		                  Array1<double>& support_prob,
											int max_disparity) {
	support_prob.zero();
	int W = support.width, H = support.height;
	int d;
	for (int i = 0; i < H; ++i) {
	  for (int j = 0; j < W; ++j) {
		  d = support[i][j];
			if (d != 0) {
				if (d <= max_disparity)
					support_prob[d]++;
			}
		}
	}
	// normalize
	support_prob.normalize();
	// add noise
	support_prob.addNoise(0.05);
  // for (int i = 0; i < support_prob.length; ++i) {
	//   printf("%d %.5lf\n", i, support_prob[i]);
	// }
}

void gen_support_prob(Grid<unsigned char>& support_left,
		                  Grid<unsigned char>& support_right,
		                  Array1<double>& support_prob,
											int max_disparity) {
	support_prob.zero();
	int W = support_left.width, H = support_left.height;
	int d;
	for (int i = 0; i < H; ++i) {
	  for (int j = 0; j < W; ++j) {
		  d = support_left[i][j];
			if (d != 0) {
				if (d <= max_disparity)
					support_prob[d]++;
			}
			d = support_right[i][j];
			if (d != 0) {
			  if (d <= max_disparity)
					support_prob[d]++;
			}
		}
	}
	// normalize
	support_prob.normalize();
	// add noise
	support_prob.addNoise(0.05);
}
void gen_initial_prob(Grid<unsigned char>& initial_d,
		                  Array1<double>& initial_prob,
											int max_disparity) {
  initial_prob.zero();
	int W = initial_d.width, H = initial_d.height;
	int d;
	for (int i = 0; i < H; ++i) {
	  for (int j = 0; j < W; ++j) {
			d = initial_d[i][j];
			if (d <= max_disparity)
				initial_prob[d]++;
		}
	}
	// normalize
  initial_prob.normalize();
	// for (int i = 0; i < initial_prob.length; ++i) {
	//   printf("%d %.5lf\n", i, initial_prob[i]);
	// }
}
void gen_initial_prob(Grid<unsigned char>& initial_d_left,
		                  Grid<unsigned char>& initial_d_right,
		                  Array1<double>& initial_prob,
											int max_disparity) {
  initial_prob.zero();
	int W = initial_d_left.width, H = initial_d_left.height;
	int d;
	for (int i = 0; i < H; ++i) {
	  for (int j = 0; j < W; ++j) {
			d = initial_d_left[i][j];
			if (d <= max_disparity)
				initial_prob[d]++;
			d = initial_d_right[i][j];
			if (d <= max_disparity)
				initial_prob[d]++;
		}
	}
	// normalize
  // initial_prob.normalize();
}
// prob_matrix(initial_d_max + 1, initial_d_max * 2 - 1)
void read_prob_matrix (char* path, vector<double>& gmm) {
	FILE* file = fopen(path, "r");
	double tmp;
	while (fscanf(file, "%lf", &tmp) == 1) {
	  gmm.push_back(tmp);
//		printf("%.5lf\n", tmp);
	}
	// prob_matrix.reset(prob.size(), prob.size() * 2 - 1);
	fclose(file);
}

void read_prob_matrix (char* path0,
		                   char* path1,
											 vector<double>& gmm0,
											 vector<double>& gmm1) {
  FILE * file = fopen(path0, "r");
	double tmp;
	while(fscanf(file, "%lf", &tmp) == 1) {
	  gmm0.push_back(tmp);
	}
  fclose(file);
	file = fopen(path1, "r");
	while(fscanf(file, "%lf", &tmp) == 1) {
	  gmm1.push_back(tmp);
//		cout << tmp << endl;
	}
	fclose(file);
}

void gen_small_given_large (Grid<double>& prob_matrix,
		                        const vector<double>& gmm) {
	prob_matrix.zero();
	for (int j = 0; j < prob_matrix.width; ++j) {
		for (int i = 1; j / 2 - i >= 0; ++i) {
		  prob_matrix[j / 2 - i][j] = gmm[gmm.size() / 2 - i];
		}
	  for (int i = 0; j / 2 + i < prob_matrix.height; ++i) {
		  prob_matrix[j / 2 + i][j] = gmm[gmm.size() / 2 + i];
		}
	}
}

void gen_small_given_large (Grid<double>& prob_matrix,
		                        Array1<double>& gmm) {
	prob_matrix.zero();
	genGMM(gmm);
	for (int j = 0; j < prob_matrix.width; ++j) {
		for (int i = 1; j / 2 - i >= 0; ++i) {
		  prob_matrix[j / 2 - i][j] = gmm[gmm.length / 2 - i];
		}
	  for (int i = 0; j / 2 + i < prob_matrix.height; ++i) {
		  prob_matrix[j / 2 + i][j] = gmm[gmm.length / 2 + i];
		}
	}
}

void gen_small_given_large (Grid<double>& prob_matrix,
		                        const vector<double>& gmm0,
														const vector<double>& gmm1) {
  prob_matrix.zero();
	for (int j = 0; j < prob_matrix.width; j += 2) {
	  for (int i = 1; j / 2 - i >= 0; ++i)
			prob_matrix[j / 2 - i][j] = gmm0[gmm0.size() / 2 - i];
		for (int i = 0; j / 2 + i < prob_matrix.height; ++i)
			prob_matrix[j / 2 + i][j] = gmm0[gmm0.size() / 2 + i];
	}
	for (int j = 1; j < prob_matrix.width; j += 2) {
	  for (int i = 1; j / 2 - i >= 0; ++i)
			prob_matrix[j / 2 - i][j] = gmm1[gmm1.size() / 2 - i];
		for (int i = 0; j / 2 + i < prob_matrix.height; ++i)
			prob_matrix[j / 2 + i][j] = gmm1[gmm1.size() / 2 + i];
	}
  //  save_large_given_small(prob_matrix, "initial");
}

void gen_small_given_large (Grid<double>& prob_matrix,
		                        Array1<double>& gmm0,
														Array1<double>& gmm1) {
  prob_matrix.zero();
	genGMM(gmm0, 0);
	genGMM(gmm1, 1);
	for (int j = 0; j < prob_matrix.width; j += 2) {
	  for (int i = 1; j / 2 - i >= 0; ++i)
			prob_matrix[j / 2 - i][j] = gmm0[gmm0.length / 2 - i];
		for (int i = 0; j / 2 + i < prob_matrix.height; ++i)
			prob_matrix[j / 2 + i][j] = gmm0[gmm0.length / 2 + i];
	}
	for (int j = 1; j < prob_matrix.width; j += 2) {
	  for (int i = 1; j / 2 - i >= 0; ++i)
			prob_matrix[j / 2 - i][j] = gmm1[gmm1.length / 2 - i];
		for (int i = 0; j / 2 + i < prob_matrix.height; ++i)
			prob_matrix[j / 2 + i][j] = gmm1[gmm1.length / 2 + i];
	}
	// save_large_given_small(prob_matrix, "initial");
}
void gen_large_given_small (Array1<double>& initial_prob,
		                  Grid<double>& prob_matrix,
											Array1<double>& support_prob) {
  int W = support_prob.length, H = initial_prob.length;
  for (int i = 0 ; i < H; ++i) {
	  for (int j = 0; j < W; ++j) {
		  if (dcmp(initial_prob[i], 0) == 0 /*initial_prob[i] == 0.0*/) {
			  prob_matrix[i][j] = 0;
			} else {
				prob_matrix[i][j] /= initial_prob[i];
			  prob_matrix[i][j] *= support_prob[j];
			}
		}
	}
	prob_matrix.normalize(2);
}

int gen_interval(Grid<double>& prob_matrix,
		             Grid<int>& interval,
								 double threshold) {
  // interval[initial_max_disparity][2]
  // interval[d][0]: lower boundary of possible interval.
  // interval[d][1]: upper boundary of possible interval.	
  int err = 0; // calculate the number of wrong interval.
	for (int i = 0; i < prob_matrix.height; ++i) {
	  int j = 0;
		while (dcmp(prob_matrix[i][j], threshold) == -1 && 
				   j < prob_matrix.width - 1) j++;
		interval[i][0] = j;
		j = prob_matrix.width - 1;
		while (dcmp(prob_matrix[i][j], threshold) == -1 &&
				   j > 0) j--;
		interval[i][1] = j;
    // wrong interval
		
		if (interval[i][0] > interval[i][1]) {
		  interval[i][0] = 0;
			interval[i][1] = 0;
			err++;
		}	
	}
	return err;
}

int gen_interval_mid(Grid<double>& prob_matrix,
		                 Grid<int>& interval,
										 double threshold) {
  double highest;
	int highest_index;
	for (int i = 0; i < prob_matrix.height; ++i) {
	  highest = prob_matrix[i][0];
		highest_index = 0;
		for (int j = 1; j < prob_matrix.width; ++j) {
		  if (highest < prob_matrix[i][j]) {
			  highest = prob_matrix[i][j];
				highest_index = j;
			}
		}
		if (dcmp(highest, 0.0) == 0) {
		  interval[i][0] = 0;
			interval[i][1] = -1;
			continue;
		}
    interval[i][0] = highest_index;
		interval[i][1] = highest_index;

		/*
		while(interval[i][0] > 0 &&
				  dcmp(prob_matrix[i][interval[i][0]], threshold) == 1) {
		  interval[i][0]--;
		}
		while(interval[i][1] < prob_matrix.width - 1 &&
				  dcmp(prob_matrix[i][interval[i][1]], threshold) == 1) {
		  interval[i][1]++;
		}*/

		double total = 0.0;
		bool mark = true;
		while(mark && interval[i][0] > 0 && interval[i][1] < prob_matrix.width - 1) {
		  mark = false;
			if (dcmp(prob_matrix[i][interval[i][0]]  / (prob_matrix[i][interval[i][0]] + total) , threshold) == 1) {
				total += prob_matrix[i][interval[i][0]];
				interval[i][0]--;
				mark = true;
			}
			if (dcmp(prob_matrix[i][interval[i][1]] / (prob_matrix[i][interval[i][1]] + total), threshold) == 1) {
			  total += prob_matrix[i][interval[i][1]];
				interval[i][1]++;
				mark = true;
			}
		}
		interval[i][0]++;
		interval[i][1]--;
	}
}

void save_interval(Grid<int>& interval, char* dataset) {

	char path[300] = "all_gen_interval/";
  strcat(path, dataset);
  strcat(path, "_interval.txt");	
	FILE* file = fopen(path, "w");
	for (int i = 0; i < interval.height; ++i) {
	  fprintf(file, "%5d %5d\n", interval[i][0], interval[i][1]);
	}
	fclose(file);
}

#endif

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <cstring>

#include "../gridcode.h"

using namespace std;

Grid<int> cnt_small_large;
Grid<double> prob_small_large;

Grid<double> gen_small_given_large;

char picture_name[] = "teddy";
int small_disparity, large_disparity;

template <class type>
void save_matrix(Grid<type>& matrix, char* filename) {
  char path[300] = "testdata/";
	strcat(path, picture_name);
	strcat(path, filename);
	FILE *file = fopen(path, "w");
	for (int i = 0; i < matrix.height; ++i) {
	  for (int j = 0; j < matrix.width; ++j) {
		  fprintf(file, "%.5lf ", matrix[i][j]);
		}
		fprintf(file, "\n");
	}
	fclose(file);
}

template <class type>
void compute_avg_prob(Grid<type>& matrix, double*& avg_prob,
		bool is_small_base = false) {

	if (is_small_base) {
	  for (int i = 0; i < matrix.height; ++i) {
			for (int j = 0; j < matrix.width; ++j) {
		    avg_prob[i - j / 2 + matrix.height] += matrix[i][j];	
			}
		}
		double tmp = 0.0;
    for (int i = 0; i < 2 * matrix.height - 1; ++i) {
			avg_prob[i] /= matrix.width;
			tmp += avg_prob[i];
		  printf("%.5lf ", avg_prob[i]);
		}
		// TODO(xuejiaobai): why tmp != 1 ?
		printf("\n%.5lf\n", tmp);
	} else {
    for (int j = 0; j < matrix.width; ++j) {
		  for (int i = 0; i < matrix.height; ++i) {
			  avg_prob[j - i * 2 + matrix.width] += matrix[i][j];
			}
		}
	  for (int i = 0; i < 2 * matrix.width - 1; ++i)
		  avg_prob[i] /= matrix.height;
	}
}

int main(int args, char** argv) {
	// ./SgL_Gen.out MST__cnt_1_0.txt teddy 60
  freopen(argv[1], "r", stdin);
	strcpy(picture_name, argv[2]);
	large_disparity = atoi(argv[3]);
	small_disparity = large_disparity / 2;

	cnt_small_large.reset(small_disparity + 1, large_disparity + 1);
	prob_small_large.reset(small_disparity + 1, large_disparity + 1);
	gen_small_given_large.reset(small_disparity + 1, large_disparity + 1);

	for (int i = 0; i <= small_disparity; ++i) {
	  for (int j = 0; j <= large_disparity; ++j) {
		  cin >> cnt_small_large[i][j];
		}
	}
	
  // compute probability of small given large 
	for (int j = 0; j <= large_disparity; ++j) {
		int cnt_line = 0;
	  for (int i = 0; i <= small_disparity; ++i) {
		  cnt_line += cnt_small_large[i][j];
		}
		double prob = 0.0;
		for (int i = 0; i <= small_disparity; ++i) {
		  if (cnt_line == 0) {
					prob_small_large[i][j] = 0.0; //1.0 / small_disparity;
			} else {
			  prob_small_large[i][j] = cnt_small_large[i][j] * 1.0 / cnt_line;
		    prob += prob_small_large[i][j];
			}
		}
		// cout << prob << endl;
	}
  
	// save prob_small_large.
  save_matrix(prob_small_large, "__prob_small_given_large_10.txt");

	double* avg_prob_large_base = new double [2 * large_disparity + 1];
	double* avg_prob_small_base = new double [2 * small_disparity + 1];

	for (int i = 0; i <= 2 * large_disparity; ++i)
		avg_prob_large_base[i] = 0.0;
	for (int i = 0; i <= 2 * small_disparity; ++i)
		avg_prob_small_base[i] = 0.0;
  // compute average probablity.
	compute_avg_prob(prob_small_large, avg_prob_large_base, false);
	compute_avg_prob(prob_small_large, avg_prob_small_base, true);
  // construct small_givn_large matrix.
  for (int j = 0; j <= large_disparity; ++j) {
	  for (int i = 0; i <= small_disparity; ++i) {
		  gen_small_given_large[i][j] =
				  avg_prob_small_base[i - j / 2 + small_disparity];
		}
	}
  save_matrix(gen_small_given_large,
			        "__small_base_small_given_large_matrix_10.txt");
  
	for (int i = 0; i <= small_disparity; ++i) {
	  for (int j = 0; j <= large_disparity; ++j) {
		  gen_small_given_large[i][j] =
				  avg_prob_large_base[j - i * 2 + large_disparity];
		}
	}

  save_matrix(gen_small_given_large,
			        "__large_base_small_given_large_matrix_10.txt");	

	delete [] avg_prob_large_base;
  delete [] avg_prob_small_base;
	return 0;
}

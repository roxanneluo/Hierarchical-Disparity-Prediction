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

inline int dcmp(double a, double b) {
  if (a - b > 1e-8) return 1;
	if (a - b < -1e-8) return -1;
	return 0;
}

void gen_support_prob (Grid<unsigned char>& support,
		                   Array1<double>& support_prob,
											 int max_disparity) {
  support_prob.zero();
	int W = support.width, H = support.height;
	int d;
	for (int i = 0; i < H; ++i) {
	  for (int j = 0; j < W; ++j) {
		  d = support[i][j];
			if (d != 0) {
				if (d < max_disparity)
					*support_prob[d] += 1.0;
			}
		}
	}
	// normalize
	support_prob.normalize();
	// add noise
	// normalize

}

void gen_initial_prob(Grid<unsigned char>& initial_d,
		                  Array1<double>& initial_prob,
											int max_disparity) {
  initial_prob.zero();
	int W = initial_d.width, H = initial_d.height;
	int d;
	for (int i = 0; i < H; ++i) {
	  for (int j = 0; j < W; ++j) {
		  if (d < max_disparity)
				*initial_prob[d] += 1.0;
		}
	}
	// normalize
  initial_prob.normalize();
}

void read_prob_matrix (Grid<double>& prob_matrix, char* path) {
  prob_matrix.zero();
	FILE* file = fopen(path, "r");
	vector<double> prob;
	double tmp;
	while (fscanf(file, "%lf", &tmp) == 1) {
	  prob.push_back(tmp);
	}
	// prob_matrix.reset(prob.size(), prob.size() * 2 - 1);
	for (int j = 0; j < prob_matrix.width; ++j) {
		for (int i = 1; j / 2 - i >= 0; ++i) {
		  prob_matrix[j / 2 - i][j] = prob[prob.size() / 2 - i];
		}
	  for (int i = 0; j / 2 + i < prob_matrix.height; ++i) {
		  prob_matrix[j / 2 + i][j] = prob[prob.size() / 2 + i];
		}
	}
	fclose(file);
}

void gen_prob_matrix (Array1<double>& initial_prob,
		                  Grid<double>& prob_matrix,
											Array1<double>& support_prob) {
  int W = prob_matrix.width, H = prob_matrix.height;
  for (int i = 0 ; i < H; ++i) {
	  for (int j = 0; j < W; ++j) {
		  prob_matrix[i][j] /= *initial_prob[i];
		  prob_matrix[i][j] *= *support_prob[j];
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
		  interval[i][0] = -1;
			interval[i][1] = -1;
			err++;
		}	
	}
	return err;
}

#endif

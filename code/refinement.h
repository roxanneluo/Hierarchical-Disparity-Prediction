#ifndef REFINEMENT_CODE_H_OCT_2013
#define REFINEMENT_CODE_H_OCT_2013

#include "gridcode.h"
#include "mylib.h"

template <class type>
void find_stable_pixels(Grid<type> &disparity_left, Grid<type> &disparity_right,
						Grid<type>& occlusion_left, Grid<type>& occlusion_right) {

	occlusion_left.reset(disparity_left.height, disparity_left.width);
    occlusion_right.reset(disparity_right.height, disparity_right.width);
	occlusion_left.zero();
	occlusion_right.zero();
	for (int i = 0; i < disparity_left.height; ++i) {
		for (int j = 0; j < disparity_left.width; ++j) {
			type d = disparity_left[i][j];
			if (j - d >= 0) {
				if (d == 0 || abs(disparity_right[i][j - d] - d) >= 1) {
					occlusion_left[i][j] = 255;
			    }
			} else {
				occlusion_left[i][j] = 255;
			}
			d = disparity_right[i][j];
			if (j + d < disparity_right.width) {
				if (d == 0 || abs(disparity_left[i][j + d] - d) >= 1) {
					occlusion_right[i][j] = 255;
				}
			} else {
				occlusion_right[i][j] = 255;
			}
		}
	}
}

template <class type>
void update_matching_cost(Array3<double>& cost_left, Array3<double>& cost_right,
						  Grid<type>& disparity_left, Grid<type>& disparity_right,
						  Grid<type>& occlusion_left, Grid<type>& occlusion_right) {
	int arr = cost_left.array;
	int h = cost_left.height;
	int w = cost_left.width;
	cost_left.zero();
	cost_right.zero();
	for (int i = 0; i < h; ++i) {
		for (int j = 0; j < w; ++j) {
			if (occlusion_left[i][j] == 0) {
				for (int d = 0; d < arr; ++d) {
					cost_left[d][i][j] = abs (d - disparity_left[i][j]);
				}
			}
			if (occlusion_right[i][j] == 0) {
				for (int d = 0; d < arr; ++d) {
					cost_right[d][i][j] = abs (d - disparity_right[i][j]);
				}
			}
		}
	}
}


#endif
#ifndef ARITHMETIC_CODE_H_OCT_2013
#define ARITHMETIC_CODE_H_OCT_2013

#include "gridcode.h"
#include "mylib.h"

#define MAX_MATCHING_COST 2.5f

template <class type>
void compute_gradient (Grid<float> & gradient, Array3<type> & rgb) {
	gradient.reset(rgb.height, rgb.width);
	float co[3] = { 0.299, 0.587, 0.114 };
	float plus = 0.5, minus = 0.5, gray;
	for (int x = 0; x < rgb.height; ++x) {
		for (int j = 0; j < 3; ++j)  {
			plus += co[j] * rgb[j][x][1]; // * rgb[j][x][1] not rgb[j][x][0]
			minus += co[j] * rgb[j][x][0]; // * rgb[j][x][0] not rgb[j][x][1]
		}
		gray = plus;
		gradient[x][0] = 127.5 + plus - minus;
		for (int i = 1; i <= rgb.width - 2; ++i) {
			plus = 0.5;
			for (int j = 0; j < 3; ++j)
				plus += co[j] * rgb[j][x][i + 1];
			gradient[x][i] = 0.5 * (plus - minus) + 127.5;
			minus = gray; gray = plus;
		}
		gradient[x][rgb.width - 1] = plus - minus + 127.5;
	}
}

void copy_left_cost_to_right (Array3<double>& cost_left, Array3<double>& cost_right) {
  for (int x = 0; x < cost_right.height; ++x) // this part is merged , by the doc
    for (int y = 0; y < cost_right.width; ++y)
       for (int d = 0; d < cost_right.array; ++d) {
          if (y + d < cost_left.width)
            cost_right[d][x][y] = cost_left[d][x][y+d];
          else
            cost_right[d][x][y] = cost_right[d-1][x][y]; // ERROR: Not d - d but d - 1;
       }
}

// Compute first cost only for the highest layer.
template <class type>
void compute_first_cost (Array3<double> &cost_left, Array3<double> &cost_right,
    Array3<type> & rgb_left, Array3<type> &rgb_right,
    Grid<float> &graient_left, Grid<float>& gradient_right,
    int max_disparity) {
	int H, W;
	cost_left.reset(max_disparity, H=rgb_left.height, W=rgb_left.width);
	cost_right.reset(max_disparity, rgb_left.height, rgb_left.width); // assume left and right rgb image has the exact same size;
	double max_gradient_color_difference= 2.0;
	double max_color_difference = 7.0;
	double weight_on_color = 0.11;
	for (int d = 0; d < max_disparity; ++d)
		for (int x = 0; x < H; ++x)
			for (int y = 0; y < W; ++y) {
				double cost = 0;
				for (int c = 0; c < 3; ++c)
					cost += mylib::ABS(rgb_left[c][x][y] - rgb_right[c][x][mylib::max(y - d,0)]);
				cost = mylib::min(cost / 3.0, max_color_difference); // here is weired
				double cost_gradient = mylib::min((double) mylib::ABS(graient_left[x][y] - gradient_right[x][mylib::max(0, y - d)]), max_gradient_color_difference);
				cost_left[d][x][y] = weight_on_color * cost + (1 - weight_on_color) * cost_gradient;
			}
	copy_left_cost_to_right(cost_left, cost_right);

}

// Compute first cost for other layers by using the initial disparity from previous layer.
template <class type>
void compute_first_cost (Array3<double>& cost_left, Array3<double>& cost_right,
    Array3<type>& rgb_left, Array3<type>& rgb_right,
    Grid<float>& graient_left, Grid<float>& gradient_right,
		Grid<type>& pre_disparity,
    int max_disparity) {
	int H, W;
	cost_left.reset(max_disparity, H=rgb_left.height, W=rgb_left.width);
	cost_right.reset(max_disparity, rgb_left.height, rgb_left.width); // assume left and right rgb image has the exact same size;
	double max_gradient_color_difference= 2.0;
	double max_color_difference = 10.0;
	double weight_on_color = 0.11;
	for (int d = 0; d < max_disparity; ++d)
		for (int x = 0; x < H; ++x)
			for (int y = 0; y < W; ++y) {
				int prex = mylib::min(pre_disparity.height - 1, x / 2);
				int prey = mylib::min(pre_disparity.width - 1, y / 2);
				if (pre_disparity[prex][prey] * 2 - 2 <= d &&
						pre_disparity[prex][prey] * 2 + 2 >= d) {
					double cost = 0;
				  for (int c = 0; c < 3; ++c)
					  cost += mylib::ABS(
								rgb_left[c][x][y] - rgb_right[c][x][mylib::max(y - d,0)]);

					cost = mylib::min(cost / 3.0, max_color_difference); // here is weired
				  double cost_gradient = mylib::min((double) mylib::ABS(graient_left[x][y] - gradient_right[x][mylib::max(0, y - d)]), max_gradient_color_difference);
				  cost_left[d][x][y] = weight_on_color * cost + (1 - weight_on_color) * cost_gradient;

				} else {
				  cost_left[d][x][y] = MAX_MATCHING_COST;
				}
			}
	copy_left_cost_to_right(cost_left, cost_right);
}

template <class type>
void compute_disparity (Array3<double> & cost, Grid<type> &disparity) {
	// something weired in the doc. left right not symetric
	disparity.reset(cost.height, cost.width);
	for (int i = 0; i < cost.height; ++i)
		for (int j = 0; j < cost.width; ++j) {
			double best = cost[0][i][j];
            int best_dis = 0;
			for (int d = 1; d < cost.array; ++d)
				if (cost[d][i][j] < best) {
					best = cost[d][i][j];
                    best_dis = d;
                }
			disparity[i][j] = best_dis;
		}
	// ctmf ignored
}

// ctmf should be here

#endif

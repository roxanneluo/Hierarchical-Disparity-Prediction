#ifndef SCALE_H_OCT_2013
#define SCALE_H_OCT_2013

#include "gridcode.h"

// This is a box filter.

template <class type> 
type average (const type& a, const type& b) {
	type ans = type((a + b) / 2);
	return ans;
}

template <class type>
Array3<type> scale_image(Array3<type>& large_image) {
	int h = large_image.height;
	int w = large_image.width;
	int arr = large_image.array;

	int hs = h / 2;
	int ws = w / 2;

  type average_i1, average_i2;


	Array3<type> scaled(hs, ws, arr);
  for (int i = 0; i < hs; i += 2) {
		int i2 = i * 2;
		for (int j = 0; j < ws; j += 2) {
		  int j2 = j * 2;
			average_i1 = (j2 + 1 < w) ?
				  average(large_image[i2][j2], large_image[i2][j2 + 1]) :
					large_image[i2][j2];
			if (i2 + 1 < h) {
		    average_i2 = (j2 + 1 < w) ?
			      average(large_image[i2 + 1][j2], large_image[i2 + 1][j2 + 1]):
						large_image[i2 + 1][j2];
	    } else {
			  average_i2 = average_i1;
			}
			scaled[i][j] = average(average_i1, average_i2);
		}
	}
  return scaled;
}


#endif

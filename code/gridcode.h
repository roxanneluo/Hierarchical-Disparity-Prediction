#ifndef GRID_CODE_H_OCT_2013
#define GRID_CODE_H_OCT_2013
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <cstdio>
// this code is meant for memory management

#define PADDING 10


template <class type>
class Grid {
public :
	type ** grid;
	int height, width;
	Grid<type> () {height = width = -1; grid = NULL;}
	Grid<type> (const Grid<type>& g) {grid = NULL;}
	Grid<type> (int H, int W) {
		reset(H, W);
	}
/*
	void reset(int H, int W) {
		height = H; width = W;
		grid = (type **) malloc(H * sizeof(type *));
		for (int i = 0; i < H; ++i)
			grid[i] = (type *) malloc(W * sizeof(type));
	}
*/

	~Grid<type> () {
		freegrid();
	}
	void reset(int H, int W) {
		height = H;
		width = W;
		type *a;
		a = (type*) malloc(sizeof(type) * (height * width + PADDING));
        assert(a != NULL);
        /*if(a == NULL) {
			printf("reset() fail, Memory is too huge, fail.\n");
			getchar();
			exit(0);
		} */
		grid = (type**) malloc(sizeof(type*) * height);
		for(int i = 0; i < height; i++)
			grid[i] = &a[i*width];
	}

	void freegrid() {
		if (grid != NULL) {
			free(grid[0]);
			free(grid);
			grid = NULL;
    }
	}
	Grid<type> (Grid<type> & other) {
		reset(other.height, other.width);
		copy(other);
	}
	void copy(Grid<type> &other) {
		if (other.height != height || other.width != width)
			reset(other.height, other.width);
		for (int i = 0; i < height; ++i)
			memcpy(grid[i], other.grid[i], width * sizeof(type));
	}
	void zero() {
		//reset(height, width);
		for (int i = 0; i < height; ++i) {
			for (int j = 0; j < width; ++j) {
				grid[i][j] = 0;
			}
		}
	}

	void white() {
        for (int i = 0; i < height; ++i)
            for (int j = 0; j < width; ++j)
                grid[i][j] = 255;
	}
	type * operator[] (int index) { return grid[index]; }
};

template <class type>
class Array3 {
public :
	int height, width, array;
	Grid<type> ** mat;
	Array3<type> () {
		array = height = width = -1;
		mat = NULL;
	}
	Array3<type> (const Array3<type>& a) {mat = NULL;}
	Array3<type> (int arr, int H, int W) {
		reset(arr, H, W);
	}

	~Array3<type> () {
		if (mat != NULL) {
			for (int i = 0; i < array; ++i)
				delete mat[i];
			free(mat);
		 	mat = NULL;
    }
	}
	void reset(int arr, int H, int W) {
		height = H, width = W, array = arr;
		mat = (Grid<type> **) malloc(array * sizeof(Grid<type> *));
		for (int i = 0; i < array; ++i)
			mat[i] = new Grid<type>(H, W);
	}

	void freearray3() {
	  if (mat != NULL) {
		  mat[0][0].freegrid();
			free(mat[0]);
			free(mat);
			mat = NULL;
		}
	}
	Array3<type>(Array3<type> & other) {
		reset(other.height, other.width, other.array);
		copy(other);
	}
	void copy(Array3<type> & other) {
		if (other.height != height || other.width != width || other.array != array)
			reset(other.array, other.height, other.width);
		for (int i = 0; i < array; ++i)
			mat[i]->copy(other[i]);
	}
	void zero() {
		//reset(array, height, width);
		for (int a = 0; a < array; ++a) {
			mat[a]->zero();
		}
	}
	
	void rgb_2_gray(Grid<type>& gray) {
		if (array != 3) {
			return;
		}
	  float co[3] = {0.299, 0.587, 0.114};
		for (int i = 0; i < height; ++i) {
		  for (int j = 0; j < width; ++j) {
				gray[i][j] = (type)(co[0] * (*mat[0][i][j]) +
						                co[1] * (*mat[1][i][j]) +
														co[2] * (*mat[2][i][j]) +
														0.5);
			}
		}
	}

	Grid<type> & operator[] (int index) { return *mat[index]; }
};

#endif

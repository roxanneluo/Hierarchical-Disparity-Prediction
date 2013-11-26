#ifndef GRID_CODE_H_OCT_2013
#define GRID_CODE_H_OCT_2013
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <cstdio>
#include <random>
#include <ctime>
// this code is meant for memory management

#define PADDING 10
using namespace std;

template <class type>
class Array1 {
public:
  type* array;
	int length;
	Array1<type> () {length = -1; array = NULL;}
	Array1<type> (const Array1<type>& a) {array = NULL;}
	Array1<type> (int L) {
	  reset(L);
	}
  ~Array1<type> () {
	  freearray();
	}
  void reset(int L) {
	  length = L;
		array = (type*) malloc(sizeof(type) * (length + PADDING));
    assert(array != NULL);
	}
	void freearray() {
    if (array != NULL) {
		  delete [] array;
			array = NULL;
		}	
	}
	type * operator[] (int index) { return &array[index]; }
  void zero() {
	  for (int i = 0; i < length; ++i) {
		  array[i] = 0;
		}
	}
	void white() {
	  for (int i = 0; i < length; ++i) {
		  array[i] = 255;
		}
	}
	void copy (Array1<type>& other) {
		if (other.length != length)
			reset(other.length);
			memcpy(array, other.array, length * sizeof(type));
	}
	void normalize () {
    double err = 1e-10;
    double sum = 0;
    for (int i = 0; i < length; ++i)
			sum += array[i];
    if (sum > err)
        for (int i = 0; i < length; ++i) array[i] /= sum;
	}
};

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
/*
	void addNoise(double sigma) {
	    unsigned seed = time(NULL);
        default_random_engine generator (seed);
        normal_distribution<double> distribution (0.0,sigma);
        double noise = 0;
        for (int i = 0; i < height; ++i) {
            if (grid[i][1] == 0.0) {
                noise = distribution(generator);
                grid[i][1] += (noise > 0)? noise:0;
            }
        }
        normalize(1);
	}
*/

	/*1: normalize along the col; 2: normalize along the column*/
	void normalize(int direct) {
	    double err = 1e-10;
        double sum = 0;
	    if (direct == 1) {
            for (int j = 0; j < width; ++j) {
                sum = 0;
                for (int i = 0; i < height; ++i) sum += grid[i][j];
                if (sum > err)
                    for (int i = 0; i < height; ++i) grid[i][j] /= sum;
            }
	    } else {
            assert(direct == 2);
            for (int i = 0; i < height; ++i) {
                sum = 0;
                for (int j = 0; j < width; ++j) sum += grid[i][j];
                if (sum > err)
                    for (int j = 0; j < width; ++j) grid[i][j] /= sum;
            }
	    }
	}
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

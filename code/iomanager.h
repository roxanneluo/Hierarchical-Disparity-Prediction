#ifndef INPUT_OUTPUT_MANAGER_H_OCT_2013
#define INPUT_OUTPUT_MANAGER_H_OCT_2013

#include <cstdio>
#include <cstring>
#include <cctype>
#include "gridcode.h"

const int BUF_LEN = 3030303;
char buff[BUF_LEN];

template <class type>
void load_image(const char * filename, Array3<type> & rgb) {
	// return if it is successfull
	FILE * f = fopen(filename, "rb");
	fread(buff, 1, BUF_LEN, f);
	fclose(f);
	int i = 0;
	while (buff[i]) {
		if (buff[i] == 'P') {++i; break; }
		++i;
	}
	//int type = buff[i] - '0';
    ++i; // i shall assume it is exactly P6 type
	int value[3], count = 0;
	while (buff[i] && count < 3) {
		if (buff[i] == '#') {
			while (buff[i] != '\n') ++i;
			++i;
		} else
		if (!isdigit(buff[i])) ++i;
		else {
			int k = 0;
			while (isdigit(buff[i]))
				k = k * 10 + buff[i++] - '0';
			value[count++] = k;
		}
	}
	// now take down the last end-of-line
	while (buff[i] != '\n') ++i; // after the while  buff[i] == \n

	rgb.reset(3, value[1], value[0]);
    for (int x = 0; x < rgb.height; ++x)
        for (int y = 0; y < rgb.width; ++y)
            for (int color = 0; color < 3; ++color)
				rgb[color][x][y] = (unsigned char) (buff[++i]);
}

template <class type>
void load_image_gray(const char * filename, Grid<type> & gray) {
	// return if it is successfull
	FILE * f = fopen(filename, "rb");
	fread(buff, 1, BUF_LEN, f);
	fclose(f);
	int i = 0;
	while (buff[i]) {
		if (buff[i] == 'P') {++i; break; }
		++i;
	}
	//int type = buff[i] - '0';
    ++i; // i shall assume it is exactly P5 type
	int value[3], count = 0;
	while (buff[i] && count < 3) {
		if (buff[i] == '#') {
			while (buff[i] != '\n') ++i;
			++i;
		} else
		if (!isdigit(buff[i])) ++i;
		else {
			int k = 0;
			while (isdigit(buff[i]))
				k = k * 10 + buff[i++] - '0';
			value[count++] = k;
		}
	}
	// now take down the last end-of-line
	while (buff[i] != '\n') ++i;

	gray.reset(value[1], value[0]);
    for (int x = 0; x < gray.height; ++x)
        for (int y = 0; y < gray.width; ++y)
            gray[x][y] = (unsigned char) (buff[++i]);
}

template <class type>
void save_image(const char * filename, Grid<type> &gray, int scale = 1) {
	FILE * f = fopen(filename, "wb");
	fprintf(f, "P5\n");
	fprintf(f, "#Produced by my silly program\n");
	fprintf(f, "%d %d\n255\n", gray.width, gray.height);
    int cnt = 0;
    for (int i = 0; i < gray.height; ++i)
		for (int j = 0; j < gray.width; ++j)
            buff[cnt++] = gray[i][j] * scale;
    fwrite(buff, 1, cnt, f);
	fclose(f);
}

template <class type>
void save_image_rgb(const char * filename, Array3<type> & rgb, int scale = 1) {
    // remember th filename should be  .ppm
	FILE * f = fopen(filename, "wb");
	fprintf(f, "P6\n");
	fprintf(f, "#Produced by my silly program\n");
	fprintf(f, "%d %d\n255\n", rgb.width, rgb.height);
    int cnt = 0;
    for (int i = 0; i < rgb.height; ++i)
		for (int j = 0; j < rgb.width; ++j)
            for (int c = 0; c < 3; c++)
                buff[cnt++] = rgb[c][i][j] * scale;
    fwrite(buff, 1, cnt, f);
	fclose(f);
}

/*

bad code...

void save_image(const char * filename, Grid<int> &gray) {
	FILE * f = fopen(filename, "wb");
	fprintf(f, "P5\n");
	fprintf(f, "#Produced by my silly program\n");
	fprintf(f, "%d %d\n255\n", gray.width, gray.height);
	int big = -1, small = 10101010;
	for (int i = 0; i < gray.height; ++i)
		for (int j = 0; j < gray.width; ++j) {
			if (gray[i][j] > big) big = gray[i][j];
			if (gray[i][j] < small) small = gray[i][j];
		}
	// big - small = 255; ij - small = 255 * x;
    int cnt = 0;
	double ratio = 255.0 / (big - small);
	for (int i = 0; i < gray.height; ++i)
		for (int j = 0; j < gray.width; ++j)
			//buff[cnt++] = gray[i][j];
            gray[i][j] =(gray[i][j] - small) * ratio;
    big = 0;
    for (int i = 0; i < gray.height; ++i)
		for (int j = 0; j < gray.width; ++j)
            if (gray[i][j] > big) big = gray[i][j];
    for (int i = 0; i < gray.height; ++i)
		for (int j = 0; j < gray.width; ++j)
            buff[cnt++] = big - gray[i][j];
    fwrite(buff, 1, cnt, f);
	fclose(f);
}
*/
#endif

#ifndef Settings_for_the_project_MAR_2014
#define Settings_for_the_project_MAR_2014



#define MAX_HEIGHT 555
#define MAX_WIDTH 555

typedef unsigned char BytArray[MAX_HEIGHT][MAX_WIDTH];
typedef int IntArray [MAX_HEIGHT][MAX_WIDTH];
typedef double FloArray [MAX_HEIGHT][MAX_WIDTH];
typedef bool BooArray [MAX_HEIGHT][MAX_WIDTH];

typedef int Picture [MAX_HEIGHT][MAX_WIDTH][3]; // RGB

const int NODES = MAX_HEIGHT * MAX_WIDTH;
int max_disparity = 16;
int scale = 16;

char file_name[4][300] =
{"left.ppm", "right.ppm", "silly_left.pgm", "silly_right.pgm"};

#endif

#ifndef Settings_for_the_project_MAR_2014
#define Settings_for_the_project_MAR_2014



#define MAX_HEIGHT 1234
#define MAX_WIDTH 1234

typedef int IntArray [MAX_HEIGHT][MAX_WIDTH];
typedef double FloArray [MAX_HEIGHT][MAX_WIDTH];
typedef int Picture [MAX_HEIGHT][MAX_WIDTH][3]; // RGB

const int NODES = MAX_HEIGHT * MAX_WIDTH;
int max_disparity = 16;
int scale = 16;

#endif

#ifndef Settings_for_the_project_MAR_2014
#define Settings_for_the_project_MAR_2014



#define MAX_HEIGHT 1150
#define MAX_WIDTH 1400

// types

typedef unsigned char BytArray[MAX_HEIGHT][MAX_WIDTH];
// typedef unsigned short BytArray[MAX_HEIGHT][MAX_WIDTH];
typedef int IntArray [MAX_HEIGHT][MAX_WIDTH];
typedef float FloArray [MAX_HEIGHT][MAX_WIDTH];
typedef bool BooArray [MAX_HEIGHT][MAX_WIDTH];

typedef int Picture [MAX_HEIGHT][MAX_WIDTH][3]; // RGB
typedef float FloPicture [MAX_HEIGHT][MAX_WIDTH][3]; // Lab color space
typedef int (*PicturePtr) [MAX_WIDTH][3];
typedef float (*FloPicturePtr) [MAX_WIDTH][3];
typedef float (*FloArrayPtr) [MAX_WIDTH];
// if this becase 8-bit, be careful. 

// enumerate dataset
// the last enum number = #element in the enumeration
enum Dataset { HALF_SIZE, FULL_SIZE, DATASET_NUM };
enum DatasetRatio {
  RATIO_1,    // ratio = 1
  RATIO_0_5,  // ratio = 0.5
  RATIO_0_3,  // ratio = 0.3
  DATASET_RATIO_NUM
};

// constants

const int NODES = MAX_HEIGHT * MAX_WIDTH + 3;
const int LEVELS = 4;

// global variables

int max_disparity = 16;
int scale = 16;

char file_name[4][300] =
{"left.ppm", "right.ppm", "default_left.pgm", "default_right.pgm"};

double tot_threshold = 0.9;

Dataset dataset = FULL_SIZE;
bool use_lab = true;

#endif

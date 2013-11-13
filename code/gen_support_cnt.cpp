#include "gridcode.h"
#include "forests1.h"
#include "arithmetics.h"
#include "iomanager.h"
#include "timekeeper.h"
#include "refinement.h"
//#include "imageresult.h"

#include "filter.h"
#include "filename.h"
#include "support_match.h"

using namespace std;

int *disp;
int max_disparity = 16;
int scale = 16;

char file_names[5][300] =
    {"tsukuba", "left.ppm", "right.ppm",
			"left_supportmap.pgm", "right_supportmap.pgm"};
char path[300] = "all_support_cnt/";

void get_disp(Grid<unsigned char>& support) {
    for (int i = 0; i <= max_disparity; ++i)
        disp[i] = 0;
    int W = support.width, H = support.height;
    int d;
    for (int i = 0; i < H; ++i)
        for (int j = 0; j < W; ++j) {
            d = support[i][j];
            if (d != 0) {
                if (d > max_disparity) printf("error: %d\n", d);
                else ++disp[d];
            } //else printf("zero\n");
        }
}

void save_disp(char* path, char* dataset) {
    //"support_disp_cnt.txt"
    strcat(path, "Prob_Gen_");
	  strcat(path, dataset);
	  strcat(path, "__support_disp_cnt.txt");	
	  FILE *file = fopen(path, "w");
	  for (int i = 0; i < max_disparity+1; ++i)
        fprintf(file, "%d\n", disp[i]);
    fclose(file);
}

int main(int args, char ** argv) {
	if (args > 3) {
		  strcpy(file_names[0], argv[1]); // dataset
			strcpy(file_names[1], argv[2]); // leftimage
			strcpy(file_names[2], argv[3]); // rightimage
		}

		if (args > 5) {
		  strcpy(file_names[3], argv[4]); // left_support image
			strcpy(file_names[4], argv[5]); // right_support image
		}

		if (args > 6) {
		  max_disparity = atoi(argv[6]);
			scale = 256 / max_disparity;
		}
		
		if (args > 7) {
		  scale = atoi(argv[7]);
		}

		Array3<unsigned char> rgb_left, rgb_right;
		try {
	    load_image(file_names[1], rgb_left);
	    load_image(file_names[2], rgb_right);	
		} catch (...) {
		  puts("Errpr loading file.");
			return 0;
		}


		// Compute support maps.
	  SupportMatch sm (rgb_left, rgb_right, rgb_left.width, rgb_left.height);
		sm.FindSupportMatch();
    save_image(file_names[3], sm.support_left_map, scale);
    save_image(file_names[4], sm.support_right_map, scale);

    disp = new int [max_disparity+1];
		// get disp from the left support map.
    get_disp(sm.support_left_map);
    save_disp(path, file_names[0]);

    delete [] disp;
  return 0;
}

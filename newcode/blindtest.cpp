#include "gridcode.h"
#include "forests.h"
#include "arithmetics.h"
#include "iomanager.h"
#include "timekeeper.h"
#include "refinement.h"
#include "imageresult.h"

#include "filter.h"
#include "blind.h"


char file_name[4][300] =
{"displeft.pgm", "dispright.pgm", "blind_left.pgm", "blind_right.pgm"};

Grid<unsigned char> disparity_left, disparity_right; // int -> unsigned char
Grid<unsigned char> blind_left, blind_right;

int scale = 1;

int main(int args, char ** argv) {
    /* if (args > 2) {
       strcpy(file_name[0], argv[1]);
       strcpy(file_name[1], argv[2]);
     }
     if (args > 3) {
       max_disparity = atoi(argv[3]);
       scale = 256 / max_disparity;
     }
     if (args > 4) {
       scale = atoi(argv[4]);
     }
     if (args > 6) {file:///home/roxanne/repos/stereo-cake/code/gridcode.h

       strcpy(file_name[2], argv[5]);
       strcpy(file_name[3], argv[6]);
     }
    */
    try {
        load_image_gray(file_name[0], disparity_left);
        load_image_gray(file_name[1], disparity_right);

    } catch (...) {
        puts("Error loading file");
        return 0;
    }

    draw_blind_area(blind_left, disparity_left, 1);
    draw_blind_area(blind_right, disparity_right, 0);

    save_image(file_name[2], blind_left, scale);
    save_image(file_name[3], blind_right, scale);

    return 0;
}

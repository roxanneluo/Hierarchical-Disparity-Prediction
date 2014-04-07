#ifndef the_small_functions_hehe
#define the_small_functions_hehe

namespace misc {

inline int max(int a, int b) { if (a > b) return a; else return b; }
inline double max(double a, double b) { if (a > b) return a; else return b; }

inline int min(int a, int b) { if (a < b) return a; else return b; }
inline double min(double a, double b) { if (a < b) return a; else return b; }

inline int abs(int a) { if (a < 0) return -a; else return a; }
inline double abs(double a) { if (a < 0) return -a; else return a; }

}

// process args
    if (args > 2) {
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
    if (args > 6) {
        strcpy(file_name[2], argv[5]);
        strcpy(file_name[3], argv[6]);
    }
    if (args > 7) {
        strcpy(file_name[4], argv[7]);
    }

    try {
        load_image(file_name[0], rgb_left[0]);
        load_image(file_name[1], rgb_right[0]);
    } catch (...) {
        puts("Error loading file");
        return 0;
    }
#endif

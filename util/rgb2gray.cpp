
#include "settings.h"
#include "iomanager.h"
#include "misc.cpp"

Picture ref;
BytArray out;
int h, w;

int main(int args, char ** argv) {
    // argv[1] is the ref image
    // argv[2] is the output filename
    if (args < 3) {
        puts("Failed. Need more specifications.");
        return 0;
    }
    load_image(argv[1], ref, h, w);
    rgb2gray(out, ref, h, w);
    save_image(argv[2], out, h, w);
    return 0;
}

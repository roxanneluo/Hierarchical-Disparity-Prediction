#include "gridcode.h"
#include "iomanager.h"
#include "filename.h"

char file_name[300] = "toy";
int num = 2, step = 5;
int W = 100, H = 100;
Array3<unsigned char> pic;
int main(int args, char ** argv) {
	if (args > 1)
		strcpy(file_name, argv[1]);
	if (args > 2)
		num = atoi(argv[2]);
	if (args > 3)
		step = atoi(argv[3]);
	if (args > 4)
		W = atoi(argv[4]);
	if (args > 5)
		H = atoi(argv[5]);
	// printf("%d,%d\n", W,H);
	pic.reset(3, H,W);
	for (int d = 0; d < 3; ++d) 
		for (int i = 0; i < H; ++i) {
			for (int n = 0; n < num; ++n) {
				int c = mylib::max(0, 255-n*step);
				for (int j = n*W/num; j<(n+1)*W/num; ++j)
					pic[d][i][j] = c;
			}
		}
	char save_name[100];
	save_image_rgb(get_file_name(save_name, file_name, num,step,W,".ppm"), pic);
	return 0;
}
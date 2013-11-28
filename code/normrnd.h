#ifndef __NORMRND__
#define __NORMRND__
#include <cmath>
#include "gridcode.h"
const double PI = 3.141592653589793;
void genGMM(Array1<double> &gmm, int K, double const *mu, double const *sigma, double const *pi) {
    int maxGMMDisp = 60;
    gmm.reset(2*maxGMMDisp+1);
    gmm.zero();
    double x;
    for (int k = 0; k < K; ++k) {
        for (int i = 0; i < 2*maxGMMDisp+1; ++i) {
            x = (i-maxGMMDisp-mu[k])/sigma[k];
            x *= x/2;
            gmm[i] += pi[k]/(sqrt(2*PI)*sigma[k])*exp(-x);
        }
    }
}
/*void genGMM(Array1<double> &gmm) {
    double mu = {}
}*/
/**
--------------------0-----------------------
    mu = [0.138324438;7.344032560];
    Pi = [0.775804385;0.224195615];
    sigma = [0.459105146;7.896771369];
    K=2;

      ------------1---------------
%     mu = [0.780102537;2.868775577];
%     Pi = [0.868242368;0.131757632];
%     sigma = [0.544941882;5.536860102];
%     K=2;
**/
void genGMM(Array1<double> &gmm, int rem) {
    assert(0<=rem && rem < 2);
    double mu[2][2] = {{0.138324438,7.344032560}, {0.780102537,2.868775577}};
    double pi[2][2] = {{0.775804385,0.224195615}, {0.868242368,0.131757632}};
    double sigma[2][2] = {{0.459105146,7.896771369}, {0.544941882,5.536860102}};
    int K[2] = {2,2};
    genGMM(gmm,K[rem],mu[rem], sigma[rem], pi[rem]);
}


#endif // __NORMRND__

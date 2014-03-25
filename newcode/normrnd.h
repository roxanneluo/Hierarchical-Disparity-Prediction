#ifndef __NORMRND__
#define __NORMRND__
#include <cmath>
#include "gridcode.h"
const double PI = 3.14159265358979323846264;
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
/**
%     mu = [0.2071;-2.2668];
%     Pi = [0.7719;0.2281];
%     sigma^2 = [0.4155;92.1834];

mu	-1.086022171	0.184231375
sigma	5.680955165	0.543865616
Pi	0.225260819	0.774739181
**/
/**
--------------data with new filter-----------

--10--
mu	0.990587103	0.187743358	-4.048706689
sigma	2.912734757	0.606497154	11.087372705
Pi	0.111202599	0.727640398	0.161157002

--21--
-1.086022171	0.184231375
sigma	5.680955165	0.543865616
Pi	0.225260819	0.774739181
**/
void genGMM_layer(Array1<double> &gmm, int large_layer) {
    if (large_layer == 0) {
        /*double mu[2] = {0.2071,-2.2668};
        double pi[2] = {0.7719,0.2281};
        double sigma[2] = {0.6446, 9.6012};
        int k = 2;
        genGMM(gmm,k,mu,sigma,pi);*/
        const int k = 3;
        double mu[k] = {0.990587103,0.187743358,-4.048706689};
        double pi[k] = {0.111202599,0.727640398,0.161157002};
        double sigma[k] = {2.912734757, 0.606497154,11.087372705};
        genGMM(gmm,k,mu,sigma,pi);
    } else {
        double mu[2] = {-1.086022171,0.184231375};
        double pi[2] = {0.225260819,0.774739181};
        double sigma[2] = {5.680955165, 0.543865616};
        int k = 2;
        genGMM(gmm,k,mu,sigma,pi);
    }
}
/**
%     mu = [0.2071;-2.2668];
%     Pi = [0.7719;0.2281];
%     sigma^2 = [0.4155;92.1834];
**/
void genGMM(Array1<double> &gmm) {
    double mu[2] = {0.2071,-2.2668};
    double pi[2] = {0.7719,0.2281};
    double sigma[2] = {0.6446, 9.6012};
    int k = 2;
    genGMM(gmm,k,mu,sigma,pi);
}
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
void genGMMRem(Array1<double> &gmm, int rem) {
    assert(0<=rem && rem < 2);
    double mu[2][2] = {{0.138324438,7.344032560}, {0.780102537,2.868775577}};
    double pi[2][2] = {{0.775804385,0.224195615}, {0.868242368,0.131757632}};
    double sigma[2][2] = {{0.459105146,7.896771369}, {0.544941882,5.536860102}};
    int K[2] = {2,2};
    genGMM(gmm,K[rem],mu[rem], sigma[rem], pi[rem]);
}


#endif // __NORMRND__

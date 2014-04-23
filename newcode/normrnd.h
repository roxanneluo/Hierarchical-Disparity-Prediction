#ifndef __NORMRND__
#define __NORMRND__
#include <cmath>
namespace gmm  {

double gmm[MAX_WIDTH];

const double PI = 3.14159265358979323846264;
void genGMM(int K, double const *mu, double const *sigma, double const *pi, int maxGMMDisp) {
    int length = 2*maxGMMDisp+1;
    for (int i = 0; i <= length; ++i) gmm[i] = 0;
    for (int k = 0; k < K; ++k) 
        for (int i = 0; i < length; ++i) {
            double x = (i-maxGMMDisp-mu[k])/sigma[k];
            x *= x/2;
            gmm[i] += pi[k]/(sqrt(2*PI)*sigma[k])*exp(-x);
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
void genGMM_layer(int large_layer, int small_max_disparity, double datasetRatio =  0.5) {
    if (datasetRatio == 1)
        if (large_layer == 0) {
            /*double mu[2] = {0.2071,-2.2668};
            double pi[2] = {0.7719,0.2281};
            double sigma[2] = {0.6446, 9.6012};
            int k = 2;
            genGMM(gmm,k,mu,sigma,pi);*/

            // gmm when gnd vs. MST 
            // const int k = 3;
            // double mu[k] = {0.990587103,0.187743358,-4.048706689};
            // double pi[k] = {0.111202599,0.727640398,0.161157002};
            // double sigma[k] = {2.912734757, 0.606497154,11.087372705};

            /*=========gmm gnd vs. MSF2=========*/
            const int k = 3;
            double mu[k] = {0.187744, -4.049134, 0.990802};
            double pi[k] = {0.727650, 0.161147, 0.111203};
            double sigma[k] = {0.606504, 11.087519, 2.913190};
            genGMM(k,mu,sigma,pi,small_max_disparity);
        } else {
            /*==========gmm when k=2, gnd vs. MST and gnd vs. MSF2 share the same GMM(k=2)===============*/
            // double mu[2] = {-1.086022171,0.184231375};
            // double pi[2] = {0.225260819,0.774739181};
            // double sigma[2] = {5.680955165, 0.543865616};
            // int k = 2;
            /*==============gmm when k=3, gnd vs. MSF2=========*/
            const int k = 3;
            double mu[k] = {-2.133305, 1.058262, 0.177772};
            double pi[k] = {0.157178, 0.094915, 0.747907};
            double sigma[k] = {6.284264, 2.368275, 0.525036};
            genGMM(k,mu,sigma,pi,small_max_disparity);
        }
    else if (large_layer == 0) { //datasetRatio=0.5
            /*=========gmm gnd vs. MSF2==========*/
            const int k = 3;
            double mu[k] = {1.049875, 0.176738, -5.205266};
            double pi[k] = {0.123876, 0.705882, 0.170241};
            double sigma[k] = {2.700242, 0.594091, 10.932382};
            genGMM(k,mu,sigma,pi,small_max_disparity);
        } else {
            /*==========gmm when k=2, gnd vs. MSF2===============*/
            double mu[2] = {-0.838225, 0.174887};
            double pi[2] = {0.218403, 0.781597};
            double sigma[2] = {5.652754, 0.543740};
            int k = 2;
            genGMM(k,mu,sigma,pi,small_max_disparity);
        }
}
/**
%     mu = [0.2071;-2.2668];
%     Pi = [0.7719;0.2281];
%     sigma^2 = [0.4155;92.1834];
**/
void genGMM(int small_max_disparity) {
    double mu[2] = {0.2071,-2.2668};
    double pi[2] = {0.7719,0.2281};
    double sigma[2] = {0.6446, 9.6012};
    int k = 2;
    genGMM(k,mu,sigma,pi,small_max_disparity);
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
void genGMMRem(int rem, int small_max_disparity) {
    assert(0<=rem && rem < 2);
    double mu[2][2] = {{0.138324438,7.344032560}, {0.780102537,2.868775577}};
    double pi[2][2] = {{0.775804385,0.224195615}, {0.868242368,0.131757632}};
    double sigma[2][2] = {{0.459105146,7.896771369}, {0.544941882,5.536860102}};
    int K[2] = {2,2};
    genGMM(K[rem],mu[rem], sigma[rem], pi[rem],small_max_disparity);
}

}

#endif // __NORMRND__

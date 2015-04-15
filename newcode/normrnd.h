#ifndef __NORMRND__
#define __NORMRND__
#include <cmath>
#include <vector>
#include <cassert>
#include "settings.h"
namespace gmm  {

double gmm[MAX_WIDTH];

const double PI = 3.14159265358979323846264;
const int MAX_GMM_NUM = 4;

class GMM {
 public:
  int K;
  double mu[MAX_GMM_NUM];
  double sigma[MAX_GMM_NUM];
  double pi[MAX_GMM_NUM];

  GMM() { K = -1; }
  GMM(const std::vector<double> mm,
      const std::vector<double> ss, 
      const std::vector<double> pp) {
    K = mm.size();
    assert(K == ss.size() && K == pp.size());
    for (int i = 0; i < K; ++i) {
      mu[i] = mm[i];
      sigma[i] = ss[i];
      pi[i] = pp[i];
    }
  }

  GMM(const GMM & gmm2) {
    K = gmm2.K;
    for (int i = 0; i < K; ++i) {
      mu[i] = gmm2.mu[i];
      sigma[i] = gmm2.sigma[i];
      pi[i] = gmm2.pi[i];
    }
  }

  GMM & operator = (const GMM & gmm2) {
    K = gmm2.K;
    for (int i = 0; i < K; ++i) {
      mu[i] = gmm2.mu[i];
      sigma[i] = gmm2.sigma[i];
      pi[i] = gmm2.pi[i];
    }
  }

  void genGMM(int maxGMMDisp) const {
    assert(K > 0);
    int length = 2*maxGMMDisp+1;
    for (int i = 0; i <= length; ++i) gmm[i] = 0;
    for (int k = 0; k < K; ++k) 
        for (int i = 0; i < length; ++i) {
            double x = (i-maxGMMDisp-mu[k])/sigma[k];
            x *= x/2;
            gmm[i] += pi[k]/(sqrt(2*PI)*sigma[k])*exp(-x);
        }
  }
};

void genGMM_layer(int large_layer, int small_max_disparity, Dataset dataset, DatasetRatio dataset_ratio = DATASET_RATIO_NUM) {
  static bool table_set = false;
  static GMM GMM_table[DATASET_NUM][DATASET_RATIO_NUM][LEVELS];
  if (!table_set) {
    GMM_table[HALF_SIZE][RATIO_1][0] = 
      GMM({0.187744, -4.049134, 0.990802},
          {0.606504, 11.087519, 2.913190},
          {0.727650, 0.161147, 0.111203});
    GMM_table[HALF_SIZE][RATIO_1][1] = 
      GMM({-2.133305, 1.058262, 0.177772},
          {6.284264, 2.368275, 0.525036},
          {0.157178, 0.094915, 0.747907});
    GMM_table[HALF_SIZE][RATIO_0_5][0] = 
      GMM({1.049875, 0.176738, -5.205266},
          {2.700242, 0.594091, 10.932382},
          {0.123876, 0.705882, 0.170241});
    GMM_table[HALF_SIZE][RATIO_0_5][1] = 
      GMM({-0.838225, 0.174887},
          {5.652754, 0.543740},
          {0.218403, 0.781597});
    GMM_table[FULL_SIZE][RATIO_1][0] = 
      GMM({0.355655, 51.055870, 4.216459, -15.923241},
          {0.656810, 14.776888, 18.815209, 42.960163},
          {0.676908, 0.076295, 0.075217, 0.171580});
    GMM_table[FULL_SIZE][RATIO_1][1] = 
      GMM({9.154573, 0.398220, 23.247545, -6.074539},
          {0.811038,0.644315,6.910081, 21.185447},
          {0.018760, 0.715488, 0.072520, 0.193232});

    table_set = true;
  }

  // set default dataset ratio for each dataset
  if (dataset_ratio >= DATASET_RATIO_NUM) {
    switch(dataset) {
      case HALF_SIZE: dataset_ratio = RATIO_0_5; break;
      case FULL_SIZE: dataset_ratio = RATIO_1; break;
      default: 
        printf("dataset %d not defined\n", dataset);
        assert(false);
    }
  }
  
  const GMM &gmm = GMM_table[dataset][dataset_ratio][large_layer];
  gmm.genGMM(small_max_disparity);
}


}

#endif // __NORMRND__

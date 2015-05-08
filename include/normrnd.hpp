#ifndef __NORMRND__
#define __NORMRND__
#include <cmath>
#include <vector>
#include <cassert>
#include "settings.hpp"
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

    /* recalculated using mst
    */
    GMM_table[FULL_SIZE][RATIO_1][0] = 
      GMM({32.502699, -34.328304, 18.462280, 0.370448},
          {25.730198, 21.352337, 3.652555, 0.632237},
          {0.146335, 0.134981, 0.015143, 0.703540});
    GMM_table[FULL_SIZE][RATIO_1][1] = 
      GMM({9.983536, -17.626809, 18.114333, 0.467670},
          {1.114562, 10.611028, 10.518979, 0.627145},
          {0.030189, 0.119504, 0.121258, 0.729049});
    GMM_table[FULL_SIZE][RATIO_1][2] = 
      GMM({-8.839617, 4.968906, 9.112209, 0.516049},
          {4.935000, 0.535987, 4.373850, 0.651702},
          {0.064693, 0.046168, 0.111436, 0.777703});
    GMM_table[FULL_SIZE][RATIO_0_5][0] = 
      GMM({-38.335031, 0.383928, 22.106222, 0.383928},
          {19.879390, 0.629609, 26.036771, 0.629609},
          {0.131236, 0.310989, 0.133314, 0.424460});
    GMM_table[FULL_SIZE][RATIO_0_5][1] = 
      GMM({0.452982, -21.247403, 11.133451, 10.101522},
          {0.617757, 9.580585, 14.423702, 0.604170},
          {0.747561, 0.100479, 0.126690, 0.025270});
    GMM_table[FULL_SIZE][RATIO_0_5][2] = 
      GMM({4.580204, 10.864041, -8.720563, 0.476397},
          {1.036329, 4.002217, 4.461282, 0.637942},
          {0.076437, 0.048733, 0.060785, 0.814045});

    /*
     * calculated using mst
    GMM_table[FULL_SIZE][RATIO_1][0] = 
      GMM({0.355655, 51.055870, 4.216459, -15.923241},
          {0.656810, 14.776888, 18.815209, 42.960163},
          {0.676908, 0.076295, 0.075217, 0.171580});
    GMM_table[FULL_SIZE][RATIO_1][1] = 
      GMM({9.154573, 0.398220, 23.247545, -6.074539},
          {0.811038,0.644315,6.910081, 21.185447},
          {0.018760, 0.715488, 0.072520, 0.193232});
    GMM_table[FULL_SIZE][RATIO_1][2] = 
      GMM({4.985110, -8.788525, 9.457388, 0.500377},
          {0.516290, 5.040455, 3.894737, 0.652619},
          {0.038318, 0.081977, 0.117143, 0.762562});
    */

    /* calculated using rt
    GMM_table[FULL_SIZE][RATIO_1][0] = 
      GMM({0.380256, -29.272048, 34.022945, 15.961184},
          {0.621078, 22.373777, 24.856599, 5.825053},
          {0.665735, 0.171789, 0.145202, 0.017274});
    GMM_table[FULL_SIZE][RATIO_1][1] = 
      GMM({9.965415, 0.459348, 18.979573, -15.10675},
          {3.279199, 0.619913, 10.909861, 10.975498},
          {0.033010, 0.689250, 0.118168, 0.159572});
     */

    /* calculated using st
    GMM_table[FULL_SIZE][RATIO_0_5][0] = 
      GMM({0.308309, -29.624998, 38.218056, 0.308309},
          {0.616869, 21.225896, 22.763226, 0.616869},
          {0.444128, 0.148890, 0.114578, 0.292403});
    GMM_table[FULL_SIZE][RATIO_0_5][1] = 
      GMM({4.123910, -19.252273, 19.717231, 0.464530},
          {14.303646, 8.235853, 10.483732, 0.620743},
          {0.071457, 0.094924, 0.073870, 0.759749});
    GMM_table[FULL_SIZE][RATIO_0_5][2] = 
      GMM({9.350401, 0.523719, 0.523719, -8.706245},
          {4.623216, 0.662745, 0.662745, 3.939637},
          {0.102574, 0.398342, 0.428751, 0.070334});
    */
    /* calculated using st
    GMM_table[FULL_SIZE][RATIO_1][0] = 
      GMM({0.365762, 18.580291 ,32.659689, -33.034826},
          {0.626881, 3.957473, 25.605166, 21.475359},
          {0.695877, 0.015757, 0.146405, 0.141961});
    GMM_table[FULL_SIZE][RATIO_1][1] = 
      GMM({18.076729, 0.463865, 10.144601, -16.893652},
          {10.459019, 0.629478, 1.102050, 10.798065},
          {0.123338, 0.722031, 0.028355, 0.126276});
    GMM_table[FULL_SIZE][RATIO_1][2] = 
      GMM({0.515384, 10.297553, 4.734522, -8.790798},
          {0.653191, 3.614636, 0.983156, 4.997334},
          {0.772544, 0.092971, 0.059689, 0.074796});
     */

    table_set = true;
  }

  // set default dataset ratio for each dataset
  if (dataset_ratio >= DATASET_RATIO_NUM) {
    switch(dataset) {
      case HALF_SIZE: dataset_ratio = RATIO_0_5; break;
      case FULL_SIZE: dataset_ratio = RATIO_0_5; break;
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

#include <cstdlib>
#include <cmath>
#include <cassert>
#include <ctime>
#include "dp_big_object.hpp"

namespace Forest{
const int COLOR_CHANNEL = 3;
const int COLOR_BITS = 8;
const int MAX_NUM_COLORS = 1 << (COLOR_CHANNEL * COLOR_BITS);

class Color {
 public:
  int rgb[COLOR_CHANNEL];
  int hash;
  const int mask = 0x00FF;

  Color(int hh) {
    hash = hh;
    for (int i = 0; i < COLOR_CHANNEL; ++i) {
      rgb[i] = (hash>> i * COLOR_BITS) & mask;
    }
  }
  Color() {hash = -1;}

  Color & operator = (const Color &color) {
    hash = color.hash;
    for (int c = 0; c < COLOR_CHANNEL; ++c)
      rgb[c] = color.rgb[c];
    return *this;
  }
  
  int operator [] (int c) {
    return rgb[c];
  }
  
  static double distance(int h1, int h2) {
    Color c1(h1), c2(h2);
    int dis = 0;
    for (int c = 0; c < COLOR_CHANNEL; ++c) 
      dis += abs(c1.rgb[c] - c2.rgb[c]);
    return dis / double (COLOR_CHANNEL);
  }
};

class ColorMap {
  int num_color_;
  Color * map_;
  int* hash_map_;
  static const int MIN_COLOR_DISTANCE = 3;

  bool exist(int hash) {
    for (int i = 0; i < num_color_; ++i) {
      if (Color::distance(hash, hash_map_[i]) <= MIN_COLOR_DISTANCE)
      //if (abs(hash-hash_map_[i]) <= MIN_COLOR_DISTANCE)
        return true;
    }    
    return false;
  }

  void add(int hash) {
    hash_map_[num_color_] = hash; 
    Color c(hash);
    map_[num_color_++] = c;
  }

 public:
  inline const Color& getColor(int i) {
    assert(0 <= i && i < num_color_);
    return map_[i];
  }

  ColorMap(int n) {
    num_color_ = 0;
    map_ = new Color[n];
    hash_map_ = new int[n];

    srand(time(NULL));
    int hash;
    for (int i = 0; i < n; ++i) {
      do {
        hash = rand() % MAX_NUM_COLORS;
      } while (exist(hash));
      add(hash);
    }
    assert(n == num_color_);
  }

  ~ColorMap() { delete [] map_; delete [] hash_map_;}
};  
void forestSegment(const DPBigObject & dp, Picture rgb) {
  int numOT = dp.numOfTree();
  printf("numOfTree = %d\n", numOT);
  OneTree oneTree = dp.treeStartEnd(); 
  ColorMap map(numOT);
  for (int i = 1; i<=numOT; ++i) {
    int low = oneTree[i][0], high = oneTree[i][1];
    Color color = map.getColor(i-1);
    for (int u = low; u <= high; ++u) {
      int x,y;
      dp.treeNodeLocation(u, x, y);
      for (int c = 0; c < COLOR_CHANNEL; ++c)
        rgb[x][y][c] = color[c];
    }
  }
}

// avg in terms of n
double get_avg_treeIntv(const DPBigObject &dp) {
  int treeIntv_sum = 0;
  int numOT = dp.numOfTree();
  OneTree oneTree = dp.treeStartEnd(); 
  for (int i = 1; i <= numOT; ++i) {
      Interval treeInterval = dp.getTreeInterval(oneTree[i][0]);
      int low = oneTree[i][0], high = oneTree[i][1];
      assert(treeInterval.length() >= 0);
      treeIntv_sum += treeInterval.length() * (high-low+1);
  }
  return treeIntv_sum / double(dp.n);
}

// avg in terms of numoftree
double get_mean_treeIntv(const DPBigObject &dp) {
  int treeIntv_sum = 0;
  int numOT = dp.numOfTree();
  OneTree oneTree = dp.treeStartEnd(); 
  for (int i = 1; i <= numOT; ++i) {
      Interval treeInterval = dp.getTreeInterval(oneTree[i][0]);
      assert(treeInterval.length() >= 0);
      treeIntv_sum += treeInterval.length();
  }
  return treeIntv_sum / double(numOT);
}


} //  end of namespace

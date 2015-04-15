#ifndef ThePredictionPart_jqw889rnasbflakhdaslj
#define ThePredictionPart_jqw889rnasbflakhdaslj

#include "normrnd.h"
#include "support_match.h"


namespace dpf {

inline int dcmp(double a, double b) {
    if (a - b > 1e-9) return 1;
    if (a - b < -1e-9) return -1;
    return 0;
}

void normalize(double * array, int len) {
    double err = 1e-10;
    double sum = 0;
    for (int i = 0; i < len; ++i)
        sum += array[i];
    if (sum > err)
        for (int i = 0; i < len; ++i) array[i] /= sum;
}


int interval[MAX_WIDTH][2]; 
double support_prob[MAX_WIDTH];//, initial_prob[MAX_WIDTH];

double prob_matrix[MAX_WIDTH][MAX_WIDTH]; 
int height, width; // size for prob_matrix

double gmm[MAX_WIDTH];

double* getSupportProb(Picture left_rgb, Picture right_rgb, 
    int h, int w, int length) {
    supportmatch::makeSupportMatch(left_rgb, right_rgb, h, w);
    supportmatch::findSupportMatch();
    
    for (int i = 0; i <= length; ++i) support_prob[i] = 0;
    for (int i = 0; i < h; ++i) 
        for (int j = 0; j < w; ++j) {
            int d = supportmatch::support_left_map[i][j];
            if (d != 0 && d <= length) // != 0??? why
                support_prob[d] += 1;
            d = supportmatch::support_right_map[i][j];
            if (d != 0 && d <= length)
                support_prob[d] += 1;
        }
    //normalize(support_prob, length);
    return support_prob;
}

void getProbMatrix(int layer, int h, int w, Dataset dataset) {
    height = h; width = w;
    gmm::genGMM_layer(layer,height, dataset);
	for (int j = 0; j <= width; ++j) {
        for (int i = 1; j / 2 - i >= 0; ++i) 
            prob_matrix[j / 2 - i][j] = gmm::gmm[height - i];
        for (int i = 0; j / 2 + i < height; ++i) 
            prob_matrix[j / 2 + i][j] = gmm::gmm[height+ i];
    }
    for (int i = 0; i <= height; ++i) {
        for (int j = 0; j <= width; ++j)
            prob_matrix[i][j] *= support_prob[j];
     //   normalize(prob_matrix[i], width);
    }
    /*
    for (int i = 0; i < height; ++i) {
        double err = 1e-10;
        double sum = 0;
        for (int j = 0; j < width; ++j) 
            sum += prob_matrix[i][j];
        if (sum > err)
            for (int j = 0; j < width; ++j) 
                prob_matrix[i][j] /= sum;
    }
    */
}


void getInterval(double threshold, double tot_threshold = 0.9) {
    double highest;
    int highest_index;
    for (int i = 0; i <= height; ++i) {
        highest = prob_matrix[i][0];
        highest_index = 0;
        for (int j = 1; j <= width; ++j) {
            if (highest < prob_matrix[i][j]) {
                highest = prob_matrix[i][j];
                highest_index = j;
            }
        }
        if (dcmp(highest, 0.0) == 0) {
            interval[i][0] = 0;
            interval[i][1] = -1;
            continue;
        }
        int ll, rr;
        ll = highest_index-1;
        rr = highest_index+1;
        double total = prob_matrix[i][highest_index];
        bool mark = true;
        while((ll >= 0 || rr <= width) 
            && (mark || total <= tot_threshold)) {
            mark = false;
            if (dcmp(prob_matrix[i][ll] / (prob_matrix[i][ll] + total), threshold) == 1) {
                total += prob_matrix[i][ll];
                --ll; mark = true;
            }
            if (dcmp(prob_matrix[i][rr] / (prob_matrix[i][rr] + total), threshold) == 1) {
                total += prob_matrix[i][rr];
                ++rr; mark = true;
            }
            if (!mark) {
                if (rr > width 
                    || ll >= 0 && prob_matrix[i][ll] > prob_matrix[i][rr]) {
                    total += prob_matrix[i][ll--];
                } else {
                    total += prob_matrix[i][rr++]; 
                }
            }
        }
        interval[i][0] = ll + 1;
        interval[i][1] = rr - 1;
    }
}

/*
void getInterval(double threshold, double tot_threshold = 0.9) {
    double highest;
    int highest_index;
    for (int i = 0; i <= height; ++i) {
        highest = prob_matrix[i][0];
        highest_index = 0;
        for (int j = 1; j <= width; ++j) {
            if (highest < prob_matrix[i][j]) {
                highest = prob_matrix[i][j];
                highest_index = j;
            }
        }
        if (dcmp(highest, 0.0) == 0) {
            interval[i][0] = 0;
            interval[i][1] = -1;
            continue;
        }
        int ll, rr;
        ll = highest_index-1;
        rr = highest_index+1;
        double total = prob_matrix[i][highest_index];
        bool mark = true;
        while(mark && (ll >= 0 || rr <= width)) {
            mark = false;
            if (dcmp(prob_matrix[i][ll] / (prob_matrix[i][ll] + total), threshold) == 1) {
                total += prob_matrix[i][ll];
                --ll; mark = true;
            }
            if (dcmp(prob_matrix[i][rr] / (prob_matrix[i][rr] + total), threshold) == 1) {
                total += prob_matrix[i][rr];
                ++rr; mark = true;
            }
        }
        interval[i][0] = ll + 1;
        interval[i][1] = rr - 1;
    }
}
*/
} // end of namespace dpf

#endif 

#ifndef UTILS_LIBRARY_H_OCT_2013
#define UTILS_LIBRARY_H_OCT_2013

#include <cstdlib>

namespace mylib {

int randint(int a, int b) {
	int k = b - a + 1;
	if (k < 1) return a;
	int p = rand() * rand() + rand();
	p = ((p % k) + k) % k;
	return k + a;
}

double fabs(double x) { if (x < 0) x=-x; return x; }

template <class type>
type min(type a, type b) { if (a < b) return a; return b; }

template <class type>
type max(type a, type b) { if (a > b) return a; return b; }

int min3abs(int a, int b, int c) { 
    if (a < 0) a = -a; if (b < 0) b = -b; if (c < 0) c = -c;
    if (b < a) a = b; if (c < a) a = c; return a; 
}

int max3abs(int a, int b, int c) {
	if (a < 0) a = -a; if (b < 0) b = -b; if (c < 0) c = -c;
	if (b > a) a = b; if (c > a) a = c; return a;
}

}

#endif

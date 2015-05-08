// NOTE: This descripter is a sparse approximation to the 50-dimensional
// descriptor described in the paper. It produces similar results, but
// is faster to compute.

#ifndef DESCRIPTOR_H
#define DESCRIPTOR_H

#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cmath>

#include "gridcode.h"
#include "descriptor.h"
#include "sobel_filter.h"
#include <emmintrin.h>

// Define fixed-width datatypes for Visual Studio projects
#ifndef _MSC_VER
#include <stdint.h>
#else
typedef __int8            int8_t;
typedef __int16           int16_t;
typedef __int32           int32_t;
typedef __int64           int64_t;
typedef unsigned __int8   uint8_t;
typedef unsigned __int16  uint16_t;
typedef unsigned __int32  uint32_t;
typedef unsigned __int64  uint64_t;
#endif

class Descriptor {

public:

    // constructor creates filters
    Descriptor(uint8_t* I, int32_t width, int32_t height, int32_t bpl,
               bool half_resolution) {
        I_desc = (uint8_t*)_mm_malloc(16 * width * height * sizeof(uint8_t),16);
        uint8_t* I_du = (uint8_t*)_mm_malloc(bpl * height * sizeof(uint8_t),16);
        uint8_t* I_dv = (uint8_t*)_mm_malloc(bpl * height * sizeof(uint8_t),16);
        sobel_filter::sobel3x3(I,I_du,I_dv,bpl,height);
        createDescriptor(I_du, I_dv, width, height, bpl, half_resolution);
        _mm_free(I_du);
        _mm_free(I_dv);
    }
    // deconstructor releases memory
    ~Descriptor() {
        _mm_free(I_desc);
    }

    // descriptors accessible from outside
    uint8_t* I_desc;
    // Grid<unsigned char> I_desc;
private:

    // build descriptor I_desc from I_du and I_dv
    void createDescriptor(uint8_t* I_du, uint8_t* I_dv,
                          int32_t width, int32_t height, int32_t bpl, bool half_resolution) {

        uint8_t *I_desc_curr;
        uint32_t addr_v0, addr_v1, addr_v2, addr_v3, addr_v4;

        // do not compute every second line
        if (half_resolution) {

            // create filter strip
            for (int32_t v=4; v<height-3; v+=2) {

                addr_v2 = v*bpl;
                addr_v0 = addr_v2-2*bpl;
                addr_v1 = addr_v2-1*bpl;
                addr_v3 = addr_v2+1*bpl;
                addr_v4 = addr_v2+2*bpl;

                for (int32_t u=3; u<width-3; u++) {
                    I_desc_curr = I_desc+(v*width+u)*16;
                    *(I_desc_curr++) = *(I_du+addr_v0+u+0);
                    *(I_desc_curr++) = *(I_du+addr_v1+u-2);
                    *(I_desc_curr++) = *(I_du+addr_v1+u+0);
                    *(I_desc_curr++) = *(I_du+addr_v1+u+2);
                    *(I_desc_curr++) = *(I_du+addr_v2+u-1);
                    *(I_desc_curr++) = *(I_du+addr_v2+u+0);
                    *(I_desc_curr++) = *(I_du+addr_v2+u+0);
                    *(I_desc_curr++) = *(I_du+addr_v2+u+1);
                    *(I_desc_curr++) = *(I_du+addr_v3+u-2);
                    *(I_desc_curr++) = *(I_du+addr_v3+u+0);
                    *(I_desc_curr++) = *(I_du+addr_v3+u+2);
                    *(I_desc_curr++) = *(I_du+addr_v4+u+0);
                    *(I_desc_curr++) = *(I_dv+addr_v1+u+0);
                    *(I_desc_curr++) = *(I_dv+addr_v2+u-1);
                    *(I_desc_curr++) = *(I_dv+addr_v2+u+1);
                    *(I_desc_curr++) = *(I_dv+addr_v3+u+0);
                }
            }

            // compute full descriptor images
        } else {

            // create filter strip
            for (int32_t v=3; v<height-3; v++) {

                addr_v2 = v*bpl;
                addr_v0 = addr_v2-2*bpl;
                addr_v1 = addr_v2-1*bpl;
                addr_v3 = addr_v2+1*bpl;
                addr_v4 = addr_v2+2*bpl;

                for (int32_t u=3; u<width-3; u++) {
                    I_desc_curr = I_desc+(v*width+u)*16;
                    *(I_desc_curr++) = *(I_du+addr_v0+u+0);
                    *(I_desc_curr++) = *(I_du+addr_v1+u-2);
                    *(I_desc_curr++) = *(I_du+addr_v1+u+0);
                    *(I_desc_curr++) = *(I_du+addr_v1+u+2);
                    *(I_desc_curr++) = *(I_du+addr_v2+u-1);
                    *(I_desc_curr++) = *(I_du+addr_v2+u+0);
                    *(I_desc_curr++) = *(I_du+addr_v2+u+0);
                    *(I_desc_curr++) = *(I_du+addr_v2+u+1);
                    *(I_desc_curr++) = *(I_du+addr_v3+u-2);
                    *(I_desc_curr++) = *(I_du+addr_v3+u+0);
                    *(I_desc_curr++) = *(I_du+addr_v3+u+2);
                    *(I_desc_curr++) = *(I_du+addr_v4+u+0);
                    *(I_desc_curr++) = *(I_dv+addr_v1+u+0);
                    *(I_desc_curr++) = *(I_dv+addr_v2+u-1);
                    *(I_desc_curr++) = *(I_dv+addr_v2+u+1);
                    *(I_desc_curr++) = *(I_dv+addr_v3+u+0);
                }
            }
        }
    }
};

#endif

#!/bin/bash
cd ../code
g++ silly_MSF2.cpp -O2 -std=c++0x -msse3 -o ../newcode/old.out

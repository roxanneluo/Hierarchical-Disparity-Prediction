#######
# print Image size and MDE for exel use. It is of the following format
# Testcase   Image Size    MDE    Testcase(MDE)
# Aloe      =H*W          =H*W*D  Aloe(H*W*D)
# ...
# 
# argument: dataset_folder (e.g., ../testdata/fullsize)
#######

import sys
import os
import subprocess

dataset_folder = sys.argv[1]
testcases = subprocess.check_output(['ls', dataset_folder]).split()

print 'Testcase\tImage Size\tMDE\tTestcase(MDE)'
for testcase in testcases:
  folder = dataset_folder + '/' + testcase + '/'
  with open(folder + 'spec.txt', 'r') as f:
    max_disp = f.readline().split()[0]
  with open(folder + 'displeft.pgm', 'r') as f:
    f.readline()
    line = f.readline()
    while line.startswith('#') :
      line = f.readline()
    line = line.split()
    #line = f.readline().split()
    width = line[0]
    height = line[1]
    print '%s\t=%s*%s/1e6\t=%s*%s*%s/1e6\t%s(%.2f)' % (testcase, height, width, height,width,
        max_disp, testcase, int(height)*int(width)*int(max_disp)/1e6)

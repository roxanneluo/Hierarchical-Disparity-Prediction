######
# convert the table of interval ratio generated
# from drawGeneratedMultiLayerForPaper.m 
# (then copied to $filename$) of the format
# Testcase(MaxDisparity)
# itv_ratio_1,0 0             0   
# 0             itv_ratio_2,1 0
# 0             0             itv_ratio_3,2
# ...
# $block_size$ = total layer number - 1. 
# So in this case, $block_size$ = 3. 
# to the format 
# Testcase(MaxDisp) itv_ratio_1,0 itv_ratio_2,1 itv_ratio_3,2
# suitable for exel table
######

import sys
import subprocess

block_size = sys.argv[1]
filename = sys.argv[2]

with open(filename, 'r') as f: 
  line  = f.readline()[:-1]
  while line :
    for i in range(int(block_size)) :
      ratio = f.readline().split()[i]
      line += '\t' + ratio
    print line
    line = f.readline()[:-1]

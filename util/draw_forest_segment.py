####################################
# draw the segments obtained by HDPF
####################################

import sys
import os
import subprocess

algo = sys.argv[1]
dataset = sys.argv[2]

use_lab = '0'
dataset_table = {'halfsize':'0', 'fullsize':'1'}

dataset_folder = '../testdata/' + dataset + '/'
result_folder = '../results/forest_segment/'
subprocess.check_output(['mkdir', '-p', result_folder])
result_folder += dataset + '/'
subprocess.check_output(['mkdir', '-p', result_folder])
testcases = subprocess.check_output(['ls', dataset_folder]).split()

for testcase in testcases:
  print algo + '@' + dataset + '/' + testcase
  folder = dataset_folder + testcase + '/'
  output_filename = result_folder + 'tree_segment_' + testcase +'_'+algo+'_left.ppm'
  with open(folder + 'spec.txt','r') as f:
    max_disp = f.readline()[:-1]
    scale = f.readline()[:-1]
  results = subprocess.check_output(['../bin/util/forest_segment_' + algo + '.bin',
      folder + 'left.ppm', folder + 'right.ppm',
      max_disp, scale,
      output_filename,
      dataset_table[dataset], use_lab])
  print results

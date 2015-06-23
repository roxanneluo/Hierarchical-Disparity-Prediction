#########################################
# compute the R_l or R_tree(l), the ratio between the average length of
# tree_intervals to the length of layer l's original search range d_l+1
#########################################

import sys
import os
import subprocess
import numpy

################ ARGUMENTS ##############
algo = sys.argv[1]
dataset = sys.argv[2]
L = int(sys.argv[3])
latex_style = bool(sys.argv[4]) if len(sys.argv)>4 else False
################ ARGUMENTS ##############

use_lab = '0'
dataset_table = {'halfsize':'0', 'fullsize':'1'}

R_tree = [0 for x in range(0,L)]

dataset_folder = '../testdata/' + dataset + '/'
testcases = subprocess.check_output(['ls', dataset_folder]).split()

layer_indices = [str(i) for i in range(0,L)]
if latex_style:
  splitter = '& '
  end = '\\\\'
else :
  splitter = '\t'
  end = ''
print 'Testcase ' + splitter + splitter.join(['R_tree(%s)' % i for i in layer_indices]) + splitter + \
    splitter.join(["R'_tree(%s)" % i for i in layer_indices]) + splitter + \
    splitter.join(['numOT(%s)' % i  for i in layer_indices]) + end
for testcase in testcases:
  folder = dataset_folder + testcase + '/'
  with open(folder + 'spec.txt','r') as f:
    max_disp = f.readline()[:-1]
    scale = f.readline()
  results = subprocess.check_output(['../bin/util/tree_intv_' + algo + '.bin',
      folder + 'left.ppm', folder + 'right.ppm',
      max_disp, scale,
      'nothing',
      dataset_table[dataset], use_lab]).split('\n')[:-1]
  avg_ratios = []
  numOTs = []
  mean_ratios = []
  for lvl in range(L) :
    idx = L-1-lvl
    small_disp = float(int(max_disp)/int(2**lvl)+1) 
    avg_intv = float(results[idx*2].split(':')[1])
    second_line =results[idx*2+1].split(':')[1].split() 
    numOT = int(second_line[0])
    mean_intv = float(second_line[1])
    avg_ratios.append(avg_intv/small_disp)
    numOTs.append(numOT)
    mean_ratios.append(mean_intv / small_disp)
  for lvl in range(L) :
    R_tree[lvl] += avg_ratios[lvl]
  print testcase + splitter + splitter.join([str(x) for x in avg_ratios]) + \
      splitter + splitter.join([str(x) for x in mean_ratios]) + \
      splitter + splitter.join([str(x) for x in numOTs]) + end


#R_tree = [str(r/len(testcases)) for r in R_tree]
#header = 'Resolution'
#for lvl in range(L):
#  header += splitter + 'Layer '+ str(lvl)
#header += end
#print dataset + splitter + splitter.join(R_tree) + end


#########################################
# in original super_reports, the way to calculate the average was wrong
#########################################

import sys
import subprocess as sp
import numpy as np

filename = sys.argv[1]
dataset = sys.argv[2]

def compute_mean(table) :
  m = len(table)
  n = len(table[0])
  s = np.zeros((n))
  print s
  for row in table:
    for j in range(n):
      s[j] += row[j]
  return [ss/float(m) for ss in s]

algo_names = ['dp_mst', 'dp_st', 'dp_rt']
algo_num = len(algo_names)
lines = sp.check_output(['grep', '<td>' +  dataset,
  filename]).split('\n')[:-1]
table = np.empty((0, algo_num)) 
for line in lines:
  line_ratio = []
  cells = line.split('</td>')[1:-1]
  for cell in cells:
    line_ratio.append(float(cell.split('>')[1]))
  table = np.append(table, [line_ratio], axis=0)
mean =  np.mean(table, axis=0)

with open(filename, 'r') as f:
  all_lines = f.read()
table = all_lines.split('</table>\n')[-1]
with open(filename, 'w') as f:
  f.write('<!DOCTYPE html>\n<html>\n<body>\n<h1>Algorithm ' +\
      'Overview</h1>\n<table border="3">\n' + \
      '<tr> <th>Algorithm</th><th>Error Rate</th> </tr>\n')
  for ai in range(len(algo_names)) :
    f.write('<tr><td>' + algo_names[ai] + '</td>')
    f.write('<td>' + str(mean[ai]) + '</td>')
    f.write('</tr>\n')
  f.write('</table>\n')

  f.write('<h1>Dataset Overview</h1>\n<table border="3">\n' + \
      '<tr> <th>Algorithm</th> <th>halfsize</th> </tr>\n')
  for ai in range(len(algo_names)) :
    f.write('<tr><td>' + algo_names[ai] + '</td>')
    f.write('<td>' + str(mean[ai]) + '</td>')
    f.write('</tr>\n')
  f.write('</table>')
  f.write(table)
print filename + '\t done.'

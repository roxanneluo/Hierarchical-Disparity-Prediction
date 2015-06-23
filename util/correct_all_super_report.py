import sys
import subprocess as sp

folder = '../results/pixel_tree_intv_threshold_sensitivity/'
all_files =  sp.check_output(['ls', folder]).split('\n')
html_files = []
for f in all_files:
  if f.endswith('.html') and f.startswith('SuperReport'):
    html_files.append(f)
for f in html_files:
  if 'halfsize' in f:
    dataset = 'halfsize'
  else :
    dataset = 'fullsize'
  print sp.check_output(['python', 'correct_wrong_super_report.py',folder + f, dataset]) 

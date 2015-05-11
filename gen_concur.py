import subprocess as sp
import sys 
import datetime
import os

algo = sys.argv[1]
in_folder = sys.argv[2]
out_folder = sys.argv[3]
dataset = sys.argv[4]
use_lab = sys.argv[5] if len(sys.argv) > 5 else 0

dataset_table = {'halfsize':0, 'fullsize':1}

out_folder = out_folder + '/' + dataset
time_str = datetime.datetime.now().strftime('%Y-%m-%d-%H:%M:%S')
support_folder = out_folder + '/support_test_' + time_str + '/'
concur_folder = out_folder + '/concur_test_' + time_str + '/'
print sp.check_output(['mkdir', '-p', out_folder])
print sp.check_output(['mkdir', support_folder, concur_folder])

for testcase in os.listdir(in_folder) :
  folder = os.path.join(in_folder, testcase) + '/'
  if not os.path.isdir(folder) :
    continue

  with open(folder + '/spec.txt', 'r') as f:
    max_disp = f.readline().strip()
    scale = f.readline().strip()

  print '-------' + algo + ' ' + testcase + ' max_disp: '+ max_disp + \
      ' scale: ' + scale + '-------'
  print sp.check_output(['bin/gen_data/gen_concur_' + algo + '.bin',
    folder + 'left.ppm', folder + 'right.ppm',
    max_disp, scale,
    folder + 'displeft.pgm',
    str(dataset_table[dataset]),
    str(use_lab),
    testcase, algo,
    concur_folder, support_folder,
    ])



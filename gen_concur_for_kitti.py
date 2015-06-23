#########################################
# actually we use GMM trained from Middlebury in experiments over KITTI
#########################################

import subprocess as sp
import sys 
import datetime
import os

algo = sys.argv[1]
in_folder = sys.argv[2] + '/'
out_folder = sys.argv[3]
dataset = sys.argv[4]
use_lab = sys.argv[5] if len(sys.argv) > 5 else 0

dataset_table = {'halfsize':0, 'fullsize':1, 'kitti':2}

max_disp = '256'
scale = '1'
out_folder = out_folder + '/' + dataset
time_str = datetime.datetime.now().strftime('%Y-%m-%d-%H:%M:%S')
support_folder = out_folder + '/support_' + time_str + '/'
concur_folder = out_folder + '/concur_' + time_str + '/'
print sp.check_output(['mkdir', '-p', out_folder])
print sp.check_output(['mkdir', support_folder, concur_folder])

left_folder = 'colored_0/'
right_folder = 'colored_1/'
for testcase in os.listdir(in_folder + '/' + left_folder) :
  if not testcase.endswith('10.png'):
    continue

  print '-------' + algo + ' ' + testcase + ' max_disp: '+ max_disp + \
      ' scale: ' + scale + '-------'
  print sp.check_output(['bin/gen_data/gen_concur_' + algo + '.bin',
    in_folder + left_folder + testcase, in_folder + right_folder + testcase,
    max_disp, scale,
    in_folder + 'disp_noc/' + testcase,
    str(dataset_table[dataset]),
    str(use_lab),
    '0.064', '0.6',
    testcase, algo,
    concur_folder, support_folder,
    ])



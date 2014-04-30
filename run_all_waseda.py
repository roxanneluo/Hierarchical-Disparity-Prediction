import os
import sys
import subprocess

import httplib

ALGORITHMS = (
  'Original',
  'ST',
  'RandTree',
  'MSF_ALL_MST',
  'MSF_ALL_ST',
  'MSF2_TEST',
)
# run_all_waseda.py alg testdata
# alg: mst dpf-mst
alg = sys.argv[1] 

path = 'testdata/waseda/'
datasets = subprocess.check_output(['ls', path]).split()
left = ''
right = ''
left_result = ''
right_result = ''

def run_test_with_dataset(dataset, alg) :
    """ path should end with a / """
    what = subprocess.check_output([
        './bin/'+alg+'.out', 
        left, right, 
        '32', '8',
        left_result,
        right_result,
        dataset[-4:],
    ])
    print what

for dataset in datasets :
  pic_names = subprocess.check_output(['ls', path + dataset]).split()
  for pic in pic_names :
    if pic.endswith('.ppm') and pic.startswith('L') :
      # print pic[-7:]
      left = path + dataset + '/L' + pic[-7:]
      right = path + dataset + '/R' + pic[-7:]
      left_result= 'bin/cviu/waseda/' + dataset + '_' + alg + '_L' + pic[-7:-4] + '.pgm'
      right_result='bin/cviu/waseda/' + dataset + '_' + alg + '_R' + pic[-7:-4] + '.pgm'
      # print dataset
      run_test_with_dataset(dataset, alg)


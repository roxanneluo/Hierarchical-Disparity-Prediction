import os
import sys
import subprocess

import httplib

#### arguments ####
alg = sys.argv[1] 
maxdisp = sys.argv[2]
scale = sys.argv[3]
########

#### configuration ####
use_lab = '0'
########


path = 'testdata/waseda/'
datasets = subprocess.check_output(['ls', path]).split()
left = ''
right = ''
left_result = ''
right_result = ''


def run_test_with_dataset(dataset, alg) :
    """ path should end with a / """
    what = subprocess.check_output([
        './bin/main/'+alg+'.bin', 
        left, right, 
        maxdisp, scale,
        left_result,
        '0', use_lab,
    ])
    print what


result_folder = 'results/waseda' + maxdisp
subprocess.check_output(['mkdir', '-p', result_folder])
for dataset in datasets :
  pic_names = subprocess.check_output(['ls', path + dataset]).split()
  for pic in pic_names :
    if pic.endswith('.ppm') and pic.startswith('L') :
      # print pic[-7:]
      left = path + dataset + '/L' + pic[-7:]
      right = path + dataset + '/R' + pic[-7:]
      left_result= result_folder+'/' + dataset + '_' + alg + '_L' + pic[-7:-4] + '.pgm'
      right_result= result_folder+'/' + dataset + '_' + alg + '_R' + pic[-7:-4] + '.pgm'
      # print dataset
      run_test_with_dataset(dataset, alg)


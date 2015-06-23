import os
import sys
import subprocess

import httplib

#### arguments ####
alg = sys.argv[1] 
dataset = sys.argv[2]
maxdisp = '256'
scale = '1'
####

#### configuration ####
use_lab = '0'
####

folder = 'testdata/kitti_stereo_flow/' + dataset
left_folder = folder + '/colored_0/'
right_folder = folder + '/colored_1/'
#datasets = subprocess.check_output(['ls', path]).split()
left = ''
right = ''
left_result = ''
right_result = ''


def run_test_with_dataset(alg) :
    """ path should end with a / """
    what = subprocess.check_output([
        './bin/main/'+alg+'.bin', 
        left, right, 
        maxdisp, scale,
        left_result,
        '1', #fullsize,
        use_lab,
    ])
    print what

#for dataset in datasets :
# subprocess.check_output(['mkdir', 'bin/cviu/kitti'+maxdisp])

result_folder = 'results/kitti_stereo/'
subprocess.check_output(['mkdir','-p',result_folder])
result_sha = dataset + '_' + alg
result_folder += result_sha + '/'
subprocess.check_output(['mkdir','-p',result_folder])
result_folder += 'data/'
subprocess.check_output(['mkdir','-p',result_folder])

pic_names = subprocess.check_output(['ls', left_folder]).split()
for pic in pic_names :
  if pic.endswith('10.png') :
      # print pic[-7:]
      left = left_folder + pic
      print left
      right = right_folder + pic
      print right
      left_result= result_folder + pic
      # print dataset
      print left_result
      run_test_with_dataset(alg)

print subprocess.check_output(['bin/checker/evaluate_stereo.bin',
                        result_sha])

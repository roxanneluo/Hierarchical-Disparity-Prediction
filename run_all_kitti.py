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
maxdisp = sys.argv[2]
scale = sys.argv[3]

path = 'testdata/kitti_0009/'
#datasets = subprocess.check_output(['ls', path]).split()
left = ''
right = ''
left_result = ''
right_result = ''


def run_test_with_dataset(alg) :
    """ path should end with a / """
    what = subprocess.check_output([
        './bin/'+alg+'.out', 
        left, right, 
        maxdisp, scale,
        left_result, right_result,
        '0', '1' #fullsize
    ])
    print what

#for dataset in datasets :
# subprocess.check_output(['mkdir', 'bin/cviu/kitti'+maxdisp])

result_folder = 'results/kitti'+maxdisp+'/'
subprocess.check_output(['mkdir','-p',result_folder])

pic_names = subprocess.check_output(['ls', path]).split()
for pic in pic_names :
  if pic.endswith('left.ppm') :
      # print pic[-7:]
      left = path + pic
      print left
      right = path + pic[:-8] + 'right.ppm'
      print right
      left_result= result_folder + alg + '_' + pic[-12:-8] + 'left.pgm'
      right_result=result_folder + alg + '_' + pic[-12:-8] + 'right.pgm'
      # print dataset
      print left_result
      print right_result
      run_test_with_dataset(alg)


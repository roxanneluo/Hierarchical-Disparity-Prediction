import os
import sys
import subprocess
from subprocess import CalledProcessError

DATASETS = (
  'Middlebury_4',
  'Middlebury_others',	
)

data_path = ''
pic_name = ''
thepath = ''

print 'Generate all support_cnt.txt:\n'

def run_gen_support_cnt(path, picture) :
	para = 0
	with open(path + 'spec.txt', 'r') as f:
		para = f.read().split()
	what = subprocess.check_output(['bin/cnt_Gen.out',
			picture,
			path + 'left.ppm',
			path + 'right.ppm',
			path + 'left_supportmap.pgm',
			path + 'right_supportmap.pgm',
			para[0],
			para[1],
			])

for dataset in DATASETS :
 	pic_names = subprocess.check_output(['ls', 'testdata/' + dataset + '/']).split()
	for picture in pic_names :
		print picture
		run_gen_support_cnt('testdata/' + dataset + '/' + picture + '/', picture)

print '\nDone.'	 

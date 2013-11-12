import os
import sys
import subprocess
from subprocess import CalledProcessError

DATASETS = (
	'Middlebury_4',	
)

cnt_path = 'code/code-probability/matlab/data/Prob_Gen_'

print 'Generate all small_given_large matrixs.\n'

def run_gen_small_given_large(test_path, data_path, piucture) :
	para = 0
	with open(test_path + 'spec.txt', 'r') as f:
		para = f.read().split()
	what = subprocess.check_output(['bin/SgL_Gen.out',
		data_path + picture + '__MST_cnt_1_0.txt',
		picture,
		para[0],
		])

for dataset in DATASETS :
	pic_names = subprocess.check_output(['ls', 'testdata/' + dataset + '/']).split()
	for picture in pic_names :
		print picture
		test_path = 'testdata/' + dataset + '/' + picture + '/'
		run_gen_small_given_large(test_path, cnt_path, picture)

print '\nDone.'

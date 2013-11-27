import os
import sys
import subprocess
from subprocess import CalledProcessError

DATASETS = (
	'Middlebury_4',
	'Middlebury_others',
)

algorithm = 'MSF'

left_result = 'MSF_left.pgm'
right_result = 'MSF_right.pgm'


def run_large_given_small_in_MSF(path, pic) :
	para = 0
	with open(path + 'spec.txt', 'r') as f :
		para = f.read().split()
	what = subprocess.check_output(['bin/' + algorithm + '.out',
			path + 'left.ppm',
			path + 'right.ppm',
			para[0],
			para[1],
			left_result,
			right_result,
			pic,])

print 'Generate all large given small matrix.\n'

for dataset in DATASETS :
	pic_names = subprocess.check_output(['ls', 'testdata/' + dataset + '/']).split()
	for picture in pic_names :
		print '~~~' + algorithm + '@' + dataset + '/' + picture + '~~~~~'
		thepath = 'testdata/' + dataset + '/' + picture + '/'
		run_large_given_small_in_MSF(thepath, picture)


print '\nDONE.'

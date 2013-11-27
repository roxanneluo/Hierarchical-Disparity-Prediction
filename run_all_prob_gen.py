import os
import sys
import subprocess
from subprocess import CalledProcessError

DATASETS = (
	#'Middlebury_4',
	'Middlebury_others',
)

algorithm = 'Prob_Gen'

def run_prob_gen(path, pic) :
	para = 0
	with open(path + 'spec.txt', 'r') as f :
		para = f.read().split()
	what = subprocess.check_output([
			'bin/' + algorithm + '.out',
			path + 'left.ppm',
			path + 'right.ppm',
			para[0],
			para[1],
			'all_prob_gen/Prob_Gen_' + picture + '_',
			path + 'displeft.pgm',
			path + 'dispright.pgm',
			])

print 'Generate all prob.\n'

for dataset in DATASETS :
	pic_names = subprocess.check_output(['ls', 'testdata/' + dataset + '/']).split()
	for picture in pic_names :
		print '~~~' + algorithm + '@' + dataset + '/' + picture + '~~~'
		thepath = 'testdata/' + dataset + '/' + picture + '/'
		run_prob_gen(thepath, picture)

print 'DONE.\n'

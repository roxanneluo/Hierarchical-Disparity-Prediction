import os
import sys
import subprocess
from subprocess import CalledProcessError

DATASETS = (
	'Middlebury_others',	
)

algorithm = 'bin/gen_interval_test.out'
output = 'all_interval/'

print 'Compare all initial cnt generated from c++ with matlab.\n'

def run_compare(path, pic) :
	para = 0
	with open(path + 'spec.txt', 'r') as f :
		para = f.read().split()
	what = subprocess.check_output([algorithm,
			pic,
			output + pic + '__average_search_range.txt',
			para[0]])


for dataset in DATASETS :
	pic_names = subprocess.check_output(['ls', 'testdata/' + dataset + '/']).split()
	for picture in pic_names :
		print '~~~' + algorithm + '@' + dataset + '/' + picture + '~~~'
		thepath = 'testdata/' + dataset + '/' + picture + '/'
		run_compare(thepath, picture)

print 'DONE.\n'


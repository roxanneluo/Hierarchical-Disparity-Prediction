import os
import sys
import random
import string
import subprocess
from subprocess import CalledProcessError

DATASETS = (
	'Middlebury_4',
	'Middlebury_others',
)
ALGORITHMS = (
	'Stable_MST',
	'Stable_RAND',
)
left_result = ''
right_result = ''
support_map = ''

def check_results(path, alg_name, x, y) :
	with open(path+'spec.txt', 'r') as f :
		para = f.read().split()
	res = subprocess.check_output([
		'bin/'+alg_name+'.out',
		path+'left.ppm',path+'right.ppm',
		para[0], para[1],
		left_result,
		right_result,
		support_map,
		alg_name,
		str(x[0]),str(y[0]),
		str(x[1]),str(y[1]),
		str(x[2]),str(y[2]),
		str(x[3]),str(y[3]),
		str(x[4]),str(y[4]),
		]) 

for dataset in DATASETS :
	pic_names = subprocess.check_output(['ls', 'testdata/'+dataset+'/']).split()
	if (dataset == 'Middlebury_others') :
		widthpos = 1
		heightpos = 2
	else :
		widthpos = 5
		heightpos = 6

	for picture in pic_names :
		thepath = 'testdata/'+dataset+'/'+picture+'/'
		with open(thepath+'left.ppm', 'r') as f:
			para = f.read().split()
		x = [random.randint(5,string.atoi(para[widthpos],10)-5) for i in range(0,5)]
		y = [random.randint(5,string.atoi(para[heightpos],10)-5) for i in range(0,5)]
		for alg_name in ALGORITHMS :
			print "~~~~"+picture+"~~~~"
			left_result = 'bin/stable/'+picture+'_left_'+alg_name+'.pgm'
			right_result = 'bin/stable/'+picture+'_right_'+alg_name+'.pgm'
			support_map = 'bin/stable/'+ picture + '_left_support_map'
			results = check_results(thepath, alg_name, x, y)

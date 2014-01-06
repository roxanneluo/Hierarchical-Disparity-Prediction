import os
import sys
import random
import string
import subprocess
from subprocess import CalledProcessError

DATASETS = (
	'input',
)
ALGORITHMS = (
	'Filter_RAND',
	'Filter_MST',
)

alg_name=''

def run(path,picture) :
	names = picture.split('.')
	ans = subprocess.check_output([
		'bin/'+alg_name+'.out',
		path,
		'toy/output/'+names[0],
		'25','10',
		'25','50',
		'25','90',
		'40','10',
		'40','50',
		'40','90',
		'50','10',
		'50','50',
		'50','90',
		'49','10',
		'49','50',
		'49','90',
		])
	print ans 


for dataset in DATASETS :
	pic_names = subprocess.check_output(['ls', 'toy/'+dataset+'/']).split()
	for picture in pic_names :
		thepath = 'toy/'+dataset+'/'+picture
		print "~~~~"+picture+"~~~~"
		for alg_name in ALGORITHMS :
			print alg_name
			run(thepath,picture)
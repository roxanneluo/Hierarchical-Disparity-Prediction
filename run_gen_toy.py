import os
import sys
import random
import string
import subprocess
from subprocess import CalledProcessError


algorithm = 'Gen_Toy'

for step in range(30,50,10):
	for side in range(20,101,20):
		print '~~~~~~~~step:'+str(step)+',side:'+str(side)+'~~~~~~~'
		ans = subprocess.check_output([
			'bin/Gen_Toy.out',
			'toy/input/toy',
			'2',
			str(step),
			str(side),
			str(side),
			])
		print ans

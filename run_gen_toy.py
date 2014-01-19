import os
import sys
import random
import string
import subprocess
from subprocess import CalledProcessError


algorithm = 'Gen_Toy'

for step in range(50,60,10):
	for side in range(40,61,10):
		print '~~~~~~~~step:'+str(step)+',side:'+str(side)+'~~~~~~~'
		ans = subprocess.check_output([
			'bin/Gen_Toy.out',
			'toy/input/toy',
			'3',
			str(step),
			str(side),
			str(side),
			])
		print ans

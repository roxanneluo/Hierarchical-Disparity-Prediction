import sys
import subprocess as sp

folders = sp.check_output(['ls', '.']).split()
filename = 'spec.txt'

for folder in folders:
	with open(folder + '/' + filename) as f:
		print folder
		for line in f:
			if line.strip() :
				print line

#######
# print the min and max height and width for testcases in $dataset_folder$
#######

import os
import sys
import subprocess

dataset_folder = sys.argv[1]

mwidth = 10000000
mheight = 10000000
Mwidth = 0
Mheight = 0
testcases = subprocess.check_output(['ls', dataset_folder]).split()
for testcase in testcases :
	if (testcase != 'size.py') :
		with open(dataset_folder + '/' + testcase+'/left.ppm') as f:
			line = f.read().split('\n')
			line = line[1]
			line = line.split()
			width = int(line[0])
			height = int(line[1])
			print str(width)+','+str(height)
			mwidth = min(mwidth, width)
			mheight = min(mheight, height)
#			print 'm:'+str(mwidth)+','+str(mheight)
			Mwidth = max(Mwidth, width)
			Mheight = max(Mheight, height)
print 'width:'+str(mwidth)+'~'+str(Mwidth)+',height:'+str(mheight)+'~'+str(Mheight)

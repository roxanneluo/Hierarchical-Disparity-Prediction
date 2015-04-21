import sys
import subprocess as sp

dataset = sys.argv[1]

fullsize_folder = '../testdata/' + dataset + '/'
folders = sp.check_output(['ls', fullsize_folder]).split()
filename = 'spec.txt'

for folder in folders:
  with open(fullsize_folder + folder + '/' + filename) as f:
	#	print folder
	#	for line in f:
	#		if line.strip() :
	#			print line
    print "{'%s', %s}, ..." % (folder, f.readline().strip())

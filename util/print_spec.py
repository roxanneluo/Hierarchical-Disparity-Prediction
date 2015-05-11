#####
# print the content in spec.txt for each testcase in dataset 
# in the format suitable for matlab cell. It is used to obtain 
# the dataset specification in matlab/get_dataset.
#
# argument: dataset (e.g., fullsize)
#####


import sys
import subprocess as sp

dataset = sys.argv[1]

dataset_folder = '../testdata/' + dataset + '/'
folders = sp.check_output(['ls', dataset_folder]).split()
filename = 'spec.txt'

for folder in folders:
  with open(dataset_folder + folder + '/' + filename) as f:
    print "{'%s', %s}, ..." % (folder, f.readline().strip())

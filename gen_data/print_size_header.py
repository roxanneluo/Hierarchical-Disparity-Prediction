import sys
import os
import subprocess

datasets_folder = sys.argv[1]
datasets = subprocess.check_output(['ls', datasets_folder]).split()

print 'Dataset\tImage Size\tMDE\tDataset(MDE)'
for dataset in datasets:
  folder = datasets_folder + '/' + dataset + '/'
  with open(folder + 'spec.txt', 'r') as f:
    max_disp = f.readline().split()[0]
  with open(folder + 'displeft.pgm', 'r') as f:
    f.readline()
    line = f.readline()
    while line.startswith('#') :
      line = f.readline()
    line = line.split()
    #line = f.readline().split()
    width = line[0]
    height = line[1]
    print '%s\t=%s*%s/1e6\t=%s*%s*%s/1e6\t%s(%.2f)' % (dataset, height, width, height,width,
        max_disp, dataset, int(height)*int(width)*int(max_disp)/1e6)

#########
# convert all displeft.ppm and dispright.ppm into displeft.pgm and dispright.pgm
########
import sys
import os
import subprocess

dataset_folder = sys.argv[1] #eg., ../testdata/fullsize

sources = ('displeft', 'dispright')

testcases = subprocess.check_output(['ls', dataset_folder]).split()
for testcase in  testcases:
  folder = dataset_folder + '/' + testcase + '/'
  for src in sources :
    in_filename = folder + src + '.ppm'
    out_filename = folder + src + '.pgm'
    print 'Processing ' + in_filename + ', output filename ' + out_filename
    print subprocess.check_output([
      '../bin/util/rgb2gray.bin',
      in_filename,
      out_filename
      ])

print 'done'

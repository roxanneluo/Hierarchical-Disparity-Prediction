import sys
import os
import subprocess

whole_folder = sys.argv[1]

sources = ('displeft', 'dispright')

datasets = subprocess.check_output(['ls', whole_folder]).split()
for dataset in  datasets:
  folder = whole_folder + '/' + dataset + '/'
  for src in sources :
    in_filename = folder + src + '.ppm'
    out_filename = folder + src + '.pgm'
    print 'Processing ' + in_filename + ', output filename ' + out_filename
    print subprocess.check_output([
      './rgb2gray.out',
      in_filename,
      out_filename
      ])

print 'done'

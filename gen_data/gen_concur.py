import subprocess as sp
import sys 
import datetime
import os

algo = sys.argv[1]
in_folder = sys.argv[2]
out_folder = sys.argv[3]

time_str = datetime.datetime.now().strftime('%Y-%m-%d-%H:%M:%S')
support_folder = out_folder + '/support_mean_' + time_str + '/'
concur_folder = out_folder + '/concur_mean_' + time_str + '/'
print sp.check_output(['mkdir', '-p', out_folder])
print sp.check_output(['mkdir', support_folder, concur_folder])

for dataset in os.listdir(in_folder) :
  folder = os.path.join(in_folder, dataset) + '/'
  if not os.path.isdir(folder) :
    continue

  with open(folder + '/spec.txt', 'r') as f:
    max_disp = f.readline().strip()
    scale = f.readline().strip()

  print '-------' + algo + ' ' + dataset + ' max_disp: '+ max_disp + \
      ' scale: ' + scale + '-------'
  print sp.check_output(['../bin/gen-concur-' + algo + '.out',
    folder + 'left.ppm', folder + 'right.ppm',
    max_disp, scale,
    folder + 'displeft.ppm',
    concur_folder, support_folder,
    dataset, algo])



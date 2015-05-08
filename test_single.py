import sys
import subprocess as sp

algo = sys.argv[1]
whole_dataset_folder = sys.argv[2]
dataset = sys.argv[3]
tot_threshold = sys.argv[4]
tolerances = sys.argv[5:]
fullsize_folder = 'testdata/' + whole_dataset_folder + '/'
result_folder = 'results/' + whole_dataset_folder + '/'

dataset_map = {'halfsize': 0, 'fullsize': 1}

dataset_folder = fullsize_folder + dataset + '/'
left_image = dataset_folder + 'left.ppm'
right_image = dataset_folder + 'right.ppm'
disp_left = dataset_folder + 'displeft.pgm'
disp_right = dataset_folder + 'dispright.pgm'
left_out = result_folder + dataset + '_left_' + algo + '.pgm'
right_out = result_folder + dataset + '_right_' + algo + '.pgm'
spec = dataset_folder + 'spec.txt'

with open(spec, 'r') as f:
  line_splits = f.read().split()
  max_disp = line_splits[0]
  scale = line_splits[1]
print algo + '\t' + dataset + '\tmax_disp: ' + max_disp + '\tscale: ' + scale

time = sp.check_output(['bin/'+algo+'.out',
      left_image, right_image,
      max_disp, scale,
      left_out, right_out,
      tot_threshold, 
      str(dataset_map[whole_dataset_folder])
    ])
print time

log_filename = result_folder + 'log_' + dataset + '_' + algo
log_file = open(log_filename, 'w')
log_file.write(time + '\n')

for tolerance in tolerances:
  checker_result = sp.check_output(['bin/red-checker.out',
          left_out, disp_left, disp_right,
          tolerance, scale,
          result_folder + 'err_'+dataset+'_left_'+algo+'_'+str(tolerance)+'.ppm'])
  print checker_result
  correct = int(checker_result.split()[0])
  total = int(checker_result.split()[1])
  err_str = 'error rate: ' + str(1 - float(correct)/total)
  result_str = 'error>=' + tolerance + ':' + checker_result + '\t' + err_str
  log_file.write(result_str+'\n')
  print result_str

import sys
import subprocess as sp

## argument list
algo = sys.argv[1]
dataset = sys.argv[2]  # halfsize/fullsize
testcase = sys.argv[3]               # eg. Aloe
tolerances = sys.argv[4:]
####
## configuration ##
use_lab = '0' # set to zero as default
err_all_red = '0' 
##
fullsize_folder = 'testdata/' + dataset + '/'
result_folder = 'results/' + dataset + '/'

dataset_map = {'halfsize': 0, 'fullsize': 1}

testcase_folder = fullsize_folder + testcase + '/'
left_image = testcase_folder + 'left.ppm'
right_image = testcase_folder + 'right.ppm'
disp_left = testcase_folder + 'displeft.pgm'
disp_right = testcase_folder + 'dispright.pgm'
left_out = result_folder + testcase + '_left_' + algo + '.pgm'
right_out = result_folder + testcase + '_right_' + algo + '.pgm'
spec = testcase_folder + 'spec.txt'

with open(spec, 'r') as f:
  line_splits = f.read().split()
  max_disp = line_splits[0]
  scale = line_splits[1]
print algo + '\t' + testcase + '\tmax_disp: ' + max_disp + '\tscale: ' + scale

time = sp.check_output(['bin/main/'+algo+'.bin',
      left_image, right_image,
      max_disp, scale,
      left_out,
      str(dataset_map[dataset]),
      use_lab
    ])
print time

log_filename = result_folder + 'log_' + testcase + '_' + algo
log_file = open(log_filename, 'w')
log_file.write(time + '\n')

for tolerance in tolerances:
  checker_result = sp.check_output(['bin/checker/checker_nonocc.bin',
          left_out, disp_left, disp_right,
          tolerance, scale,
          result_folder +
          'err_'+testcase+'_left_'+algo+'_'+str(tolerance)+'.ppm',
          err_all_red])
  print checker_result
  correct = int(checker_result.split()[0])
  total = int(checker_result.split()[1])
  err_str = 'error rate: ' + str(1 - float(correct)/total)
  result_str = 'error>=' + tolerance + ':' + checker_result + '\t' + err_str
  log_file.write(result_str+'\n')
  print result_str

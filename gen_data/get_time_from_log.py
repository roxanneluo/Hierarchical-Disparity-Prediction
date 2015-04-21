import subprocess
import sys

log_filename = sys.argv[1]
algo = sys.argv[2]
show_dataset = bool(int(sys.argv[3])) if len(sys.argv) > 3 else False

blocks = subprocess.check_output(['grep', '-A', '4', '~~~ ' + algo + '@',
  log_filename]).split('--\n')

for block in blocks:
  lines = block.split('\n')
  dataset = lines[0].split('@')[1][:-4]
  time = lines[2].split('all: ')[1][:-1]
  if show_dataset:
    print dataset + '\t' + time
  else :
    print time

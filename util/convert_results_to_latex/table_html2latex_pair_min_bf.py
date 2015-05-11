######
# convert table copied from html in $filename$
# to the format suitable for latex table.
# min in each pair of each row is shown bold
######

import sys
filename = sys.argv[1]

with open(filename, 'r') as f:
  line = f.readline()
  while line:
    line = line +  f.readline()
    fields = line.split()
    testcase = fields[0].split('.')[1]
    row = '{' + testcase + '}'
    errs = [float(fields[i]) * 100 for i in range(1, len(fields))]
    for i in range(int(len(errs)/2)) :
      m = min(errs[2*i:2*(i+1)])
      for j in range(2*i, 2*(i+1)) :
        row += ' & {'
        if errs[j] == m:
          row += '\\bf '
        row += '%.1f}' % errs[j]
    row += ' \\\\ \\hline'
    print row
    line = f.readline()

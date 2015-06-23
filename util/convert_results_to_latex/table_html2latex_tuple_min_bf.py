######
# convert table copied from html in $filename$
# to the format suitable for latex table.
# min in each pair of each row is shown bold
######

import sys
filename = sys.argv[1]
tuple_len = int(sys.argv[2]) if len(sys.argv) > 2 else 2
line_break = sys.argv[3] if len(sys.argv) > 3 else '\hline'
times = int(sys.argv[4]) if len(sys.argv) > 4 else 100
digit = sys.argv[5] if len(sys.argv) > 5 else '1'

with open(filename, 'r') as f:
  line = f.readline()
  while line:
    fields = line.split()
    testcase = fields[0].split('.')[-1]
    row = '{' + testcase + '}'
    errs = [float(fields[i]) * times for i in range(1, len(fields))]
    for i in range(int(len(errs)/tuple_len)) :
      m = min(errs[tuple_len*i:tuple_len*(i+1)])
      for j in range(tuple_len*i, tuple_len*(i+1)) :
        row += ' & {'
        if errs[j] == m:
          row += '\\bf '
        row += ('%.' + digit + 'f}') % errs[j]
    row += ' \\\\ \\hline'
    print row
    line = f.readline()

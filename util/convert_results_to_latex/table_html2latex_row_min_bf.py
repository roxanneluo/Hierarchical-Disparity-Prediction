######
# convert table copied from html in $filename$
# to the format suitable for latex table.
# min in each row is shown bold
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
    m = min(errs)
    for err in errs:
      row += ' & {'
      if err == m:
        row += '\\bf '
      row += '%.1f}' % err
    row += ' \\\\ \\hline'
    print row
    line = f.readline()

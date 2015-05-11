######
# convert table copied from html in $filename$
# to the format suitable for latex table.
# nothing is bold
######

import sys
filename = sys.argv[1]

with open(filename, 'r') as f:
  line = f.readline()
  while line:
    fields = line.split()
    testcase = fields[0]
    row = '{' + testcase + '}'
    errs = [float(fields[i]) * 100 for i in range(1, len(fields))]
    for err in errs:
      row += ' & {'
      row += '%.1f}' % err
    row += ' \\\\ \\hline'
    print row
    line = f.readline()

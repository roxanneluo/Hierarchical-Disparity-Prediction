import os
import sys
import subprocess

import httplib

total = 0
correct = 0

DATASET = (
		'halfsize',
		)

total = 0
correct = 0

table = {}

def check_results(path, dataset) :
  global correct
  global total
  ret = []
  with open(path + 'spec.txt', 'r') as f :
    para = f.read().split()
  res = subprocess.check_output(['bin/checker1.out',
      'bin/qx/' + dataset + '_disparity.pgm',
			path + 'displeft.pgm',
			'1',
			para[1],])
  tmp = map(int, res.split())
  correct = correct + tmp[0]
  total = total + tmp[1]
  rati = float(tmp[0]) / tmp[1]
  tmp.append(' %.6f' % rati)
  ret.append(tmp)
  return ret

for dataset in DATASET :
  table[dataset] = {}
  pic_names = subprocess.check_output(['ls',
      'testdata/' + dataset + '/']).split()
  temp = (total, correct)
  for pic in pic_names :
    print pic
    results = check_results('testdata/' + dataset + '/' + pic + '/', pic)
    table[dataset][pic] = results
  table[dataset]['Overall'] = [correct, total]

print '\nDode.'

report = open('bin/SuperReport_QX.html', 'w')
report.write('<!DOCTYPE html>\n<html>\n<body>\n')

report.write('<h1>Dataset Overview</h1>\n')
report.write('<table border = "3">\n')
report.write('<tr> <th>Algorithm</th>')

for dataset in DATASET :
  report.write('<th>' + dataset + '</th>')
report.write('</tr>\n')
report.write('<tr><td> MST_QX </td>')
for dataset in DATASET :
  report.write('<td> %.6f </td>' % (float(table[dataset]['Overall'][0]) / float(table[dataset]['Overall'][1])))
report.write('</tr>\n')
report.write('</table>\n')

report.write('<h1>Dataset Detail</h1>\n')
report.write('<table border = "3">\n')
report.write('<tr><td>Test Data </td> <td> MST_QX </td></tr>\n')

for dataset in DATASET :
  pic_names = subprocess.check_output(['ls', 'testdata/' + dataset + '/']).split()
  for pic in pic_names :
    report.write('<tr><td>' + dataset + '.' + pic + '</td>')
    report.write('<td>' + table[dataset][pic][0][2] + '</td>')
    report.write('</tr>\n')
report.write('</table>\n')
report.write('/html')
report.close()


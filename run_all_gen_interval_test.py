import os
import sys
import subprocess
from subprocess import CalledProcessError

DATASETS = (
	'Middlebury_others',	
)

algorithm = 'bin/gen_interval_test.out'
output = 'all_gen_interval/'

table = {}
total = 0.0
interval = 0.0


print 'Compare all initial cnt generated from c++ with matlab.\n'

def run_interval_test(path, pic) :
	para = 0
	ret = []
	global total
	global interval	
	with open(path + 'spec.txt', 'r') as f :
		para = f.read().split()
	try :
		what = subprocess.check_output([algorithm,
				pic,
				para[0]])
		tmp =  map(float, what.split())
		total = total + tmp[0]
		interval = interval + tmp[1]
#		interval = tmp[0];
#   print interval
		rati = float(tmp[1]) * 100 / tmp[0];
#		print rati
		tmp.append(' %.6lf' % rati)
		ret.append(tmp)
	except CalledProcessError :
		print 'null'
		ret.append([0, 0, 'null'])
	return ret

for dataset in DATASETS :
	total = 0.0
	interval = 0.0
	table[dataset] = {}
	pic_names = subprocess.check_output(['ls', 'testdata/' + dataset + '/']).split()
	for picture in pic_names :
		print '~~~' + algorithm + '@' + dataset + '/' + picture + '~~~'
		thepath = 'testdata/' + dataset + '/' + picture + '/'
		results = run_interval_test(thepath, picture)
		table[dataset][picture] = results
	table[dataset]['Overall'] = [total, interval]	
print 'DONE.\n'

report = open('bin/GenerateDisparityIntervalTest.html', 'w')
report.write('<!DOCTYPE html>\n<html>\n<boby>\n')

report.write('<h1>Overall Average Search Range Based On Generated Disparity Range</h1>\n')
report.write('<table border = "3">\n')
report.write('<tr> <th>Dataset</th> <th>Avg. Search Range</th>\n')
for dataset in DATASETS :
	report.write('<tr><td>' + dataset + '</td>')
	report.write('<td> %.6f %% </td>'
							% (table[dataset]['Overall'][1] * 100 /
								 table[dataset]['Overall'][0])
							)
	report.write('</tr>\n')
report.write('</table>\n')

report.write('<h1>Average Search Range for each dataset</h1>\n')
report.write('<table border = "3">\n')
report.write('<tr><td>Test Data</td>')
for side in ('Avg. Disparity Interval', 'Avg. Search Ratio') :
	report.write('<td colspan = "1">' + side + '</td>')
report.write('</tr>\n')

for dataset in DATASETS :
	pic_names = subprocess.check_output(['ls', 'testdata/' + dataset + '/']).split()
	for picture in pic_names :
		report.write('<tr><td>' + dataset + '.' + picture + ' (' + str(int(table[dataset][picture][0][0])) + ')' + '</td>')
		report.write('<td>' + str(table[dataset][picture][0][1]) + '</td>')
		report.write('<td>' + str(table[dataset][picture][0][2]) + ' % ' + '</td>')
		report.write('</tr>\n')
report.write('</html>')

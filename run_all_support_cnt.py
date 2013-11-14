import os
import sys
import subprocess
from subprocess import CalledProcessError

DATASETS = (
  'Middlebury_4',
  'Middlebury_others',	
)

data_path = ''
pic_name = ''
thepath = ''
total = 0
correct = 0
table = {}

print 'Generate all support_cnt.txt:\n'

def run_gen_support_cnt(path, picture) :
	para = 0
	with open(path + 'spec.txt', 'r') as f:
		para = f.read().split()
	what = subprocess.check_output(['bin/cnt_Gen.out',
			picture,
			path + 'left.ppm',
			path + 'right.ppm',
			path + 'left_supportmap.pgm',
			path + 'right_supportmap.pgm',
			para[0],
			para[1],
			])

def check_support_map(path) :
	global correct
	global total
	para = 0
	tolerance = '1'
	ret = []
	side = ('left', 'right')
	with open(path + 'spec.txt', 'r') as f:
		para = f.read().split()
	output = (path + 'left_supportmap.pgm', path + 'right_supportmap.pgm')
	for side, outfile in zip( ('left', 'right'), output ) :
		try :
			res = subprocess.check_output(['bin/checker_support.out',
					outfile,
					path + 'disp' + side + '.pgm',
					tolerance,
					para[1],
					])
			tmp = map(int, res.split())
			correct = correct + tmp[0];
			total = total + tmp[1];
			rati = float(tmp[0]) / tmp[1]
			print rati
			tmp.append(' %.6f ' % rati)
			ret.append(tmp)
		except CalledProcessError :
			print 'null'
			ret.append([0, 0, 'null'])
	return ret

for dataset in DATASETS :
	table[dataset] = {}
 	pic_names = subprocess.check_output(['ls', 'testdata/' + dataset + '/']).split()
	for picture in pic_names :
		print picture
		run_gen_support_cnt('testdata/' + dataset + '/' + picture + '/', picture)
		results =	check_support_map('testdata/' + dataset + '/' + picture + '/')
		table[dataset][picture] = results

print '\nDone.'
report = open('bin/SupportMapReport.html', 'w')
report.write('<!DOCTYPE html>\n<html>\n<body>\n')

report.write('<h1>Support Map Matching Ratio</h1>\n')
report.write('<table border = "3">\n')
report.write('<tr><td>Test Data</td>')
for side in ('Left Support Map', 'Right Support Map') :
	report.write('<td colspan = "1">' + side + '</td>')
report.write('</tr>\n')

for dataset in DATASETS :
	pic_names = subprocess.check_output(['ls', 'testdata/' + dataset + '/']).split()
	for picture in pic_names :
		report.write('<tr><td>' + dataset + '.' + picture + '</td>')
		for i in [0, 1] :
			report.write('<td>' + table[dataset][picture][i][2] + '</td>')
		report.write('</tr>\n')

report.write('</html>')	

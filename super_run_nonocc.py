import os
import sys
import subprocess
from subprocess import CalledProcessError

DATASETS = ( 
#    'Middlebury_1',
#    'Middlebury_4',
#    'Middlebury_others',
    'halfsize',
#    'Middlebury_bad',
)
ALGORITHMS = (
    'rt',
    'dpf-rt',	
)
CHECKER = 'checker.out'

data_path = ''
pic_names = '' 
total = 0
correct = 0
table = {}
html_name = 'bin/SuperReport.html'

def check_results(algo, path) : #, left_result = 'leftdisp.pgm', right_result = 'rightdisp.pgm') :
    global correct
    global total
    para = 0
    with open(path + 'spec.txt','r') as f :
        para = f.read().split()
    output = ('bin/left.pgm', 'bin/right.pgm')

    #run the algorithm
    subprocess.check_output([
        'bin/' + algo + '.out', 
        path + 'left.ppm', path + 'right.ppm', 
        para[0], para[1],
        output[0], output[1],
    ])

    tolerance = '1'
    command = ['bin/'+CHECKER, 
        path + 'displeft.pgm', path + 'dispright.pgm',
        tolerance, para[1]]

    res = subprocess.check_output(command)
    tmp = map(int, res.split())
    correct = correct + tmp[0]
    total = total + tmp[1]
    rati = float(tmp[0]) / tmp[1]
    print rati
    return tmp

for algoritm in ALGORITHMS :
    table[algoritm] = {}
    total = 0
    correct = 0
    temp0 = (correct, total)
    for dataset in DATASETS :
        table[algoritm][dataset] = {}
        pic_names = subprocess.check_output(['ls', 'testdata/' + dataset + '/']).split()
        temp = (correct, total)
        for picture in pic_names :
            print "~~~ "+ algoritm + '@' + dataset + '/' + picture +" ~~~"
            thepath = 'testdata/' + dataset + '/' + picture + '/'
            results = check_results(algoritm, thepath)
            table[algoritm][dataset][picture] = results
        table[algoritm][dataset]['Overall'] = [correct - temp[0], total - temp[1]]
    table[algoritm]['Overall'] = [correct - temp0[0], total - temp0[1]]

print '\nDone.'

html_name = sys.argv[1]
report = open(html_name, 'w')
report.write('<!DOCTYPE html>\n<html>\n<body>\n')

report.write('<h1>Algorithm Overview</h1>\n')
report.write('<table border="3">\n')
report.write('<tr> <th>Algorithm</th> <th> ErrorRate </th> </tr>\n')
for algoritm in ALGORITHMS :
    report.write('<tr><td>'+algoritm+'</td>')
    report.write('<td> %.6f </td>' 
                % (1.0 - float(table[algoritm]['Overall'][0])
											 / float(table[algoritm]['Overall'][1])))
    report.write('</tr>\n')
report.write('</table>\n')

report.write('<h1>Dataset Overview</h1>\n')
report.write('<table border="3">\n')
report.write('<tr> <th>Algorithm</th> ')
for dataset in DATASETS :
    report.write('<th>'+dataset+'</th> ')
report.write('</tr>\n')
for algoritm in ALGORITHMS :
    report.write('<tr>')
    report.write('<td>' + algoritm + '</td>')
    for dataset in DATASETS :
        report.write('<td> %.6f </td>' 
            % (1.0 - float(table[algoritm][dataset]['Overall'][0])
                   / float(table[algoritm][dataset]['Overall'][1])))
    report.write('</tr>\n')
report.write('</table>\n')

report.write('<h1>Dataset Detail</h1>\n')
report.write('<table border="3">\n')
report.write('<tr><td>Test Data</td>')
for algoritm in ALGORITHMS :
    report.write('<td>'+algoritm+'</td>')
report.write('</tr>\n')

for dataset in DATASETS :
    pic_names = subprocess.check_output(['ls', 'testdata/' + dataset + '/']).split()
    for picture in pic_names :
        report.write('<tr><td>'+dataset+'.'+picture+'</td>')
        tmp = {}
        for i in [0, 1] :
            tmp[i] = []
            for algoritm in ALGORITHMS :
                tmp[i].append(table[algoritm][dataset][picture][i][2])
        for algoritm in ALGORITHMS :
            color = []
            for i in [0, 1] :
                if table[algoritm][dataset][picture][i][2] == max(tmp[i]) :
                    color.append("#F79F81")
                elif table[algoritm][dataset][picture][i][2] == min(tmp[i]) :
                    color.append("#BEF781")
                else :
                    color.append("white")

            for i in [0, 1] :
                report.write('<td bgcolor = ' + color[i] + '>'+table[algoritm][dataset][picture][i][2]+'</td>')
        report.write('</tr>\n')

report.write('</html>')
report.close()

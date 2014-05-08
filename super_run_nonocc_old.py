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
#    'STCostAggre',
#    'Original',
#    'MSF_ALL_MST',
#		'RandTreeEdgeAware',
#    'ST',    
#    'MSF_ALL_ST',
#    'RandTree',
#		'RandTree_ust',
#    'MST_blind',
#       'MSF2',
#     'MSF2_TEST',
#     'MSF2_LAST_RAND',
#    'silly',
#    'mst',
#    'dpf-mst',
#    'st',
#    'dpf-st',
    'rt',
    'dpf-rt',	
)

data_path = ''
pic_names = '' # 
left_result = ''
right_result = ''
total = 0
correct = 0
table = {}
html_name = 'bin/SuperReport.html'

def run_ST_with_path(path, output) :
    """ path should end with a / """
    para = 0
    with open(path + 'spec.txt','r') as f :
        para = f.read().split()
    what = subprocess.check_output([
        'bin/' + algoritm + '.out', 
        path + 'left.ppm', path + 'right.ppm', 
        output,
        para[0],para[1],
        '0.1',
        'ST-2',
    ])

def run_test_with_path(path, left_result, right_result) :
    """ path should end with a / """
    para = 0
    with open(path + 'spec.txt','r') as f :
        para = f.read().split()
    what = subprocess.check_output([
        'bin/' + algoritm + '.out', 
        path + 'left.ppm', path + 'right.ppm', 
        para[0], para[1],
        left_result, right_result,
    ])

def check_results(path) : #, left_result = 'leftdisp.pgm', right_result = 'rightdisp.pgm') :
    global correct
    global total
    para = 0
    tolerance = '1'
    ret = []
    with open(path + 'spec.txt','r') as f :
        para = f.read().split()
    output = ('bin/left.pgm', 'bin/right.pgm')
    #for side, filename in zip(('left', 'right'), (left_result, right_result)) :
    if algoritm == 'STCostAggre' :
        run_ST_with_path(thepath, output[0])
        run_ST_with_path(thepath, output[1])
    else :
        run_test_with_path(thepath, output[0], output[1])
    for side, outfile in zip( ('left','right'), output ) :
        left_or_right = '0'
        if side == 'right' :
          left_or_right = '1'
        try :
            res = subprocess.check_output(['bin/checker_nonocc.out',
                                              outfile, 
                                              path + 'displeft.pgm',
																							path + 'dispright.pgm',
                                              tolerance,
                                              para[1],
																							left_or_right])
            tmp = map(int, res.split())
            correct = correct + tmp[0]
            total = total + tmp[1]
            rati = float(tmp[0]) / tmp[1]
            print rati
            tmp.append(' %.6f ' % (1.0 - rati))
            ret.append(tmp)
        except CalledProcessError :
#            print 'null'
            ret.append([0,0, 'null'])
    return ret

for algoritm in ALGORITHMS :
    table[algoritm] = {}
    total = 0
    correct = 0
    for dataset in DATASETS :
        table[algoritm][dataset] = {}
        pic_names = subprocess.check_output(['ls', 'testdata/' + dataset + '/']).split()
        temp = (total, correct)
        for picture in pic_names :
            print "~~~ "+ algoritm + '@' + dataset + '/' + picture +" ~~~"
            #left_result= 'bin/' + algoritm + '_' + picture + '_left.pgm'
            #right_result='bin/' + algoritm + '_' + picture + '_right.pgm'
            thepath = 'testdata/' + dataset + '/' + picture + '/'
            results = check_results(thepath)
            table[algoritm][dataset][picture] = results
        table[algoritm][dataset]['Overall'] = [correct - temp[0], total - temp[1]]
    table[algoritm]['Overall'] = [correct, total]

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
    report.write('<td colspan="2">'+algoritm+'</td>')
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

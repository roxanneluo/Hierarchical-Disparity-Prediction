import os
import sys
import subprocess
from subprocess import CalledProcessError

DATASETS = [ 
#    'Middlebury_1',
#    'Middlebury_4',
#    'Middlebury_others',
#    'halfsize',
#    'Middlebury_bad',
#    'fullsize'
#    'halfsize',
]
ALGORITHMS = (
    #'dpf-mst-mean-mst-GMM-1',
    #'dpf-st-mean-mst-GMM-1',
    #'dpf-rt-mean-mst-GMM-1',
    'dpf-mst-mean-mst-GMM-0.5-new-notot',
    'dpf-st-mean-mst-GMM-0.5-new-notot',
    'dpf-rt-mean-mst-GMM-0.5-new-notot',
    'dpf-mst-mean-mst-GMM-0.5-notot',
    'dpf-st-mean-mst-GMM-0.5-notot',
    'dpf-rt-mean-mst-GMM-0.5-notot',
    #'dpf-st-4-mean-mst-GMM-0.5-notot',
    #'dpf-rt-mean-mst-GMM-0.5-notot',
    #'dpf-st-mean-own-GMM-0.5',
    #'dpf-st-mean-own-GMM',
    #'dpf-mst-bai',
    #'dpf-st-bai',
    #'dpf-rt-bai',
    #'dpf-mst-mean',
    'mst',
    #'dpf-mst',
    #'dpf-st-mean',
    'st',
    #'dpf-st',
    'rt',
    #'dpf-rt-mean',
    #'dpf-rt',
    #'dpf-mst-4',
    #'dpf-mst-4-lab',
    #'dpf-st-4',
    #'dpf-st-4-lab',
    #'dpf-rt-4',
    #'dpf-rt-4-lab',
    #'dpf-mst-lab',
    #'dpf-mst',	
    #'mst',
    #'dpf-st-lab',
    #'dpf-st',
    #'st',
    #'dpf-rt-lab',
    #'dpf-rt',
    #'rt',
)
CHECKER = 'checker.out'
tot_threshold = '0'

tolerance = sys.argv[1]
html_name_pre = sys.argv[2]
for dataset in sys.argv[3:] :
  DATASETS.append(dataset) # argv[1], argv[2], ... gives the dataset

dataset_table = {'halfsize': 0, 'fullsize': 1}

data_path = ''
pic_names = '' 
total = 0
correct = 0
table = {}
html_name = 'results/SuperRepor_' + html_name_pre + '_err_ge_' + tolerance + '_'.join(DATASETS) + '.html'

def ratio(x) :
    return float(x[0]) / float(x[1])

def check_results(algo, path, dataset, picture) : #, left_result = 'leftdisp.pgm', right_result = 'rightdisp.pgm') :
    global correct
    global total
    para = 0
    with open(path + 'spec.txt','r') as f :
        para = f.read().split()
    output = ('results/' + picture + '_left_' + algo + '.pgm',
        'results/' + picture + '_left_' + algo + '.pgm')

    #run the algorithm
    print subprocess.check_output([
        'bin/' + algo + '.out', 
        path + 'left.ppm', path + 'right.ppm', 
        para[0], para[1],
        output[0], output[1],
        tot_threshold,
        str(dataset_table[dataset]),
    ])

    command = ['bin/'+CHECKER, output[0],
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
            results = check_results(algoritm, thepath, dataset, picture)
            table[algoritm][dataset][picture] = results
        table[algoritm][dataset]['Overall'] = [correct - temp[0], total - temp[1]]
    table[algoritm]['Overall'] = [correct - temp0[0], total - temp0[1]]

print '\nDone.\n'

print "Saving result to " + html_name
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

COLOR_RED =   "#F79F81"
COLOR_GREEN = "#BEF781"
for dataset in DATASETS :
    pic_names = subprocess.check_output(['ls', 'testdata/' + dataset + '/']).split()
    for picture in pic_names :
        report.write('<tr><td>'+dataset+'.'+picture+'</td>')
        tmp = [table[i][dataset][picture][0] for i in ALGORITHMS]

        for algoritm in ALGORITHMS :
            color = []
            if table[algoritm][dataset][picture][0] == min(tmp) :
                color = COLOR_RED
            elif table[algoritm][dataset][picture][0] == max(tmp) :
                color = COLOR_GREEN
            else :
                color = "white"
            report.write('<td bgcolor = ' + color + '>'+str(1-ratio(table[algoritm][dataset][picture]))+'</td>')
        report.write('</tr>\n')

report.write('</html>')
report.close()

import os
import sys
import subprocess
from subprocess import CalledProcessError

## configuration ##
DATASETS = [ 
#    'fullsize'
#    'halfsize',
#    'Middlebury_1',
#    'Middlebury_4',
#    'Middlebury_others',
#    'Middlebury_bad',
]
ALGORITHMS = (
    'dp_mst',
    'dp_st',
    'dp_rt',
#    'mst',
#    'st',
#    'rt',
#    'dp_st_disp_prior',
)
CHECKER = 'checker_nonocc.bin'
use_lab = '0' # set to zero as default
####

## argument list ##
tolerance = sys.argv[1]
result_folder = sys.argv[2]
html_name_pre = sys.argv[3]
pixel_intv_threshold = sys.argv[4]
tree_intv_threshold = sys.argv[5]
for dataset in sys.argv[6:] :
  DATASETS.append(dataset) # argv[1], argv[2], ... gives the dataset
####

dataset_table = {'halfsize': 0, 'fullsize': 1}

data_path = ''
pic_names = '' 
total = 0
correct = 0
table = {}
html_name = result_folder + 'SuperReport_' + html_name_pre + '_err_ge_' + tolerance +\
'_'.join(DATASETS) + '_pixelIntvThreshold' +  pixel_intv_threshold + '_treeIntvThreshold' + tree_intv_threshold + '.html'
print html_name

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
        'bin/main/' + algo + '.bin', 
        path + 'left.ppm', path + 'right.ppm', 
        para[0], para[1],
        output[0],
        str(dataset_table[dataset]),
        use_lab,
        pixel_intv_threshold,
        tree_intv_threshold,
    ])

    command = ['bin/checker/'+CHECKER, output[0],
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
    table[algorithm]['Overall'] = 0
    for dataset in DATASETS :
        table[algorithm][dataset] = {'Overall':0}
        pic_names = subprocess.check_output(['ls', 'testdata/' + dataset + '/']).split()
        temp = (correct, total)
        for picture in pic_names :
            print "~~~ "+ algoritm + '@' + dataset + '/' + picture +" ~~~"
            thepath = 'testdata/' + dataset + '/' + picture + '/'
            results = check_results(algoritm, thepath, dataset, picture)
            table[algoritm][dataset][picture] = results
            table[algorithm][dataset]['Overall'] += results
        table[algorithm][dataset]['Overall'] /= float(len(pic_names))
    table[algorithm]['Overall'] /= float(len(DATASETS))

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

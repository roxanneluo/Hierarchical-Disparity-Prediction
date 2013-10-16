import os
import sys
import subprocess
from subprocess import CalledProcessError

NOW_PATH = ""
other_path = 'testdata/Middlebury_4/'
alg_name = sys.argv[1]

pic_names = subprocess.check_output(['ls', other_path]).split()
left_result = ''
right_result = ''

def run_test_with_path(path) :
    """ path should end with a / """
    para = 0
    with open(path + 'spec.txt','r') as f :
        para = f.read().split()
    what = subprocess.check_output([
        'bin/' + sys.argv[1] + '.out', 
        path + 'left.ppm', path + 'right.ppm', 
        para[0], para[1],
        left_result,
        right_result,
    ])

total = 0
correct = 0

def check_results(p) :
    global correct
    global total
    para = 0
    tolerance = '1'
    with open(p + 'spec.txt','r') as f :
        para = f.read().split()
    try :
        res = subprocess.check_output(['bin/checker1.out',
                                       left_result,
                                       p + 'displeft.pgm',
                                       tolerance,
                                       para[1],])
        tmp = map(int, res.split())
        correct = correct + tmp[0]
        total = total + tmp[1]
        rati = float(tmp[0]) / tmp[1]
        print rati
    except CalledProcessError :
        print 'null'
    try :
        res = subprocess.check_output(['bin/checker1.out',
                                       right_result,
                                       p + 'dispright.pgm',
                                       tolerance,
                                       para[1],])
        tmp = map(int, res.split())
        correct = correct + tmp[0]
        total = total + tmp[1]
        rati = float(tmp[0]) / tmp[1]
        print rati
    except CalledProcessError :
        print 'null'

for who in pic_names :
    print "~~~ "+ who +" ~~~"
    left_result= 'bin/' + alg_name + '_' + who + '_left.pgm'
    right_result='bin/' + alg_name + '_' + who + '_right.pgm'
    thepath = other_path + who + '/'
    run_test_with_path(thepath)
    check_results(thepath)

print "----- Final result -----"
print "Total = {}\nCorrect = {}".format(total, correct)
print "Overall ratio = %.10f" % (float(correct)/ total)
print "{} {}".format(total, correct) + (" %.10f" % (float(correct)/ total))


import os
import sys
import subprocess

import httplib

NOW_PATH = ""
#other_path = 'testdata/cviu/'
other_path='testdata/aloe/'
alg_name = sys.argv[1]
pic_names = subprocess.check_output(['ls', other_path]).split()

left_result= 'bin/silly_left.pgm'
right_result='bin/silly_right.pgm'

def run_test_with_path(path) :
    """ path should end with a / """
    para = 0
    with open(path + 'spec.txt','r') as f :
        para = f.read().split()
    what = subprocess.check_output([
        'bin/'+sys.argv[1]+'.out', 
        path + 'left.ppm', path + 'right.ppm', 
        para[0], para[1],
        left_result,
        right_result,

    ])
    print what

def run_test_with_path_and_dataset(path, dataset) :
    """ path should end with a / """
    para = 0
    with open(path + 'spec.txt','r') as f :
        para = f.read().split()
    what = subprocess.check_output([
        'bin/'+sys.argv[1]+'.out', 
        path + 'left.ppm', path + 'right.ppm', 
        para[0], para[1],
        left_result,
        right_result,
				dataset,
    ])
    print what



total = 0
correct = 0

def check_results(p) :
    global correct
    global total
    para = 0
    tolerance = '1'
    with open(p + 'spec.txt','r') as f :
        para = f.read().split()
    res = subprocess.check_output(['bin/checker_nonocc_red_cviu.out',
                                   left_result,
                                   p + 'displeft.pgm',
																	 p + 'dispright.pgm',
                                   tolerance,
                                   para[1],
																	 '0',])
    print res
    tmp = map(int, res.split())
    correct = correct + tmp[0]
    total = total + tmp[1]
    print float(tmp[0]) / tmp[1]
    res = subprocess.check_output(['bin/checker_nonocc_red_cviu.out',
                                   right_result,
                                   p + 'displeft.pgm',
																	 p + 'dispright.pgm',
                                   tolerance,
                                   para[1],
																	 '1',])
    print res
    tmp = map(int, res.split())
    correct = correct + tmp[0]
    total = total + tmp[1]
    print float(tmp[0]) / tmp[1]


for who in pic_names :
    print "~~~ "+ who +" ~~~"
    thepath = other_path + who + '/'
    left_result= 'bin/cviu/' + alg_name + '_' + who + '_left.pgm'
    right_result='bin/cviu/' + alg_name + '_' + who + '_right.pgm'
#run_test_with_path(thepath)
    run_test_with_path_and_dataset(thepath, who)
    check_results(thepath)

print "----- Final result -----"
print "Total = {}\nCorrect = {}\n".format(total, correct)
print "Overall ratio = %.10f" % (float(correct)/ total)

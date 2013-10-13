import os
import sys
import subprocess

import httplib

NOW_PATH = ""
other_path = 'testdata/Middlebury_others/'

pic_names = subprocess.check_output(['ls', other_path]).split()

left_result= 'bin/silly_left.pgm'
right_result='bin/silly_right.pgm'

def run_test_with_path(path) :
    """ path should end with a / """
    para = 0
    with open(path + 'spec.txt','r') as f :
        para = f.read().split()
    what = subprocess.check_output([
        'bin/silly.out', 
        path + 'left.ppm', path + 'right.ppm', 
        para[0], para[1],
        'bin/silly_left.pgm',
        'bin/silly_right.pgm', 
    ])
    print what



def check_results(p) :
    #global left_restult
    #global right_result
    res = subprocess.check_output(['bin/checker.out', left_result, p + 'displeft.ppm', '3'])
    print "Check left :\n" + res
    res = subprocess.check_output(['bin/checker.out', right_result, p + 'dispright.ppm', '3'])
    print "Check right :\n" + res


thepath = 0
try :
    y = int(sys.argv[1])
    thepath = other_path + pic_names[int(sys.argv[1])] + '/'
except ValueError :
    thepath = other_path + name + '/'

run_test_with_path(thepath)
check_results(thepath)

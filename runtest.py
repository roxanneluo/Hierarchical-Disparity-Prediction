import os
import sys
import subprocess

import httplib

NOW_PATH = ""
other_path = 'testdata/Middlebury_others/'

pic_names = subprocess.check_output(['ls', other_path]).split()


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

def run_test_with_name(name) :
    run_test_with_path(other_path + name + '/')

def run_test_with_number(number) : 
    run_test_with_path(other_path + pic_names[int(sys.argv[1])] + '/')


try :
    y = int(sys.argv[1])
    run_test_with_number(y)
except ValueError :
    run_test_with_name(sys.argv[1])



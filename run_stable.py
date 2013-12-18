import os
import sys
import random
import string
import subprocess
from subprocess import CalledProcessError

DATASETS = (
	'Middlebury_4',
	'Middlebury_others',
)
ALGORITHMS = (
	'Stable_MST',
	'Stable_RAND',
)
left_result = ''
right_result = ''
support_map = ''
alg_to_gen_points = 'Original'
points = []
picked = []
picknum = 5

def print_stable(path, alg_name, x, y) :
	with open(path+'spec.txt', 'r') as f :
		para = f.read().split()

	points_str = ''
	for i in range(0, picknum) :
		points_str += picked[i]+' '

	res = subprocess.check_output([
		'bin/'+alg_name+'.out',
		path+'left.ppm',path+'right.ppm',
		para[0], para[1],
		left_result,
		right_result,
		support_map,
		alg_name,
		# str(x[0]),str(y[0]),
		# str(x[1]),str(y[1]),
		# str(x[2]),str(y[2]),
		# str(x[3]),str(y[3]),
		# str(x[4]),str(y[4]),
		points_str,
		]) 

def run_test_with_path(path, alg) :
    """ path should end with a / """
    para = 0
    print 'run test with path '+path+' '+alg
    with open(path + 'spec.txt','r') as f :
        para = f.read().split()
    what = subprocess.check_output([
        'bin/' + alg + '.out', 
        path + 'left.ppm', path + 'right.ppm', 
        para[0], para[1],
        left_result,
        right_result,
    ])

def check_results(p) :
    global correct
    global total
    para = 0
    tolerance = '1'
    with open(p + 'spec.txt','r') as f :
        para = f.read().split()
    try :
        res = subprocess.check_output(['bin/checker_print_err.out',
                                       left_result,
                                       p + 'displeft.pgm',
                                       tolerance,
                                       para[1],])
        points = []
        # print res
        res = res.split()
        if len(res) > 1 :
	        for str in res :
	        	index = str.split(',')
	        	points.append(index[0]+' '+index[1])
    except CalledProcessError :
        print ('null')


for dataset in DATASETS :
	pic_names = subprocess.check_output(['ls', 'testdata/'+dataset+'/']).split()
	# if (dataset == 'Middlebury_others') :
	# 	widthpos = 1
	# 	heightpos = 2
	# else :
	# 	widthpos = 5
	# 	heightpos = 6
	for picture in pic_names :
		thepath = 'testdata/'+dataset+'/'+picture+'/'
		print "~~~~"+picture+"~~~~"
		print "gen points"
		left_result= 'bin/' + alg_to_gen_points + '_' + picture + '_left.pgm'
		right_result='bin/' + alg_to_gen_points + '_' + picture + '_right.pgm'
		print left_result
		run_test_with_path(thepath, alg_to_gen_points)
		check_results(thepath)
		picked = []
    	if len(points) > 0 :
	    	for i in range(0,picknum) :
	    		picked.append(points[random.randint(0,len(points)-1)])
		for alg_name in ALGORITHMS :
			print "draw stable "+alg_name
			left_result = 'bin/stable/'+picture+'_left_'+alg_name+'.pgm'
			right_result = 'bin/stable/'+picture+'_right_'+alg_name+'.pgm'
			support_map = 'bin/stable/'+ picture + '_left_support_map'
			results = print_stable(thepath, alg_name)
			# results = print_stable(thepath, alg_name, x, y)



		# with open(thepath+'left.ppm', 'r') as f:
		# 	para = f.read().split()
		# x = [random.randint(5,string.atoi(para[widthpos],10)-5) for i in range(0,5)]
		# y = [random.randint(5,string.atoi(para[heightpos],10)-5) for i in range(0,5)]

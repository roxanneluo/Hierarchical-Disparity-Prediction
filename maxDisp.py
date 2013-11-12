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

print 'all files:\n'
for dataset in DATASETS :
 	pic_names = subprocess.check_output(['ls', 'testdata/' + dataset + '/']).split()
	for picture in pic_names :
		#print '\''+picture+'\';'
		para = 0
		with open('testdata/'+dataset+'/'+picture+'/' + 'spec.txt','r') as f :
        		para = f.read().split()
		print para[0]+','
#		thepath = 'testdata/' + dataset + '/' + picture + '/'
#		print thepath + '\n'
#		print thepath + 'left.pgm\n'
#		print thepath + 'right.pgm\n'

print '\nDone.'	 

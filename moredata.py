import os
import sys
import subprocess

import httplib

NOW_PATH = ""
other_path = 'testdata/Middlebury_others/'

pic_names = subprocess.check_output(['ls', other_path]).split()

site = "vision.middlebury.edu"
url_1 = "/stereo/data/scenes2006/FullSize/"
url_2 = "/dmin.txt"

def get_disp(who):
    conn = httplib.HTTPConnection(site)
    conn.request('GET', url_1 + who + url_2)
    return conn.getresponse().read()
    
for pic in pic_names :
    t = int(get_disp(pic))
    size = 3
    with open(other_path + pic + '/spec.txt', 'w') as f :
        f.write(str(t / size) + '\n' + str(size))



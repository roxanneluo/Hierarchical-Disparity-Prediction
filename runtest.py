import os
import sys
import subprocess

import httplib

NOW_PATH = ""
other_path = 'testdata/Middlebury_others/'

pic_names = subprocess.check_output(['ls', other_path]).split()


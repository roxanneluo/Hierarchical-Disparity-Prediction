import sys
import subprocess

block_size = sys.argv[1]
filename = sys.argv[2]

with open(filename, 'r') as f: 
  line  = f.readline()[:-1]
  while line :
    for i in range(int(block_size)) :
      ratio = f.readline().split()[i]
      line += '\t' + ratio
    print line
    line = f.readline()[:-1]

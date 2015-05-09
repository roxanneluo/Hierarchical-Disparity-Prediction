#######
# crawl data from the middlebury
######

urltemp = 'http://vision.middlebury.edu/stereo/data/scenes2014/zip/{}'
tag1='<a href="{}">'

s = open('names','r').read().splitlines()
import urllib

for k in s:
    a = k[:k.find('.zip')+4]
    print a
    #if a < 'Playro' : continue
    url = urltemp.format(a)
    #print tag1.format(url) + a + '</a><br/>\n'
    urllib.urlretrieve(url, a)
    
    

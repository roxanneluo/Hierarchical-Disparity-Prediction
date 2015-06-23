import sys
import subprocess as sp
import numpy as np
from datetime import datetime
import os

#############################
# enumerate pixel_intval_threshold and tree_interval_threshold and obtain its
# time and accuracy and output a summary report
#############################

######### arg def ###########
pixel_intv_thresholds = [str(0.001 * (4**i)) for i in range(-5,5)]
tree_intv_thresholds = [str(x) for x in np.arange(0.5,1,0.05)]
print pixel_intv_thresholds
print tree_intv_thresholds
######### arg def ###########

tolerance_map = {'halfsize':'1', 'fullsize': '2'}
datasets = ['halfsize', 'fullsize']
dataset_num = len(datasets)
algo_names = ['dp_mst','dp_st','dp_rt']
algo_num = len(algo_names)
p_num = len(pixel_intv_thresholds)
t_num = len(tree_intv_thresholds)
print 'algo_num = %d pixel_itv_threshold_num = %d tree_itv_threshold_num = %d' \
  % (algo_num, p_num, t_num)
result_folder = 'results/pixel_tree_intv_threshold_sensitivity/'
sp.check_output(['mkdir','-p',result_folder])



def get_time(algo_names, log_filename) :
  avg_time = []
  for algo in algo_names:
    log_time = sp.check_output(['python', 'util/get_time_from_log.py',
      log_filename, algo]).split()
    log_time = [float(t) for t in log_time]
    avg_time.append(np.mean(log_time))
  return avg_time

def get_accuracy(algo_names, html_filename):
  html_accuracy = []
  for algo in algo_names:
    line = sp.check_output(['grep', 
      '<tr><td>' + algo + '</td>', html_filename]).split('\n')[0]
    a =  line.split('</td><td>')[1].split('</td>')[0]
    html_accuracy.append(a)
  return html_accuracy


def write_table(f, caption, table) :
  f.write('<table border="3">\n')
  f.write('<caption>' + caption + '</caption>\n')

  f.write('<tr><th>tree\pixel</th>')
  for pt in pixel_intv_thresholds:
    f.write('<th>'+pt+'</th>')
  f.write('</tr>')

  for ti in range(t_num):
    tt = tree_intv_thresholds[ti]
    f.write('<tr><th>' + tt +'</th>')
    for pi in range(p_num):
      f.write('<td>' + str(table[ti][pi]) + '</td>')
    f.write('</tr>')
  f.write('</table>\n\n')

def print_html(html_filename, dataset, time, accuracy) :
  print html_filename
  f = open(html_filename, 'w') 
  f.write('<h1>' + dataset + '</h1>\n')
  for ai in range(algo_num):
    algo = algo_names[ai]
    f.write('<h2>' + algo + '</h2>\n')
    write_table(f, 'Time', time[ai][:][:])
    write_table(f, 'Error Rate', accuracy[ai][:][:])
  f.close()

def cout(dataset, time, accuracy) :
  print '~~~~~~~~~~~~~~~~' + dataset + '~~~~~~~~~~~~~~~~~~'
  for ai in range(algo_num) :
    print '====' + algo_names[ai] + '====='
    print 'Error Rate'
    for ti in range(t_num):
      print ', '.join([str(x) for x in accuracy[ai][ti]])
    print 'Time'
    for ti in range(t_num):
      print ', '.join([str(x) for x in time[ai][ti]])

def print_data(data_filename, time, accuracy) :
  with open(data_filename, 'w') as f:
    f.write('%d\n'%algo_num)
    f.write( ' '.join(algo_names)+'\n')
    f.write( ' '.join(pixel_intv_thresholds)+'\n')
    f.write( ' '.join(tree_intv_thresholds)+'\n')
    f.write( '%d %d %d' % (t_num, p_num, algo_num)+'\n')
    f.write( 'Error Rate\n')
    for ai in range(algo_num):
      for ti in range(t_num):
        f.write( ' '.join([str(x) for x in accuracy[ai][ti][:]])+'\n')
    f.write( 'Time\n')
    for ai in range(algo_num):
      for ti in range(t_num):
        f.write( ' '.join([str(x) for x in time[ai][ti][:]])+'\n')

arr_size = (algo_num, t_num, p_num)
accuracy = np.zeros(arr_size)
time = np.zeros(arr_size)

for dataset in datasets:
  prefix = 'pixel_tree_intv_threshold'
  html_filename = result_folder + 'Summary_' + dataset + '_' + prefix + '.html'
  data_filename = result_folder + 'Data_' + dataset + '.txt'
  print html_filename
  for ti,tt in zip(range(t_num), tree_intv_thresholds):
    for pi, pt in zip(range(p_num), pixel_intv_thresholds):
      print 'tree_threshold %s\tpixel_threshold %s' % (tt, pt)
      super_report_filename = result_folder + 'SuperReport_' + prefix \
          + '_err_ge_' + tolerance_map[dataset] + dataset \
          + '_pixelIntvThreshold' +  pt\
          + '_treeIntvThreshold' + tt+ '.html'
      log_filename = result_folder + 'log_' + prefix \
          + '_err_ge_' + tolerance_map[dataset] + dataset \
          + '_pixelIntvThreshold' +  pt\
          + '_treeIntvThreshold' + tt+ '.txt'
      print log_filename
      if not (os.path.exists(log_filename) and
          os.path.exists(super_report_filename)):
        super_run = sp.Popen(('python', '-u',
            'super_run_nonocc_pixel_tree_threshold.py', tolerance_map[dataset],
            result_folder, prefix, pt, tt, dataset), stdout = sp.PIPE)
        results = sp.check_output(['tee', log_filename], stdin = super_run.stdout)
        print results
      # time update
      avg_time = get_time(algo_names, log_filename) 
      print avg_time
      for ai in range(algo_num):
        time[ai][ti][pi] = avg_time[ai]
      # accuracy update
      avg_accuracy = get_accuracy(algo_names, super_report_filename)
      print avg_accuracy
      for ai in range(algo_num):
        accuracy[ai][ti][pi] = avg_accuracy[ai]
  print_data(data_filename, time, accuracy)
  print_html(html_filename, dataset, time, accuracy)


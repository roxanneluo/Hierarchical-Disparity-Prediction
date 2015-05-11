function concur = readConcur(dataset, small, large)
%%{
  filename_format= ['/Users/roxanne/repos/stereo-cake/results/stat/' ...
            'halfsize/concur_mean_2015-04-21-20:34:49/'...
            'concur_%s_dpf-mst_%d,%d.txt'];
%%}
%{
    filename_format= ['/Users/roxanne/repos/stereo-cake/results/stat/' ...
            'fullsize/concur_mean_2015-04-21-18:50:09/'...
            'concur_%s_dpf-mst_%d,%d.txt'];
%}        
%{
  % exp for GMM
  filename_format= ['/Users/roxanne/repos/stereo-cake/results/stat/' ...
            'fullsize/concur_mean_2015-04-19-19:43:19/'...
            'concur_%s_dpf-mst_%d,%d.txt'];
%}
  
  
  filename = sprintf(filename_format, dataset, small, large);
  file = fopen(filename,'r');
  h = fscanf(file, '%d', [1,1]);
  w = fscanf(file, ' %d\n', [1,1]);
  assert(floor((w-1)/2+1) == h);
  concur = fscanf(file, '%d ', [w, h])';
  fclose(file);
end

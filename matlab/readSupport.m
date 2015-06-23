function sp = readSupport(dataset, layer, toDistr)
%{
  filename_format= ['/Users/roxanne/repos/stereo-cake/results/stat/' ...
            'halfsize/support_mean_2015-04-21-20:34:49/'...
            'support_%s_dpf-mst_%d.txt'];
%}
%%{

  filename_format= ['/Users/roxanne/repos/stereo-cake/results/stat/' ...
            'fullsize/support_mean_2015-04-21-18:50:09/'...
            'support_%s_dpf-mst_%d.txt'];
%%}
%{
  % exp  for GMM  
  filename_format= ['/Users/roxanne/repos/stereo-cake/results/stat/' ...
            'fullsize/support_mean_2015-04-19-19:43:19/'...
            'support_%s_dpf-mst_%d.txt'];
  %}
%{
  filename_format= ['/Users/roxanne/repos/stereo-cake/results/stat/' ...
            'kitti/support_dp_mst_2015-05-27-22:21:29/'...
            'support_%s_dp_mst_%d.txt'];
%}          
  filename = sprintf(filename_format, dataset, layer);
  file = fopen(filename, 'r');
  len = fscanf(file, '%d\n',[1,1]);
  sp = fscanf(file, '%d ', [1, inf]);
  fclose(file);
  assert(len == length(sp));
  if toDistr
    sp = sp ./ sum(sp);
  end
end

function pixel_tree_intv_analysis
  folder = '../../results/matlab/pic/';
  mkdir(folder);
  folder = [folder 'pixel_tree_intv_thresholds_analysis/'];
  mkdir(folder);

  datasets = {'halfsize','fullsize'};
  for di = 1:length(datasets)
    dataset = datasets{di};
    [algo_names,pixel_thresholds,tree_thresholds,accuracy, time] = ...
      read_data(['../../results/pixel_tree_intv_threshold_sensitivity/Data_', ...
        dataset,'.txt']);
    %%{
    draw_all(dataset, 'Error Rate', '%', accuracy*100, algo_names, pixel_thresholds,...
      tree_thresholds, folder);
    draw_all(dataset, 'Time','s', time, algo_names, pixel_thresholds,...
      tree_thresholds, folder);
        %%}
    get_best(dataset, accuracy, time, algo_names, pixel_thresholds, ...
        tree_thresholds);
       
  end
end

function get_best(dataset, accuracy, time, algo_names, pixel_thresholds, ...
        tree_thresholds)
  sum_accuracy = sum(accuracy,3);
  sum_time = sum(time,3);
  obj = 100*sum_accuracy' + log2(sum_time');
  [m,yi] = min(obj);
  [mm, xi] = min(m);
  yi = yi(xi);
  for ai=1:length(algo_names)
    fprintf('%s-%s: pt=%f, tt=%f, obj = %f, accuracy = %f, time = %f\n', ...
      dataset,...
    algo_names{ai}, pixel_thresholds(xi),tree_thresholds(yi), ...
      mm, accuracy(xi,yi,ai), time(xi,yi,ai));
  end
end

function draw_all(dataset, name, unit, table, algo_names, pixel_thresholds,...
  tree_thresholds, folder)
  for ai = 1:length(algo_names)
    figure();
    [X, Y] = meshgrid(pixel_thresholds, tree_thresholds);
    surf(X, Y, table(:,:,ai)');
    view(-5,45);
    xlim([min(pixel_thresholds), max(pixel_thresholds)]);
    ylim([min(tree_thresholds), max(tree_thresholds)]);
    %xlabel('pixel_intv_threshold'); 
    %ylabel('tree_intv_threshold');
    label_fontsize = 15;
    xlabel('\alpha_0','fontsize',label_fontsize); 
    ylabel('\beta','fontsize',label_fontsize); 
    zlabel([name '(' unit ')'],'fontsize',label_fontsize);
    pixel_threshold_labels = cell(1,length(pixel_thresholds));
    
    set(gca,'xtick',pixel_thresholds);
    for i=1:length(pixel_thresholds)
      if pixel_thresholds(i) <= 0.001
        pixel_threshold_labels{i} = sprintf('%.3g', pixel_thresholds(i));
      else
        pixel_threshold_labels{i} = sprintf('%.3f', pixel_thresholds(i));
      end
    end
    set(gca,'xticklabel',pixel_threshold_labels);
    
    set(gca,'ytick',tree_thresholds);
    %shading interp;
    %set(gcf, 'renderer','opengl');
    set(gca, 'xscale','log');
    grid minor
    set(gca,'XMinorTick','off');set(gca,'yMinorTick','off');
    title_str = [strrep(name,' ','-') '-' dataset '-' algo_names{ai}];
    saveas(gcf, [folder, title_str, '.png']);
    saveas(gcf, [folder, title_str, '.fig']);
    title(title_str);
  end
end

function [algo_names,pixel_thresholds,tree_thresholds,accuracy, time] ...
            = read_data(filename)
  f = fopen(filename);
  algo_num = fscanf(f, '%d\n',[1,1]);
  algo_names = {};
  for i=1:algo_num
    algo_names{end+1} = strrep(fscanf(f,'%s',[1,1]),'_','-');
  end
  fgetl(f);
  pixel_thresholds = sscanf(fgetl(f),'%f ',[1,inf]);
  tree_thresholds = sscanf(fgetl(f),'%f ',[1,inf]);
  m = fscanf(f, '%d ',[1,1]);
  n = fscanf(f, '%d ',[1,1]);
  algo_num = fscanf(f, '%d\n',[1,1]);
  assert(strcmp('Error Rate', fgetl(f)));
  accuracy = reshape(fscanf(f, '%f ', [n,m*algo_num]),[n,m,algo_num]);
  assert(strcmp('Time', fgetl(f)));
  time = reshape(fscanf(f, '%f ', [n,m*algo_num]),[n,m,algo_num]);
  fclose(f);
end

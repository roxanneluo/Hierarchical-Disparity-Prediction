function drawErrVsTimeForPaper
  half = [
    4.9472   20.5847
    1.0756   17.7583
    4.8881   19.7771
    0.8095   17.4295
    5.0843   18.0586
    1.1603   17.1894];
  full = [
   41.4357   13.6171
    4.1315   10.1913
   42.7926   13.4818
    3.1803   10.2503
   43.5902   14.0958
    4.9137   10.7799];
  names = {'MST', 'DP+MST','ST', 'DP+ST','RT','DP+RT'};
  ylabels = {'err>=1(%)', 'err>1(%)'};
  cmap = jet(length(names));
  dataset = {half, full};
  dataset_names = {'fullsize','halfsize'};
  folder_name = '~/repos/iccv2015/pic/err vs time/';
  mkdir pic
  mkdir(folder_name)
  for d = 1:length(dataset)
     figure();
     data = dataset{d};
     for i = 1:length(names)
       scatter(data(i,1), data(i,2), 50, cmap(i,:), 'o','markerfacecolor', cmap(i,:)); hold on
     end
     ylabel(ylabels{d});
     xlabel('time(s)');
     legend(names, 'location','northwest');
     set(gcf,'position',[1 1 400 300]);
     set(gca,'position',[0.12,0.14,0.85,0.83]);
     grid on
     saveas(gcf, [folder_name, dataset_names{d}],'fig');
     saveas(gcf, [folder_name, dataset_names{d}],'epsc');
  end
end
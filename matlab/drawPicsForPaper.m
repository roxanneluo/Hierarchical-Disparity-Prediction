function [ratio, interval, err] = drawPicsForPaper(HTMLNameSpec,...
                                    full_or_half, test,draw, save,...
                                    level, datasetRatio)
    path = sprintf('../results/matlab/pic/%s/', full_or_half);
    mkdir(path)
    mkdir([path 'sgl'])
    mkdir([path 'lgs'])
    mkdir([path 'PixelIntv'])
    mkdir([path 'concur'])
    
    if strcmp(full_or_half,'fullsize')
      prec = 0.064;
    else
      prec = 0.004;
    end
    pixelIntvOnly = false;
    times = 1;
    dataset = getDataSet(test, full_or_half);
    contract = 2;
    dataSetSize = length(dataset);
    maxRange = 1;
    para = 0.01*5; % so far no use, just a place holder
    ratio = zeros(level, level,maxRange, dataSetSize);
    err = zeros(level, level,maxRange, dataSetSize);
    interval = zeros(level,level,maxRange,dataSetSize);
    for t = 1:times
        for i = 1:length(dataset)
            for range = 1:maxRange
                [r,e,itv] = generateMultiLayer(dataset{i}{1},...
                            dataset{i}{2}, level, ...
                            contract, para(range),... %para no use now
                            draw,prec, datasetRatio,...
                            pixelIntvOnly,save, full_or_half,...
                            path);
                ratio(:, :, range,i) = ratio(:,:,range,i)+r;
                err(:,:,range,i) = err(:,:,range,i)+e;
                interval(:,:,range,i) = interval(:,:,range,i)+itv;
            end
        end
    end
    for i = 1:dataSetSize
       for range = 1:maxRange
          ratio(:,:,range,i) = ratio(:,:,range,i)/times;
          err(:,:,range,i) = err(:,:,range,i)/times;
          interval(:,:,range,i) = interval(:,:,range,i)/times;
       end
    end
    
    HTMLNameSpec = sprintf('%s %s prec=%f,datasetRatio=%f',...
      HTMLNameSpec, full_or_half, prec, datasetRatio);
   
    genRatioHTMLReport(ratio, dataset, level, para, 'Uncover Ratio',HTMLNameSpec,'sigma');
    genRatioHTMLReport(err, dataset, level, para, 'E[err]',HTMLNameSpec,'sigma');
    genRatioHTMLReport(interval, dataset, level, para, 'Interval',HTMLNameSpec,'sigma');
    
%     sprintf('avg interval ratio:%f',getAvgIntervalRatio())
% 
%     for d = 1:dataSetSize
%         drawRatio_Para(ratio, 0.01:0.01:0.1, 2,1,d,name);
%     end
    ratio = mean(ratio,4); ratio = mean(ratio,3);
    interval = mean(interval,4); interval = mean(interval,3);
    err = mean(err,4); err = mean(err,3);
    for i = 2:level
       for j = 1:i-1
           sprintf('%d-%d: ratio=%f,intervalRatio=%f,E[err]=%f\t',i-1,j-1,ratio(i,j),interval(i,j),err(i,j))
       end
    end
    ['done~']
end


function [ratio,err,avgInterval]= generateMultiLayer(dataSetName, ...
                                        maxDisp, level, con, range,...
                                        drawData,prec,...
                                        datasetRatio,pixelIntvOnly,...
                                        save, full_or_half, path)
    %     m is the range of disp+1 in layer i
    m = floor(maxDisp./con.^(0:level-1));
    
    ratio = zeros(level, level);
    err = zeros(level,level);
    avgInterval = zeros(level, level);
    for large = 1:level-1
        small = large+1;
        A = readConcur(dataSetName, small-1, large-1);
        trueSmallGivenLarge = findTrueSmallGivenLarge(A); 
        trueLargeGivenSmall = findTrueLargeGivenSmall(A);

        r = readSupport(dataSetName, large-1, true);
        smallGivenLarge = genSmallBaseSmallGivenLarge(small, large,...
          m(small), m(large), dataSetName, con, datasetRatio, full_or_half); 
          % TODO check
        largeGivenSmall = normalize(smallGivenLarge * diag(r));

        [ratio(small,large),intervalIX,wrongIX] = getWrongCoverRatio(largeGivenSmall',...
                                A',prec*(2^(large-1))); % ,totThreshold
        intervalIX = intervalIX'; wrongIX = wrongIX';
        err(small,large) = getExpectedErr(largeGivenSmall,...
                                trueLargeGivenSmall, ...
                                A);
        avgInterval(small,large) = getAvgInterval(A',intervalIX')/m(large);


        if drawData
            drawAll(A,...
                largeGivenSmall,...
                trueLargeGivenSmall,...
                trueSmallGivenLarge,...
                wrongIX,intervalIX,...
                small, large, dataSetName, range,prec, pixelIntvOnly,...
                save, path);
        end
    end
end

% ------------------------for generation-----------------------------------
function smallGivenLarge = genLargeBaseSmallGivenLarge(small, large, maxSmallDisp, maxLargeDisp, dataSetName)
    smallGivenLarge = readf(['data/gen 0.5/',dataSetName,'__small_given_large_matrix_',int2str(small-1),int2str(large-1),'_large_base.txt'],maxSmallDisp+1, maxLargeDisp+1);
end

function smallGivenLarge = genSmallBaseSmallGivenLarge(small, large,...
    maxSmallDisp, maxLargeDisp, dataSetName, contract,datasetRatio, ...
    full_or_half)  
    [mu, sigma, Pi, K] = getGMMPara(full_or_half, datasetRatio, large);
    distr = genNormrnd(-maxSmallDisp:maxSmallDisp, mu, Pi, sigma, K);
    smallGivenLarge = zeros(maxSmallDisp+1, maxLargeDisp+1);
    for j = 1:maxLargeDisp+1
        offset = maxSmallDisp-floor((j-1)/contract);
       for i = 1:maxSmallDisp+1
          smallGivenLarge(i,j) = distr(offset+i);
       end
    end
end

function y = genNormrnd(X,mu, Pi, sigma, K)
    y = zeros(size(X));
    for k = 1:K
        xx = (X-mu(k))/sigma(k);
        y = y+Pi(k)*exp(-xx.*xx/2)/(sqrt(2*pi)*sigma(k));
    end
end
% ----------------------------for calculation------------------------------
function normalized = normalize(A)
    l = sum(A,2);
    l = getInverse(l);
    normalized = diag(l) * A;
end
function err = getExpectedErr(largeGivenSmall, trueLargeGivenSmall, A)
    totalPoints = sum(sum(A));
    diff = abs(trueLargeGivenSmall-largeGivenSmall).*A;
    err = sum(sum(diff))/totalPoints;
end
% ------------------------------for drawing--------------------------------
function draw2d(x, y, string) 
%     figure();
    plot(x, y);title(string);
end
function drawTrueSGL(A, m,n, index, string, view2,save,folder)
    [small, large] = size(A);
    color = jet(large);
    for j=2:large
        offset = floor(j/2);
        plot(-offset:small-1-offset,A(:,j),'color',color(j,:)), hold on;
    end
    set(gcf,'position',[560,728,461,239]);
%     set(gca,'position',[0.04,-0.035,0.93,1.1]);
    set(gca,'outerposition',[0,0,1,1]);
    
    xlim([1-large,large-1]);
    set(gca,'fontsize',20,'xtick',1-large:10:large-1);
    shading interp;
    
    if save
       set(gcf, 'renderer','opengl');
       saveas(gcf,[folder, string,'.eps'],'epsc'); 
       saveas(gcf,[folder, string,'.fig']);
    end
end
function draw(A, m,n, index, string, view2,save,folder) 
    [small, large] = size(A);
    subplot(m,n, index);
    [X, Y] = meshgrid(0:large-1, 0:small-1);
    surf(X,Y, A,'linestyle','none');
    if view2
        view(2); axis equal;
%         set(gcf,'position',[560,728,461,239]);
%         set(gca,'position',[0.04,-0.035,0.93,1.1]);
        set(gcf,'position',[560,728,400,220]);
        set(gca,'position',[0.07,-0.035,0.88,1.2]);
        set(gca,'fontsize',15,'xtick',0:20:large-1,'ytick',0:20:small-1);
    else
        view(-10,45);
        set(gcf,'position',[560,728,500,340]);
        set(gca, 'position',[0.1,0.17,0.8,0.83]);
%         set(gcf,'position',[560,728,379,264]);
%         set(gca,'position',[0.044,0.10606060606060606,0.9164221635883905,0.8832775608849234]);
%         view([1,0,1]);
        m = min(diff(get(gca,'xLim')),diff(get(gca,'yLim')))/1.5;
        set(gca, 'DataAspectRatio', [m,m, diff(get(gca, 'ZLim'))])
        set(gca,'fontsize',15,'xtick',0:20:large-1,'ytick',0:20:small-1);
        caxis manual
        caxis([0,1])
    end
    xlim([0,large-1]);
    ylim([0,small-1]);
    
    shading interp;
    
    if save
       set(gcf, 'renderer','opengl');
       saveas(gcf,[folder, string,'.eps'],'epsc'); 
       saveas(gcf,[folder, string,'.fig']);
    end
end
function drawAll(A, largeGivenSmall, trueLargeGivenSmall, trueSmallGivenLarge,...
                 wrongIX,intervalIX, small, large, dataset, range,prec, pixelIntvOnly,...
                 save, path)
    disp(['draw ' dataset])
    small = small-1; large = large-1;
    err = abs(largeGivenSmall-trueLargeGivenSmall);
    wrongCover = err; wrongCover(~wrongIX)=0; 
    pixelIntv = largeGivenSmall;
    pixelIntv(~intervalIX)=-0.5;
    
    suffix = [int2str(small),int2str(large), 'Range',int2str(range)];
    %f = figure();
    %draw(A, 1,1,1, sprintf('%s-P(D%d,D%d)', dataset,small, large),...
     %   true, save,[path, 'concur/']);
%     figure(); 
%     draw(wrongCover, 1,1,1, [dataset, 'wrongCover',suffix], false);
%     figure();
%     draw(smallGivenLarge,1,1,1,[dataset,'smallGivenLarge', int2str(small), int2str(large)], true);
    if ~pixelIntvOnly
        %f = figure();
        filename = sprintf('%s-true-statistical-Pr(D%d-D%d)',dataset, large, small);
        draw(trueLargeGivenSmall,1,1,1,filename,false,...
            save,[path,'lgs/']);
        %f = figure(); 
        filename = sprintf('%s-Pr(D%d-D%d)',dataset, large, small);
        draw(largeGivenSmall,1,1,1,filename, false,...
             save,[path,'lgs/']);
        
        %f = figure();
        %filename = sprintf('%s-true-statistical-Pr(D%d-D%d)',dataset, small, large);
        %draw(trueSmallGivenLarge,1,1,1,filename, true,...
         %    save,[path,'sgl/']);
%         filename = sprintf('%s_true_statistical_Pr(D_%d-offset_D_%d)',dataset, small, large);
%         drawTrueSGL(trueSmallGivenLarge,1,1,1,filename, true,...
%              save,[path,'sgl/']);
        %}
    end
    %f = figure(); 
    %filename = sprintf('%s_PixelIntv_{%d}',dataset, large);
    draw(pixelIntv, 1,1,1, filename, true,...
         save,[path,'PixelIntv/']); 
%     figure();
%     draw(largeGivenSmall, 1,1,1, [dataset,' largeGivenSmall ',suffix],false);
%     figure();
%     draw(trueLargeGivenSmall, 1,1,1, [dataset,' trueLargeGivenSmall ',suffix],false);
%     figure();
%     draw(trueSmallGivenLarge, 1,1,1, [dataset,' smallGivenLarge ',suffix],false);
end
function drawRatio_Para(ratio, para, small, large, data, name)
    draw2d(para, squeeze(ratio(small, large,:,data)),[name{data}, int2str(small-1),int2str(large-1)]);
end

% -------------------------for HTML----------------------------------------
function genRatioHTMLReport(ratio, dataset, level, para, prefix, nameSpec,paraName) 
    avgEach = mean(ratio,4);
    avg = mean(avgEach,3);
    paraLen = size(para,2);
    dataSetSize = size(dataset,2);
    f = fopen(['../results/matlab/', prefix,' Report ',nameSpec,'.html'],'w');
    fprintf(f,'<!DOCTYPE html>\n<html>\n<body>\n');
    fprintf(f,['<h1>',prefix,' Report ',nameSpec,'</h1>\n']);
%     avg------------------------------------------------------------------
    fprintf(f,'<table border = "3">\n');
    fprintf(f,'<tbody>\n');
    fprintf(f,'<tr>\n<td rowspan="%d">Avg</td>\n</tr>\n', level);
    for i=2:level
    fprintf(f,'<tr>\n');
        for j=1:level-1
            fprintf(f,'<td>%f</td>\n',avg(i,j));
        end
    fprintf(f,'</tr>\n');
    end
    fprintf(f,'</tr>\n');
    fprintf(f,'</tbody>\n');
    fprintf(f,'</table>\n<br>\n');
    
%     avgEach--------------------------------------------------------------
    fprintf(f,'<table border = "3">\n');
    fprintf(f,'<thead>\n<tr>\n');
    fprintf(f,['<td>',paraName,'</td>\n']);
    for i=1:paraLen
        fprintf(f, '<td colspan="%d">%f</td>\n',level-1, para(i));
    end
    fprintf(f,'</tr>\n</thead>\n');
%     tbody
    fprintf(f,'<tbody>\n');
    fprintf(f,'<tr>\n<td rowspan="%d">Avg Each</td>\n</tr>\n', level);
    for i=2:level
        fprintf(f,'<tr>\n');
        for p=1:paraLen
            for j=1:level-1
                fprintf(f,'<td>%f</td>\n',avgEach(i,j,p));
            end
        end
        fprintf(f,'</tr>\n');
    end
    fprintf(f,'</tr>\n');
    fprintf(f,'</tbody>\n');
    fprintf(f,'</table>\n<br>\n');
    
%     all------------------------------------------------------------------
    fprintf(f,'<table border = "3">\n');
    fprintf(f,'<thead>\n<tr>\n');
    fprintf(f,['<td>',paraName,'</td>\n']);
    for i=1:paraLen
        fprintf(f, '<td colspan="%d">%f</td>\n',level-1, para(i));
    end
    fprintf(f,'</tr>\n</thead>\n');
%     tbody
    fprintf(f,'<tbody>\n');
    for d=1:dataSetSize
        if (rem(d,2)==0)
            color = '#e3e3e3';
        else color = 'white';
        end
        fprintf(f,[ '<tr style="background:',color,'">\n<td rowspan="%d">',char(dataset{d}(1)),'(%d)</td>\n</tr>\n'], level, cell2mat(dataset{d}(2)));
        for i=2:level
            fprintf(f,['<tr style="background:',color,'">\n']);
            for p=1:paraLen
                for j=1:level-1
                    fprintf(f,'<td>%f</td>\n',ratio(i,j,p,d));
                end
            end
            fprintf(f,'</tr>\n');
        end
        fprintf(f,'</tr>\n');
    end
    fprintf(f,'</tbody>\n');
    fprintf(f,'</table>\n');
    fprintf(f,'</body>\n</html>');
    fclose(f);
end

function findGMMDrawPicForPaper(full_or_half, K,dataSetRatio,times, width)
    scale = 2;
    level = 4;
    smallMax = 150;
    smallM = ones(level,1);
    smallM(2) = smallMax;
    for i = 3:level
       smallM(i) = floor(smallM(i-1)/scale); 
    end
    
    
    dataSet = getDataSet(false, full_or_half);
    N = size(dataSet,2);
    dataSet = dataSet(randsample(N,N));
    NRatio = max(size(dataSetRatio));
    
    prefix = sprintf('new %s GMM up to %d Kernels for %d times gnd vs. mst %s',...
        full_or_half, K,times, datestr(now)); 
    f = fopen(['../results/',prefix,'.html'],'w');
    printHTMLHead(f,prefix);
    for large = 1:level-1
%        for small = large+1:level
            small = large+1;
           contract = scale^(small-large);


           trueDistr = genDistr(small,large,smallM(small),contract,dataSet);

           figure();
           x = [-smallM(small):smallM(small)]';
           plot(x,trueDistr,'color',[1,0,1],'linewidth',width+0.2), hold on;

%            cnt = zeros(NRatio);
           KRatios = cell(K,1);
            
           cnt = 0;
           printHTMLTableHead(f,small,large,K,dataSetRatio);
           distr = zeros(2*smallM(small)+1,NRatio);
           for n = 1:NRatio
            distr(:,n) = genDistr(small,large,smallM(small),contract,dataSet(1:floor(N*dataSetRatio(n))));
           end
           
           maxp = 0;
           cmap = jet(4*K * NRatio);
           for k = 1:K
               mu = zeros(NRatio,k); Pi = zeros(NRatio,k); sigma = zeros(NRatio,k);
               KL=10 * ones(NRatio,1);
               for n = 1:NRatio
                   
                   found= false;
                   for t = 1:times
                        %try
                             [mm,pp,ss,kk] = findGMM(distr(:,n),smallM(small),k,trueDistr);
                             if (kk < KL(n))
                                mu(n,:) = mm; Pi(n,:) = pp; sigma(n,:) = ss;
                                KL(n) = kk;
                                found= true;
                             end
                        %{
                         catch ME
                            fprintf( 'Ill conditioned sigma\nin layer %d%d of %d gaussians dataSetRatio %1.1f\n',...
                            small-1,large-1,k,dataSetRatio(n));
                        end
                        %}
                   end
                   if found
                       cnt = cnt+1;
                       KRatios{k} = [KRatios{k},dataSetRatio(n)];
                       p = genNormrnd(x,mu(n,:),Pi(n,:),sigma(n,:),k);
                       drawGMM(x,p,small,large,k,KL(n),...
                          (k-1)*NRatio+n,width,max(trueDistr),scale,...
                          cmap);
                       maxp = max(max(p),maxp);
                   end
               end
               printHTML(f,mu,Pi,sigma,KL,K,NRatio);
           end
           fprintf(f,'</tbody>\n</table>\n');
            
            
            legendNames = cell(cnt+1,1);
            legendNames{1} = 'true';
           
            lgdcnt = 2;
            for k = 1:K
                for i = 1:max(size(KRatios{k}))
                   legendNames{lgdcnt}=sprintf('k_%d=%d, datasetRatio=%1.1f,',large-1,k,KRatios{k}(i)); 
                   lgdcnt = lgdcnt+1;
                end
            end
            legend(legendNames);
            set(gca,'position',[0.06,0.06,0.9,0.9]);
            ylim([0,max(maxp,max(trueDistr))+0.05]);
             folder = ['pic/GMM/' full_or_half '/'];
             mkdir folder;
            string = sprintf('GMM_%d_%d',large,scale);
             set(gcf,'position',[200,200,541,340]);
%               set(gcf, 'renderer','opengl');
              saveas(gcf,[folder, string,'.eps'],'epsc'); 
              saveas(gcf,[folder, string,'.fig']);
               
         end
%    end
    printHTMLTail(f);
    fclose(f);
end

% ---------------------for gen x,distr,normrnd for KL----------------------
function distr = genDistr(small,large,smallM,contract,dataSet)
    distr = zeros(2*smallM+1,1);
    dataSetSize = size(dataSet,2);
    for i = 1:dataSetSize
       dd = genDistrEach(dataSet{i}{1}, smallM, ... 
          floor(dataSet{i}{2}/contract^(small-1))+1 ,floor(dataSet{i}{2}/contract^(large-1))+1,small,large,contract);
       distr = distr+dd;
    end
    distr = distr/sum(distr);
end
function distr = genDistrEach(dataset,smallMAX,mSmall,mLarge, small, large,contract)
    A = readConcur(dataset, small-1, large-1);
    %figure()
    %surf(A)
    %title(dataset)
    distr = zeros(2*smallMAX+1,1); 
%     rem
%     contract = contract^(small-large);
    for j = 1:mLarge
        for i=1:mSmall
            d = i-1-floor((j-1)/contract);
            index = d+1+smallMAX;
            distr(index) = distr(index)+A(i,j);
        end
    end
    %{
    figure();
    plot(-smallMAX:smallMAX, distr);
    title(sprintf('%s%d,%d',dataset, small-1,large-1))
    %}
end

function [mu, Pi, sigma, KL] = findGMM(distr,smallM,k,trueDistr) 
%    gmm = gmdistribution.fit([-smallM:smallM]',distr,k);
    gmm = ...    
    gmdistribution.fit([-smallM:smallM]',distr,k,'start','randSample','regularize',0.1);
    mu = gmm.mu';
    Pi = gmm.PComponents;
    sigma = squeeze(gmm.Sigma)';

    X = [-smallM:smallM]';
    p = genNormrnd(X,mu,Pi,sigma,k);
    KL = KLDiv(p,trueDistr);
end

function y = genNormrnd(X,mu, Pi, sigma, K)
    y = zeros(size(X));
    for k = 1:K
        xx = X-mu(k);
        y = y+Pi(k)*exp(-xx.*xx/(2*sigma(k)))/(sqrt(2*pi*sigma(k)));
    end
end
% ------------------------------for file IO--------------------------------
function A = read(filename, m, n)
    A = zeros(m, n);
    file = fopen(filename, 'r');
    A = fscanf(file, '%d ', [n,m])';
    %{
    for i = 1:m
        A(i,:) = fscanf(file, '%d ',[1, n]);
        fscanf(file, '\n');
    end
    %}
    fclose(file);
end
% ------------------------------forHTML------------------------------------
function printHTMLHead(f,prefix)
    fprintf(f,'<!DOCTYPE html>\n<html>\n<body>\n');
    fprintf(f,['<h1>',prefix,'</h1>\n']);
end
function printHTMLTail(f)
    fprintf(f,'</body>\n</html>\n');
end
function printHTMLTableHead(f,small,large,K, dataSetRatio)
    NRatio = max(size(dataSetRatio));
    fprintf(f,'<h2>d%d|d%d</h2>',small-1,large-1);
    fprintf(f,'<table border = "3">\n');
    fprintf(f,'<thead><tr><td>dataset Ratio</td>');
    for i = 1:NRatio
       fprintf(f,'<td colspan="%d">%1.1f</td>',2+K,dataSetRatio(i)); 
    end
    fprintf(f,'</tr>');
    fprintf(f,'<tr><td>K</td>');
    for i = 1:NRatio
        fprintf(f,'<td>K</td>');
        for k = 1:K
           fprintf(f,'<td>%d</td>',k); 
        end
        fprintf(f,'<td>KL-div</td>');
    end
    fprintf(f,'</tr>');
end
function printDispRowHead(f,n,dispStart,dispEnd,K)
    if (rem(n,2)==0)
        color = '#d3d3d3';
    else
        color = 'white';
    end
    fprintf(f,[ '<tr style="background:',color,'">\n<td rowspan="%d">%d-%d</td>\n</tr>\n'], 4*K+1,dispStart,dispEnd); 
end
function printHTML(f,mu,Pi,sigma,KL,K,NRatio)
    k = size(mu,2);
    if (rem(k,2)==0)
        color = '#e3e3e3';
    else
        color = 'white';
    end
    fprintf(f,'<tr style="background:%s"><td rowspan="4">%d</td></tr>\n',color,k);
    fprintf(f,'<tr style="background:%s">\n',color);
    
    for i = 1:NRatio
        printPara(f,'mu',mu(i,:),K);
        fprintf(f,'<td rowspan="3">%f</td>\n',KL(i));
    end
    fprintf(f,'</tr>\n');
    paraName = {'sigma';'Pi'};para={sqrt(sigma);Pi};
    for i=1:2
        fprintf(f,'<tr style="background:%s">\n', color);
       for j=1:NRatio
           printPara(f,paraName{i},para{i}(j,:),K);
       end
       fprintf(f,'</tr>\n');
    end
end

function printPara(f,name,para,K)
    k = size(para,2);
    fprintf(f,'<td>%s</td>\n',name);
    for i=1:k
       fprintf(f,'<td>%f</td>\n',para(i)); 
    end
    for i=k+1:K
       fprintf(f,'<td></td>\n'); 
    end
end
% ------------------------------for drawing--------------------------------
function drawGMM(X,p,small,large,k,KL,colorIndex,width,maxTrue,scale,cmap)
     N = size(cmap,1);
     plot(X,p,'color',cmap(rem((colorIndex-1)*4+1,N)+1,:),'linewidth',width),hold on;
     
%      ylim([0,max(maxTrue,max(p))+0.05]);
%      title(sprintf('GMM_%d',large-1));
    
     hold on;
end
%-------------------------------data set-----------------------------------

function lIndex  = getlIndex(maxDisp,step)
    bound = 0;
    N = size(maxDisp,2);
    lIndex=[];
    for i=1:N
       if bound < maxDisp(i)
          bound = maxDisp(i)-rem(maxDisp(i),step)+step;
          lIndex = [lIndex;i];
       end
    end
end

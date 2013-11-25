function findGMMAllWithRem(K, step,rem, times)
    level = 3;
    contract = 2;
    smallMax = 60;
    smallM = ones(level,1);
    smallM(2) = smallMax;
    for i = 3:level
       smallM(i) = floor(smallM(i-1)/contract); 
    end
    name = getNames();
    maxDisp = getMaxDisp();
    [maxDisp,IX] = sort(maxDisp);
%     maxDisp
    name = name(IX);
    dataSetSize = size(name,1);
    lIndex  = getlIndex(maxDisp,step);
    N = size(lIndex,1);
    
    prefix = ['GMM up to ',int2str(K),' gaussians with step ',int2str(step),' rem ',int2str(rem),' for ',int2str(times),' times'];
    f = fopen([prefix,'.html'],'w');
    printHTMLHead(f,prefix);
    for large = 1:level
       for small = large+1:level
            printHTMLTableHead(f,small,large,K);
            for n = 1:N 
                if (n == N)
                    rIndex = dataSetSize;
                else
                    rIndex = lIndex(n+1)-1;
                end
                printDispRowHead(f,n,maxDisp(lIndex(n)),maxDisp(rIndex),K);
                for k = 1:K
                    mu = zeros(1,k); Pi = zeros(1,k);sigma = zeros(1,k);
                    KL = 1;
                    distr = genDistr(small,large,rem,smallM(small),contract,name(lIndex(n):rIndex),maxDisp(lIndex(n):rIndex));
                    for t = 1:times
                        try
                       [mm,pp,ss,kk] = findGMM(distr,smallM(small),k);
                        if (kk < KL)
                            mu = mm; Pi = pp; sigma = ss;
                            KL = kk;
                        end
                        catch ME
                            fprintf( 'Ill conditioned sigma\nin layer %d%d of %d gaussians with rem %d in disp range [%d,%d]\n',...
                            small-1,large-1,k,rem,maxDisp(lIndex(n)),maxDisp(rIndex));
                        end
                    end
                    x = [-smallM(small):smallM(small)]';
                    p = genNormrnd(x,mu,Pi,sigma,k);
%                         drawGMM(x,p,distr,small,large,rem,k,KL,maxDisp(lIndex(n)),maxDisp(rIndex));
                    printHTML(f,mu,Pi,sigma,KL,K);
               end
            end
            fprintf(f,'</tbody>\n</table>\n');
        end
    end
    printHTMLTail(f);
    fclose(f);
end

% ---------------------for gen x,distr,normrnd for KL----------------------
function distr = genDistr(small,large,rem,smallM,contract,name,maxDisp)
    distr = zeros(2*smallM+1,1);
    dataSetSize = size(name,1);
    for i = 1:dataSetSize
       dd = genDistrEach(['data/Prob_Gen_',name{i},'__'], smallM, floor(maxDisp(i)/contract^(small-1))+1 ,floor(maxDisp(i)/contract^(large-1))+1,small,large, rem,contract);
       distr = distr+dd;
    end
    distr = distr/sum(distr);
end
function distr = genDistrEach(dataset,smallMAX,mSmall,mLarge, small, large,rem,contract)
    A = read([dataset,'MST_cnt_',int2str(small-1),'_',int2str(large-1),'.txt'], mSmall, mLarge);
    distr = zeros(2*smallMAX+1,1); 
%     rem=1;
    contract = contract^(small-large);
    for jj = 1:contract:mLarge
        j = jj+rem;
        if j>mLarge
            break 
        end
        for i=1:mSmall
            d = i-1-floor((j-1)/contract);
            index = d+1+smallMAX;
            distr(index) = distr(index)+A(i,j);
        end
    end
end

function [mu, Pi, sigma, KL] = findGMM(distr,smallM,k)   
    gmm = gmdistribution.fit([-smallM:smallM]',distr,k);
    mu = gmm.mu';
    Pi = gmm.PComponents;
    sigma = squeeze(gmm.Sigma)';

    X = [-smallM:smallM]';
    p = genNormrnd(X,mu,Pi,sigma,k);
    KL = KLDiv(p,distr);
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
    for i = 1:m
        A(i,:) = fscanf(file, '%d ',[1, n]);
        fscanf(file, '\n');
    end
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
function printHTMLTableHead(f,small,large,K)
    fprintf(f,['<h2>d',int2str(small-1),'|d',int2str(large-1),' small base</h2>\n']);
    fprintf(f,'<table border = "3">\n');
    fprintf(f,'<thead>\n<tr><td rowspan=2>disp range</td></tr>\n');
    fprintf(f,'<tr>\n<td>K</td>\n');
    fprintf(f,'<td></td>\n');
    for k=1:K
       fprintf(f,'<td>%d</td>\n',k); 
    end
    fprintf(f,'<td>KL-div</td>\n'); 
    fprintf(f,'</tr>\n</thead>\n');
%     tbody
    fprintf(f,'<tbody>\n');
end
function printDispRowHead(f,n,dispStart,dispEnd,K)
    if (rem(n,2)==0)
        color = '#d3d3d3';
    else
        color = 'white';
    end
    fprintf(f,[ '<tr style="background:',color,'">\n<td rowspan="%d">%d-%d</td>\n</tr>\n'], 4*K+1,dispStart,dispEnd); 
end
function printHTML(f,mu,Pi,sigma,KL,K)
    k = size(mu,2);
    if (rem(k,2)==0)
        color = '#e3e3e3';
    else
        color = 'white';
    end
    fprintf(f,'<tr style="background:%s"><td rowspan="4">%d</td></tr>\n',color,k);
    fprintf(f,'<tr style="background:%s">\n',color);
    printPara(f,'mu',mu,K);
    fprintf(f,'<td rowspan="3">%f</td>\n',KL);
    fprintf(f,'</tr>\n');
    paraName = {'sigma';'Pi'};para={sqrt(sigma);Pi};
    for i=1:2
        fprintf(f,'<tr style="background:%s">\n', color);
        printPara(f,paraName{i},para{i},K);
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
function drawGMM(X,p,distr,small,large,rem,k,KL, dispStart,dispEnd)
     figure();
     plot(X,distr,'b');hold on;
     plot(X,p,'r');
     title(['GMM ',int2str(small-1),int2str(large-1),' rem=',int2str(rem),...
         ',k=',int2str(k),' ',int2str(dispStart),'-',int2str(dispEnd),' (red one is generated)']);
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
function name = getNames()
%     name = {'cones';'teddy';'Baby1';'Cloth1'; 'Cloth2'; 'Rocks2';'Midd1';'Midd2';'Monopoly'};
%     name = {'Aloe'};
%     name = {'Midd1';'Midd2';'Monopoly'};
%     name = {'Baby1';'Cloth1'; 'Cloth2'; 'Rocks2';'Midd1';'Midd2';'Monopoly'};
%     name = {'Plastic'};
    name = {'cones';'teddy';...
            'Aloe';'Baby1';'Baby2';'Baby3';'Bowling1';'Bowling2';'Cloth1';...
            'Cloth2';'Cloth3';'Cloth4';'Flowerpots';'Lampshade1';...
            'Lampshade2';'Midd1';'Midd2';'Monopoly';'Plastic';'Rocks1';...
            'Rocks2';'Wood1';'Wood2'};
%     name = {'cones';'teddy';'tsukuba';'venus';...
%             'Aloe';'Baby1';'Baby2';'Baby3';'Bowling1';'Bowling2';'Cloth1';...
%             'Cloth2';'Cloth3';'Cloth4';'Flowerpots';'Lampshade1';...
%             'Lampshade2';'Midd1';'Midd2';'Monopoly';'Plastic';'Rocks1';...
%             'Rocks2';'Wood1';'Wood2'};
end

function disp = getMaxDisp()
%     disp = [60,60,100, 96, 86, 91, 65, 71, 79];
%     disp = [90];
%     disp = [65, 71, 79];
%     disp = 93;
%     disp = [100, 96, 86, 91, 65, 71, 79];
    disp = [60,60,...
            90,100,100,83,96,80,96,...
            86,96,86,83,86,...
            86,65,71,79,93,91,...
            91,70,84];
%     disp = [60,60,16,20,...
%             90,100,100,83,96,80,96,...
%             86,96,86,83,86,...
%             86,65,71,79,93,91,...
%             91,70,84];
end

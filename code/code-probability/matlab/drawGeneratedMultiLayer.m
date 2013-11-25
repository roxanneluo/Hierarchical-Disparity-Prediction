function drawGeneratedMultiLayer
    name = getNames();
    dataSetSize = size(name, 1);
    maxDisp = getMaxDisp();
    [maxDisp,IX] = sort(maxDisp);
%     maxDisp
    name = name(IX);
    level = 3;
    contract = 2;
    maxRange = 10;
    ratio = zeros(level, level,maxRange, dataSetSize);
    err = zeros(level, level,maxRange, dataSetSize);
    times = 1;
    for t = 1:times
        for i = 1:dataSetSize
            for range = 1:maxRange
                [r,e] = generateMultiLayer(['data/Prob_Gen_',name{i},'__'], name{i}, maxDisp(i), level, contract, range*0.01, 1);
                ratio(:, :, range,i) = ratio(:,:,range,i)+r;
                err(:,:,range,i) = err(:,:,range,i)+e;
            end
        end
    end
    for i = 1:dataSetSize
       for range = 1:maxRange
          ratio(:,:,range,i) = ratio(:,:,range,i)/times;
          err(:,:,range,i) = err(:,:,range,i)/times;
       end
    end
    genRatioHTMLReport(ratio, name, maxDisp, level, 0.01:0.01:0.1, 'Uncover Ratio',' small base 0.5, large base with no threshold, normrnd(0,sigma)','sigma');
    genRatioHTMLReport(err, name, maxDisp, level, 0.01:0.01:0.1, 'E[err]',' small base 0.5, large base with no threshold, normrnd(0,sigma)','sigma');
    
% 
%     for d = 1:dataSetSize
%         drawRatio_Para(ratio, 0.01:0.01:0.1, 2,1,d,name);
%     end
end


function [ratio,err]= generateMultiLayer(dataset, dataSetName, maxDisp, level, con, range, smallBase)
    %     m is the range of disp+1 in layer i
    m = zeros(1,level);
    for i = 1:level
       m(i) = floor(maxDisp/con^(i-1)+1);
    end
    A = zeros(m(2), m(1),level, level);
    for i = 1:level
        for j = i+1:level
            A(1:m(j), 1:m(i),j,i) = read([dataset,'MST_cnt_',int2str(j-1),'_',int2str(i-1),'.txt'], m(j), m(i));
        end
    end

    smallGivenLarge = zeros(m(2), m(1), level, level);
   	trueLargeGivenSmall = zeros(m(2), m(1), level, level);
    for large = 1:level
        for small = large+1:level
            contract = con^(small-large);
            if smallBase
                smallGivenLarge(1:m(small), 1:m(large), small, large) = genSmallBaseSmallGivenLarge(small, large, m(small)-1, m(large)-1, dataSetName, contract);
            else 
                smallGivenLarge(1:m(small), 1:m(large), small, large) = genLargeBaseSmallGivenLarge(small, large, m(small)-1, m(large)-1, dataSetName);
            end
            trueLargeGivenSmall(1:m(small), 1:m(large), small, large) = findTrueLargeGivenSmall(A(1:m(small), 1:m(large), small, large));
        end
    end

   
    l = getL(m, level, A);
    r = getR(m, level, dataset, smallGivenLarge, range);
    
    ratio = zeros(level, level);
    err = zeros(level,level);
    for large = 1:level
        for small = large+1:level
            if (small == large+1)
                base = rMul(smallGivenLarge(1:m(small), 1:m(large), small, large),r(1:m(large),large));
            else
                base = smallGivenLarge(1:m(small), 1:m(small-1), small, small-1)*base;
            end
            largeGivenSmall = lMul(getInverse(l(1:m(small),small)), base);
            largeGivenSmall = normalize(largeGivenSmall);
%             largeGivenSmall(largeGivenSmall>1) = 1;
            ratio(small,large) = getWrongCoverRatio(largeGivenSmall,...
                                    trueLargeGivenSmall(1:m(small), 1:m(large), small, large), ...
                                    A(1:m(small), 1:m(large), small, large));
            err(small,large) = getExpectedErr(largeGivenSmall,...
                                    trueLargeGivenSmall(1:m(small), 1:m(large), small, large), ...
                                    A(1:m(small), 1:m(large), small, large));
%             drawAll(A(1:m(small), 1:m(large), small, large),...
%                 largeGivenSmall,...
%                 trueLargeGivenSmall(1:m(small), 1:m(large), small, large),...
%                 smallGivenLarge(1:m(small), 1:m(large), small, large),...
%                 small, large, dataset, range);
            
%             if smallBase
%                 smallBase = genSmallBase(dataSetName, 2,1);
%                 draw2d(-m(small)+1:m(small)-1,smallBase, [dataSetName,'(',int2str(m(2)),')small base']);
%             else
%                 largeBase = genLargeBase(dataSetName,2,1);
%                 draw2d(-m(large)+1:m(large)-1,largeBase, [dataSetName, '(',int2str(m(1)),') large base']);
%             end
        end
    end
end


% function SL = genSmallGivenLarge(smallDisp, largeDisp, range)
%     SL = zeros(smallDisp+1, largeDisp+1);
%     contract = largeDisp/smallDisp;
%     for j = 0:largeDisp
%         if (rem(j,contract)==0)
%             b = max(0,j/contract-range);
%             t = min(smallDisp, j/contract+range);
%             cnt = t-b+1;
%         else
%             b = max(0, floor(j/contract)-range);
%             t = min(smallDisp, ceil(j/contract)+range);
%             cnt = t-b+1;
%         end
%         for i = b:t
%             SL(i+1,j+1) = 1/cnt;
%         end
%     end
% end

% ------------------------for generation-----------------------------------
function smallGivenLarge = genLargeBaseSmallGivenLarge(small, large, maxSmallDisp, maxLargeDisp, dataSetName)
    smallGivenLarge = readf(['data/gen 0.5/',dataSetName,'__small_given_large_matrix_',int2str(small-1),int2str(large-1),'_large_base.txt'],maxSmallDisp+1, maxLargeDisp+1);
end
% function smallGivenLarge = genSmallBaseSmallGivenLarge(small, large, maxSmallDisp, maxLargeDisp, dataSetName)
%     smallGivenLarge = readf(['data/gen 0.5/',dataSetName,'__small_given_large_matrix_',int2str(small-1),int2str(large-1),'_small_base.txt'],maxSmallDisp+1, maxLargeDisp+1);
% end
function smallGivenLarge = genSmallBaseSmallGivenLarge(small, large, maxSmallDisp, maxLargeDisp, dataSetName, contract)
    mu = [0.2071;-2.2668];
    Pi = [0.7719;0.2281];
    sigma = [0.4155;92.1834];
    K = 2;
    distr = genNormrnd(-maxSmallDisp:maxSmallDisp, mu, Pi, sigma, K);
    smallGivenLarge = zeros(maxSmallDisp+1, maxLargeDisp+1);
    for j = 1:maxLargeDisp+1
        offset = maxSmallDisp-floor((j-1)/contract);
       for i = 1:maxSmallDisp+1
          smallGivenLarge(i,j) = smallGivenLarge(i,j)+distr(offset+i);
       end
    end
end

% function smallGivenLarge = genSmallBaseSmallGivenLarge(small, large, maxSmallDisp, maxLargeDisp, dataSetName, contract)  
%     distr = cell(2,1);
%     mu = [0.006599;-2.1005880];
%     Pi = [0.752007;0.247993 ];
%     sigma = [0.559044;9.554292];
%     sigma = sigma.*sigma;
%     K = 2;
%     distr{1} = genNormrnd(-maxSmallDisp:maxSmallDisp, mu, Pi, sigma, K);
%     mu = [-4.350168;1.356980;0.498251];
%     sigma = [10.557543;3.396443;0.578839];
%     sigma = sigma.*sigma;
%     Pi = [0.141961;0.100777;0.757262];
%     K = 3;
% %     mu = [-2.277375;0.500922];
% %     Pi = [0.217550;0.782450];
% %     sigma = [9.269545;0.600388];
% %     sigma = sigma.*sigma;
% %     K = 2;
%     distr{2} = genNormrnd(-maxSmallDisp:maxSmallDisp, mu, Pi, sigma, K);
%     smallGivenLarge = zeros(maxSmallDisp+1, maxLargeDisp+1);
%     for j = 1:maxLargeDisp+1
%         offset = maxSmallDisp-floor((j-1)/contract);
%         if rem(j-1,contract)==0
%             d = distr{1};
%         else
%             d = distr{2}; 
%         end
%        for i = 1:maxSmallDisp+1
%           smallGivenLarge(i,j) = smallGivenLarge(i,j)+d(offset+i);
%        end
%     end
% end

function y = genNormrnd(X,mu, Pi, sigma, K)
    y = zeros(size(X));
    for k = 1:K
        xx = X-mu(k);
        y = y+Pi(k)*exp(-xx.*xx/(2*sigma(k)))/(sqrt(2*pi*sigma(k)));
    end
end
function base = genLargeBase(dataSetName, small, large)
%     base = readPd(['data/',dataSetName,'__avg_prob_large_base.txt']);
    base = readPd(['data/gen 0.5/',dataSetName,'__avg_prob_',int2str(small-1), int2str(large-1),'_large_base.txt']);
end
function base = genSmallBase(dataSetName, small, large)
%     base0 = readPd(['data/',dataSetName,'__avg_prob_small_base_even',int2str(small-1), int2str(large-1),'.txt']);
%     base1 = readPd(['data/',dataSetName,'__avg_prob_small_base_odd',int2str(small-1), int2str(large-1),'.txt']);
%     base = [base0,base1];
    base = readPd(['data/gen 0.5/',dataSetName,'__avg_prob_',int2str(small-1), int2str(large-1),'_small_base.txt']);
end
% ------------------------for file read------------------------------------
function l = readPd(filename) 
    file = fopen(filename,'r');
    l = fscanf(file, '%f\n',[1,inf]);
    fclose(file);
    l = l'; 
end
function l = readSupport(filename, sigma) 
    file = fopen(filename,'r');
    l = fscanf(file, '%d\n',[1,inf]);
    fclose(file);
    l = l'; l = l/sum(l);
    len = size(l,1);
    for i=1:len
        if l(i)==0
            noise = normrnd(0,sigma);
            if (noise > 0)
                l(i) = l(i)+noise;
            end
        end
    end
    l = l/sum(l);
end

function A = read(filename, m, n)
    A = zeros(m, n);
    file = fopen(filename, 'r');
    for i = 1:m
        A(i,:) = fscanf(file, '%d ',[1, n]);
        fscanf(file, '\n');
    end
    fclose(file);
end
function A = readf(filename, m, n)
    A = zeros(m, n);
    file = fopen(filename, 'r');
    for i = 1:m
        A(i,:) = fscanf(file, '%f ',[1, n]);
        fscanf(file, '\n');
    end
    fclose(file);
end
% ----------------------------for calculation------------------------------
function normalized = normalize(A)
    n = size(A,2);
    l = sum(A,2);
    l = getInverse(l);
    normalized = A.*(l*ones(1,n));
end
function ratio = getWrongCoverRatio(largeGivenSmall, trueLargeGivenSmall, A)
    totalPoints = sum(sum(A));
    wrongPoints = A((largeGivenSmall==0) & (trueLargeGivenSmall ~= 0));
    ratio = sum(wrongPoints)/totalPoints;
end
function err = getExpectedErr(largeGivenSmall, trueLargeGivenSmall, A)
    totalPoints = sum(sum(A));
    diff = abs(trueLargeGivenSmall-largeGivenSmall).*A;
    err = sum(sum(diff))/totalPoints;
end
function r = getR(m, level, dataset, smallGivenLarge, sigma)
    r = zeros(m(1), level);
    r(1:m(1),1) = readSupport([dataset,'support_disp_cnt.txt'], sigma);
    for i = 2:level-1
        r(1:m(i),i) = smallGivenLarge(1:m(i),1:m(i-1), i,i-1)*r(1:m(i-1),i-1);
    end
end
function l = getL(m ,level, A)
    l = zeros(m(1), level);
%     for i = 1: level
%         l(1:m(i),i) = readPd([dataset, 'MST__pd_',int2str(i-1),'.txt']);
%     end  
%     l(1:m(1),1) = genR(A(1:m(2), 1:m(1), 2,1));
     for i = 2: level
        l(1:m(i),i) = genL(A(1:m(i), 1:m(i-1), i,i-1));
%         draw2d(trueL, ['trueL', int2str(i)]);
%         draw2d(l(1:m(i),i), ['genL', int2str(i)]);
%         draw2d(l(1:m(i),i)-trueL, ['errL', int2str(i)]);
    end
end
function r = genR(A)
    r = sum(A);
    s = sum(r);
    r = r'/s;
end

function l = genL(A)
    l = sum(A, 2);
    s = sum(l);
    l = l/s;
end
function Y = getInverse(L)
    [m, n] = size(L);
    Y = zeros(m,n);
    if n < m 
        n = m;
    end
    for i = 1:n
        if L(i) >= 0.0000001
            Y(i) = 1/L(i);
        else Y(i) = 0;
        end
    end
end

% function smallGivenLarge = findSmallGivenLarge(A)
%    [small, large] = size(A);
%     L = sum(A);
%     L = getInverse(L);
%     smallGivenLarge = A.*(ones(small,1)*L);
% end

function largeGivenSmall = findTrueLargeGivenSmall(A)
    [small, large] = size(A);
    s = sum(A, 2);
    s = getInverse(s);
    largeGivenSmall = A.*(s*ones(1,large));
end
function product = lMul(l, A) 
    n = size(A,2);
    product = A.*(l*ones(1,n));
end
function product = rMul(A, r)
    r = r';
    m = size(A,1);
    product = A.*(ones(m,1)*r);
end
% ------------------------------for drawing--------------------------------
function draw2d(x, y, string) 
    figure();
    plot(x, y);title(string);
end

function draw(A, m,n, index, string, equal) 
    [small, large] = size(A);
    subplot(m,n, index);
    [X, Y] = meshgrid(0:large-1, 0:small-1);
    if ~equal
        surf(X,Y, A), shading interp; colorbar; title(string); 
    else
         surf(X,Y, A), shading interp; colorbar; title(string); 
    end
end
function drawAll(A, largeGivenSmall, trueLargeGivenSmall, smallGivenLarge, small, large, dataset, range)
    small = small-1; large = large-1;
    err = largeGivenSmall-trueLargeGivenSmall;
    
    wrongCover = err;
%     wrongCover(largeGivenSmall<trueLargeGivenSmall & trueLargeGivenSmall>0) = -1;
    wrongCover(largeGivenSmall<trueLargeGivenSmall & largeGivenSmall>0) = ...
        -wrongCover(largeGivenSmall<trueLargeGivenSmall & largeGivenSmall>0);
        
    f = figure();
    [dataset, int2str(f)]
%     draw(A, 1,1,1, ['cnt', int2str(small),int2str(large)], false);
%     figure();
    draw(smallGivenLarge, 4,2,1, [dataset,'smallGivenLarge',int2str(small),int2str(large), 'Range',int2str(range)], false);
    draw(smallGivenLarge, 4,2,2, [dataset,'smallGivenLarge',int2str(small),int2str(large), 'Range',int2str(range)],false);
    draw(trueLargeGivenSmall, 4,2,3, [dataset,'largeGivenSmall',int2str(small),int2str(large), 'Range',int2str(range)],false);
    draw(trueLargeGivenSmall, 4,2,4, [dataset,'largeGivenSmall',int2str(small),int2str(large), 'Range',int2str(range)],false);
    draw(largeGivenSmall, 4,2,5, [dataset,'calculatedLargeGivenSmall',int2str(small),int2str(large), 'Range',int2str(range)],false);
    draw(largeGivenSmall, 4,2,6, [dataset,'calculatedLargeGivenSmall',int2str(small),int2str(large), 'Range',int2str(range)],false);
    draw(err, 4,2,7, [dataset,'error',int2str(small),int2str(large), 'Range', int2str(range)],false);
    draw(err, 4,2,8, [dataset,'error',int2str(small),int2str(large), 'Range',int2str(range)], false);
    figure(); 
    draw(wrongCover, 1,1,1, [dataset, 'wrongCover',int2str(small),int2str(large), 'Range', int2str(range)], false);
%     figure();
%     draw(smallGivenLarge,1,1,1,[dataset,'smallGivenLarge', int2str(small), int2str(large)], true);
end
function drawRatio_Para(ratio, para, small, large, data, name)
    draw2d(para, squeeze(ratio(small, large,:,data)),[name{data}, int2str(small-1),int2str(large-1)]);
end

% -------------------------for HTML----------------------------------------
function genRatioHTMLReport(ratio, name, maxDisp, level, para, prefix, nameSpec, paraName) 
    avgEach = mean(ratio,4);
    avg = mean(avgEach,3);
    paraLen = size(para,2);
    dataSetSize = size(name,1);
    f = fopen([prefix,' Report ',nameSpec,'.html'],'w');
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
        fprintf(f,[ '<tr style="background:',color,'">\n<td rowspan="%d">',name{d},'(%d)</td>\n</tr>\n'], level, maxDisp(d));
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
%-------------------------------data set-----------------------------------
function name = getNames()
%     name = {'Midd1';'Midd2';'Monopoly'};
%     name = {'Baby1';'Cloth1'; 'Cloth2'; 'Rocks2';'Midd1';'Midd2';'Monopoly'};
%     name = {'Plastic'};
    name = {'cones';'teddy';'tsukuba';'venus';...
            'Aloe';'Baby1';'Baby2';'Baby3';'Bowling1';'Bowling2';'Cloth1';...
            'Cloth2';'Cloth3';'Cloth4';'Flowerpots';'Lampshade1';...
            'Lampshade2';'Midd1';'Midd2';'Monopoly';'Plastic';'Rocks1';...
            'Rocks2';'Wood1';'Wood2'};
end

function disp = getMaxDisp()
% disp = [65, 71, 79];
%     disp = 93;
%     disp = [100, 96, 86, 91, 65, 71, 79];
    disp = [60,60,16,20,...
            90,100,100,83,96,80,96,...
            86,96,86,83,86,...
            86,65,71,79,93,91,...
            91,70,84];
end

function drawGeneratedMultiLayer
    name = {'cones';'teddy';'tsukuba';'venus'};
    maxDisp = [60, 60, 16, 20];
    level = 3;
    contract = 2;
    maxRange = 3;
    ratio = zeros(level, level,1, maxRange);
    for i = 1:1
        for range = 1:maxRange
            ratio(:, :, range,i) = generateMultiLayer(['data/Prob_Gen_',name{i},'__'], maxDisp(i), level, contract, range);
        end
    end
    ratio
end
function ratio = generateMultiLayer(dataset, maxDisp, level, contract, range)
    %     m is the range of disp+1 in layer i
    m = zeros(level);
    for i = 1:level
       m(i) = maxDisp/contract^(i-1)+1;
    end
    
    A = zeros(m(2), m(1),level, level);
    for i = 1:level
        for j = i+1:level
            A(1:m(j), 1:m(i),j,i) = read([dataset,'MST_cnt_',int2str(j-1),'_',int2str(i-1),'.txt'], m(j), m(i));
        end
    end
    
    l = getL(m, level, dataset, A);

    
    smallGivenLarge = zeros(m(2), m(1), level, level);
   	trueLargeGivenSmall = zeros(m(2), m(1), level, level);
    for large = 1:level
        for small = large+1:level
            smallGivenLarge(1:m(small), 1:m(large), small, large) = genSmallGivenLarge(m(small)-1, m(large)-1, range);
            trueLargeGivenSmall(1:m(small), 1:m(large), small, large) = findTrueLargeGivenSmall(A(1:m(small), 1:m(large), small, large));
        end
    end
    
    ratio = zeros(level, level);
    for large = 1:level
        for small = large+1:level
            if (small == large+1)
                base = rMul(smallGivenLarge(1:m(small), 1:m(large), small, large),l(1:m(large),large));
            else
                base = smallGivenLarge(1:m(small), 1:m(small-1), small, small-1)*base;
            end
            largeGivenSmall = lMul(getInverse(l(1:m(small),small)), base);
            largeGivenSmall(largeGivenSmall>1) = 1;
            ratio(small,large) = getWrongCoverRatio(largeGivenSmall,...
                                    trueLargeGivenSmall(1:m(small), 1:m(large), small, large), ...
                                    A(1:m(small), 1:m(large), small, large));
            drawAll(A(1:m(small), 1:m(large), small, large),...
                largeGivenSmall,...
                trueLargeGivenSmall(1:m(small), 1:m(large), small, large),...
                smallGivenLarge(1:m(small), 1:m(large), small, large),...
                small, large, dataset, range);
        end
    end
end

function ratio = getWrongCoverRatio(largeGivenSmall, trueLargeGivenSmall, A)
    totalPoints = sum(sum(A));
    wrongPoints = A((largeGivenSmall==0) & (trueLargeGivenSmall ~= 0));
    ratio = sum(wrongPoints)/totalPoints;
end
function SL = genSmallGivenLarge(smallDisp, largeDisp, range)
    SL = zeros(smallDisp+1, largeDisp+1);
    contract = largeDisp/smallDisp;
    for j = 0:largeDisp
        if (rem(j,contract)==0)
            b = max(0,j/contract-range);
            t = min(smallDisp, j/contract+range);
            cnt = t-b+1;
        else
            b = max(0, floor(j/contract)-range);
            t = min(smallDisp, ceil(j/contract)+range);
            cnt = t-b+1;
        end
        for i = b:t
            SL(i+1,j+1) = 1/cnt;
        end
    end
end
function l = getL(m ,level, dataset, A)
    l = zeros(m(1), level);
    l(1:m(1),1) = readSupport([dataset,'support_disp_cnt.txt']);
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
function product = lMul(l, A) 
    n = size(A,2);
    product = A.*(l*ones(1,n));
end
function product = rMul(A, r)
    r = r';
    m = size(A,1);
    product = A.*(ones(m,1)*r);
end

function l = readPd(filename) 
    file = fopen(filename,'r');
    l = fscanf(file, '%f\n',[1,inf]);
    fclose(file);
    l = l'; 
end
function l = readSupport(filename) 
    file = fopen(filename,'r');
    l = fscanf(file, '%d\n',[1,inf]);
    fclose(file);
    l = l'; l = l/sum(l);
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

function Y = getInverse(L)
    [m, n] = size(L);
    Y = zeros(m,n);
    if n < m 
        n = m;
    end
    for i = 1:n
        if L(i) >= 0.00001
            Y(i) = 1/L(i);
        else Y(i) = 0;
        end
    end
end

function smallGivenLarge = findSmallGivenLarge(A)
   [small, large] = size(A);
    L = sum(A);
    L = getInverse(L);
    smallGivenLarge = A.*(ones(small,1)*L);
end

function largeGivenSmall = findTrueLargeGivenSmall(A)
    [small, large] = size(A);
    s = sum(A, 2);
    s = getInverse(s);
    largeGivenSmall = A.*(s*ones(1,large));
end
function draw2d(l, string) 
    figure();
    m = size(l,1);
    plot(1:m, l); shading interp; colorbar; title(string);
end

function draw(A, m,n, index, string, equal) 
    [small, large] = size(A);
    subplot(m,n, index);
    [X, Y] = meshgrid(0:large-1, 0:small-1);
    if ~equal
        surf(X,Y, A), shading interp; colorbar; title(string); 
    else
         surf(X,Y, A), shading interp; colorbar; title(string); axis equal;
    end
end
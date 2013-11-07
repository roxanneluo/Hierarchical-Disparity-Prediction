function drawMultiLayer()
    dataset = '';
    maxDisp = 60;
    level = 2;
    contract = 2;
    %     m is the range of disp+1 in layer i
    m = zeros(level);
    for i = 1:level
       m(i) = maxDisp/contract^(i-1)+1;
    end
    
    A = zeros(m(2), m(1),level, level);
    for i = 1:level
        for j = i+1:level
            A(1:m(j), 1:m(i),j,i) = read([dataset,'MST__cnt_',int2str(j-1),'_',int2str(i-1),'.txt'], m(j), m(i));
        end
    end
    
    l = zeros(m(1), level);
    l(1:m(1),1) = readSupport([dataset,'support_disp_cnt.txt']);
    for i = 2: level
        l(1:m(i),i) = readPd([dataset, 'MST__pd_',int2str(i-1),'.txt']);
    end    
    
    smallGivenLarge = zeros(m(2), m(1), level, level);
   	trueLargeGivenSmall = zeros(m(2), m(1), level, level);
    for large = 1:level
        for small = large+1:level
            smallGivenLarge(1:m(small), 1:m(large), small, large) = findSmallGivenLarge(A(1:m(small), 1:m(large), small, large));
            trueLargeGivenSmall(1:m(small), 1:m(large), small, large) = findTrueLargeGivenSmall(A(1:m(small), 1:m(large), small, large));
        end
    end
    
    for large = 1:level
        for small = large+1:level
            if (small == large+1)
                base = rMul(smallGivenLarge(1:m(small), 1:m(large), small, large),l(1:m(large),large));
            else
                base = smallGivenLarge(1:m(small), 1:m(small-1), small, small-1)*base;
            end
            largeGivenSmall = lMul(getInverse(l(1:m(small),small)), base);
            drawAll(A(1:m(small), 1:m(large), small, large),largeGivenSmall, trueLargeGivenSmall(1:m(small), 1:m(large), small, large), smallGivenLarge(1:m(small), 1:m(large), small, large), small, large);
        end
    end
end

function drawAll(A, largeGivenSmall, trueLargeGivenSmall, smallGivenLarge, small, large)
    small = small-1; large = large-1;
    err = largeGivenSmall-trueLargeGivenSmall;
    figure();
    draw(A, 1,1,1, ['cnt', int2str(small),int2str(large)], false);
    figure();
    draw(smallGivenLarge, 4,2,1, ['smallGivenLarge',int2str(small),int2str(large)], false);
    draw(smallGivenLarge, 4,2,2, ['smallGivenLarge',int2str(small),int2str(large)],false);
    draw(trueLargeGivenSmall, 4,2,3, ['largeGivenSmall',int2str(small),int2str(large)],false);
    draw(trueLargeGivenSmall, 4,2,4, ['largeGivenSmall',int2str(small),int2str(large)],false);
    draw(largeGivenSmall, 4,2,5, ['calculatedLargeGivenSmall',int2str(small),int2str(large)],false);
    draw(largeGivenSmall, 4,2,6, ['calculatedLargeGivenSmall',int2str(small),int2str(large)],false);
    draw(err, 4,2,7, ['error',int2str(small),int2str(large)],false);
    draw(err, 4,2,8, ['error',int2str(small),int2str(large)],false);
    figure();
    draw(smallGivenLarge,1,1,1,['smallGivenLarge', int2str(small), int2str(large)], false);
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
    l = fscanf(file, '%f\n',[1,inf]);
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
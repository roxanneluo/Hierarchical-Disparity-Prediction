function findAvgSmallGivenLarge()
    name = {'cones';'teddy';'tsukuba';'venus'};
    maxDisp = [60, 60, 16, 20];
    level = 2;
    contract = 2;
    for i = 1:4
        find(['data/Prob_Gen_',name{i},'__'], maxDisp(i), level, contract);
    end
end
function find(dataset, maxDisp, level, contract)
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
    
    smallGivenLarge = zeros(m(2), m(1), level, level);
    for large = 1:level
        for small = large+1:level
            smallGivenLarge(1:m(small), 1:m(large), small, large) = findSmallGivenLarge(A(1:m(small), 1:m(large), small, large));
        end
    end
    
    avg = avgSmallGivenLarge(smallGivenLarge(1:m(2),1:m(1),2,1), level, maxDisp, dataset)
    draw2dMid(avg, maxDisp, [dataset,'avgSmallGivenLarge10']);
end

function draw2dMid(avg, maxDisp, string)
    figure();
    x = -maxDisp:maxDisp;
    plot(x,avg); title(string);
end
function avg = avgSmallGivenLarge(smallGivenLarge, level, maxDisp, dataset)
    %     level 10
    %   from -maxDisp to maxDisp, offset maxDisp+1
    avg = zeros(2*maxDisp+1,1);
    cnt = zeros(2*maxDisp+1, 1);
    for i = 0:maxDisp/2
        mid = 2*i;
        if (smallGivenLarge(i+1, mid+1)<1/maxDisp)
            continue;
        end
        for j = 0:maxDisp
            dis = j-mid;
            index = dis+maxDisp+1;
            avg(index) = avg(index)+smallGivenLarge(i+1,j+1);
            cnt(index) = cnt(index)+1;
        end
    end
    cnt = getInverse(cnt);
    avg = avg.*cnt;
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
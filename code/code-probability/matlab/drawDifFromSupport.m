function drawDifFromSupport()
    maxDisp = 60;
    A10 = read('dif_gnd_2.txt', maxDisp/2+1,maxDisp+1);
    A20 = read('dif_gnd_4.txt', maxDisp/4+1, maxDisp+1);
    A21 = read('dif_MST_4_2.txt', maxDisp/4+1, maxDisp/2+1);
    l = readSupport('support_disp_cnt.txt');
    
    smallGivenLarge10 = findSmallGivenLarge(A10);
    largeGivenSmall10 = findLargeGivenSmall(smallGivenLarge10,eye(maxDisp+1),l);
    
    smallGivenLarge21 = findSmallGivenLarge(A21);
    largeGivenSmall20 = findLargeGivenSmall(smallGivenLarge21,largeGivenSmall10,smallGivenLarge10*l);
    
    trueLargeGivenSmall20 = findTrueLargeGivenSmall(A20);
    err = largeGivenSmall-trueLargeGivenSmall20;
    draw(largeGivenSmall20,1,1,1,'20');
    draw(trueLargeGivenSmall20,1,1,1,'true20');
    draw(err, 1,1,1, 'err');
    
    
   
    
    smallGivenLarge = findSmallGivenLarge(A);
    s = smallGivenLarge*l; s = getInverse(s);    
    largeGivenSmall = smallGivenLarge.*(s*l');
    
    trueLargeGivenSmall = findLargeGivenSmall(A);
    Err = largeGivenSmall-trueLargeGivenSmall;
    
    m = 4; n=2;
    string = 'MSTLargeGivenSmall';
    draw(smallGivenLarge,1,1,1,'ErrGND');
%     draw(smallGivenLarge,m,n,1,'smallGivenLarge');
%     draw(smallGivenLarge,m,n,2,'smallGivenLarge');
%     draw(trueLargeGivenSmall,m,n,3,string);
%     draw(trueLargeGivenSmall,m,n,4,string);
%     draw(largeGivenSmall, m,n,5, 'largeGivenSmallCalculated');
%     draw(largeGivenSmall, m,n,6, 'largeGivenSmallCalculated');
%     draw(Err, m, n, 7, 'error');
%     draw(Err, m, n, 8, 'error');
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

function largeGivenSmall = findLargeGivenSmall(smallGivenLarge, prevLargeGivenSmall, l)
    s = smallGivenLarge*l; s = getInverse(s);    
    largeGivenSmall = smallGivenLarge*prevLargeGivenSmall.*(s*l');
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

function draw(A, m,n, index, string) 
    [small, large] = size(A);
    subplot(m,n, index);
    [X, Y] = meshgrid(0:large-1, 0:small-1);
    surf(X,Y, A), shading interp; colorbar; title(string);
end
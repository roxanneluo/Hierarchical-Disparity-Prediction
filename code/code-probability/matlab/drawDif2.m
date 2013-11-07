function drawDif2()
    file = fopen('dif_gnd_2.txt', 'r');
    A = fscanf(file, '%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n',[17, inf]);
    fclose(file);
    A = A';
    file = fopen('support_disp_cnt.txt','r');
    support = fscanf(file, '%d\n',[1,inf]);
    fclose(file);
    support = support'

    [small, large] = size(A);
    l = zeros(1,large);
    s = sum(A,2);
    s = s/sum(s);
    a = 0.5;
    l(1) = s(1)*a*1.5;
    l(2) = s(1)*a/2;
    l(large) = s(small)*a*1.5;
    l(large-1) = s(small)*a/2;
    for i=2:small-1
        l(2*i-1) = s(i)*a;
        l(2*i-2) = l(2*i-2)+s(i)*a/2;
        l(2*i) = l(2*i) + s(i)*a/2;
    end

    L = sum(A);L = L/sum(L);
    ss = zeros(small,1); ss(2:small-1) = ones(small-2,1)./s(2:small-1);
    draw(ss*l);
%     smallGivenLarge = A./(ones(small,1)*L);
%     largeGivenSmall = smallGivenLarge.*(ones(small,1)./s*l);
%     draw(largeGivenSmall);
%     
%     trueLargeGivenSmall = findLargeGivenSmall(A);
%     draw(trueLargeGivenSmall);
%     draw(largeGivenSmall-trueLargeGivenSmall);
end

function largeGivenSmall = findLargeGivenSmall(A)
    [small, large] = size(A);
    s = sum(A, 2);
    S = s*ones(1,large);
    largeGivenSmall = zeros(small, large);
    largeGivenSmall(2:small-1,:) = A(2:small-1,:)./S(2:small-1,:);
end

function draw(A) 
    [small, large] = size(A);
    figure();
    [X, Y] = meshgrid(0:large-1, 0:small-1);
    surf(X,Y, A), shading interp; colorbar; 
end
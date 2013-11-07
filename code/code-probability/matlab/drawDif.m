function drawDif()
    file = fopen('dif_MST.txt', 'r');
    A = fscanf(file, '%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n',[17, inf]);
    fclose(file);
    A = A';
    draw(A);
    [small, large] = size(A);
    l = sum(A);
    S = ones(small, 1)*l;
    smallGivenLarge = zeros(small, large);
    smallGivenLarge(:, 2:large-1) = A(:, 2:large-1)./S(:, 2:large-1);
    draw(smallGivenLarge);
    
    s = sum(A, 2);
    S = s*ones(1,large);
    largeGivenSmall = zeros(small, large);
    largeGivenSmall(2:small-1,:) = A(2:small-1,:)./S(2:small-1,:);
    draw(largeGivenSmall);
    
    draw(largeGivenSmall-0.25*smallGivenLarge);
    
    sumS = sum(s);
    s = s./sumS
    ss = ones(small, 1)./s;
    sumL = sum(l);
    l = l./sumL
    
    draw(ss(3:small)*l);
    
    draw(largeGivenSmall./(ss*ones(1, large)));
end

function draw(A) 
    [small, large] = size(A);
    figure();
    [X, Y] = meshgrid(0:large-1, 0:small-1);
    surf(X,Y, A), shading interp; colorbar; 
end
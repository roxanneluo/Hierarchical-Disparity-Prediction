function [mu,Pi,sigma] = findGMM(dataset, maxDisp, level, contract, K)
    m = zeros(1,level);
    for i = 1:level
       m(i) = floor(maxDisp/contract^(i-1)+1);
    end
    A = zeros(m(2), m(1),level, level);
    for i = 1:level
        for j = i+1:level
            A(1:m(j), 1:m(i),j,i) = read([dataset,'MST_cnt_',int2str(j-1),'_',int2str(i-1),'.txt'], m(j), m(i));
        end
    end
    x = genDataFromConCur(A, level, m);
    mu = zeros(K,level,level);
    Pi = zeros(K,level,level);
    sigma = zeros(K,level,level);
    for large = 1: level
       for small = large+1:level
          N = sum(sum(A(:,:,small,large)));
          gmm = gmdistribution.fit(x{small,large},K);
          mu(:,small,large) = gmm.mu;
          Pi(:,small,large) = gmm.PComponents;
          sigma(:,small,large) = squeeze(gmm.Sigma);
          drawGMM(gmm, m, small);
       end
    end
    mu
    Pi
    sigma
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
function x = genDataFromConCur(A, level, m)
    x = cell(level,level);
    for large = 1:level
       for small = large+1:level
           N = sum(sum(A(:,:,small,large)));
           x{small,large} = zeros(N,1);
           cnt = 0;
           for j = 1:m(large)
              for i=1:m(small)
                 for n = 1:A(i,j,small,large)
                    cnt = cnt+1;
                    x{small, large}(cnt) = i-1-floor((j-1)/2);
                 end
              end
           end
       end
    end
end

function drawGMM(gmm, m, small)
    K = gmm.NComponents;
    x = -m(small):m(small);
    y = zeros(1,2*m(small)+1);
    mu = gmm.mu;
    Pi = gmm.PComponents;
    sigma = squeeze(gmm.Sigma);
    for k = 1:K
        xx = x-mu(k);
        y = y+Pi(k)*exp(-xx.*xx/(2*sigma(k)))/(sqrt(2*pi*sigma(k)));
    end
    plot(x,y);
end
function x = genData(K,N)
    miu = 5*rand(1,K)
    Pi = randi(K,1,K);
    s = sum(Pi);
    sigma = 20*rand(1,K)
    x = zeros(N,1);
    for n = 1:N
        k = genK(Pi,s,K);
        x(n) = normrnd(miu(k),sqrt(sigma(k)));
    end
    Pi = Pi/s
end
function k = genK(Pi,s,K)
    n = randi(s)-1;
    ss = 0;
    for i=1:K
        ss = ss+Pi(i);
        if (n<ss)
            k = i;
            break;
        end
    end
end

function [r,miu,Pi,sigma] = GMM(x,K,err) 
    N = size(x,1);
    miu = zeros(1,K);
    sigma = 10*[1:K];
    Pi = ones(1,K)/K;
    lastL = 0;
    cnt = 0;
    while (1)
        cnt = cnt+1;
        B = getB(x,miu,sigma,Pi,K,N);
        r = getR(B,K);
        
        n = sum(r);
        Pi = n/N;
        miu = x'*r./n;
        sigma = getSigma(x,miu,n,r,K);
        l = getL(B);
        if (abs(lastL-l) < err) 
            cnt
            break;
        else
            lastL = l;
        end
    end
   B = getB(x,miu,sigma,Pi,K,N);
   r = getR(B,K);
end

function B = getB(x,miu,sigma,Pi,K,N)
    B = zeros(N,K);
    for k=1:K
        xx = x-miu(k);
        B(:,k) = Pi(k)*exp(-xx.*xx/(2*sigma(k)))/sqrt(2*pi)/sigma(k);
    end
end

function r = getR(B,K)
    s = sum(B,2);
    r = B./(s*ones(1,K));
end

function sigma = getSigma(x,miu,n,r,K)
    sigma = zeros(1,K);
    for k=1:K
        xx = x-miu(k);
        sigma(k) = r(:,k)'*(xx.*xx)/n(k);
    end
end

function l = getL(B)
    l = sum(log(sum(B,2)));
end


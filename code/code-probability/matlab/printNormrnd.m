function y = printNormrnd()
    X=-60:60;
    mu = [0.2071;-2.2668];
    Pi = [0.7719;0.2281];
    sigma = [0.4155;92.1834];
    K=2;
    y = zeros(size(X));
    for k = 1:K
        xx = X-mu(k);
        y = y+Pi(k)*exp(-xx.*xx/(2*sigma(k)))/(sqrt(2*pi*sigma(k)));
    end
    
    f = fopen('gmm base','w');
    for i=1:size(y,2)
        fprintf(f,'%f\n',y(i));
    end
    fclose(f);
end
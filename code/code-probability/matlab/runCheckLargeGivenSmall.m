function runCheckLargeGivenSmall()
    prec = 3;
    draw = false;
    HTMLNameSpec = ['  normrnd(0,sigma) ref check new gmm 1base +-1 mid interval 1e-',int2str(prec)];
    folderPrefix = 'TEST_1128/large_given_small_1_gmm';
    checkLargeGivenSmall(HTMLNameSpec,true,draw,10^(-prec),folderPrefix);
    folderPrefix = 'TEST_1128/large_given_small_2_gmm';
    HTMLNameSpec = ['  normrnd(0,sigma) ref check new gmm 2base +-1 mid interval 1e-',int2str(prec)];
    checkLargeGivenSmall(HTMLNameSpec,true,draw,10^(-prec),folderPrefix);
%     checkLargeGivenSmall(HTMLNameSpec,true,draw,10^(-prec),folderPrefix);
end
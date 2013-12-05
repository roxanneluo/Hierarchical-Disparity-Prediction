function runCheckLargeGivenSmall()
    prec = 3;
    draw = false;
    HTMLNameSpec = ['  normrnd(0,sigma) ref check new 1 gmm ratio gnd mid interval'];
    folderPrefix = 'TEST_1128/large_given_small_1_gmm';
    checkLargeGivenSmall(HTMLNameSpec,true,draw,3,folderPrefix);
%     folderPrefix = 'TEST_1128/large_given_small_2_gmm';
%     HTMLNameSpec = ['  normrnd(0,sigma) ref check new 2 gmm ratio interval mid interval'];
%     checkLargeGivenSmall(HTMLNameSpec,true,draw,3,folderPrefix);
end
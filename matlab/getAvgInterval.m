function expect = getAvgInterval(A,intervalIX)
    intervalLen = sum(intervalIX);
    num = sum(A);
    expect = sum(intervalLen.*num)/sum(num);
end

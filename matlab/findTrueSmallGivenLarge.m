function gtSmallGLarge = findTrueSmallGivenLarge(A)
    s = getInverse(sum(A));
    gtSmallGLarge = A*diag(s);
end


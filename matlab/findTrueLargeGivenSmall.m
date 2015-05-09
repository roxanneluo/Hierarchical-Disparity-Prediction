function LgS = findTrueLargeGivenSmall(concur)
    s = sum(concur, 2);
    s = getInverse(s);
    LgS = diag(s) * concur;
end

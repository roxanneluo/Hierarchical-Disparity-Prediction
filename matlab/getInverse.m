function Y = getInverse(L)
    Y = zeros(size(L));
    for i = 1:length(L)
        if abs(L(i)) >= 0.000001
            Y(i) = 1/L(i);
        else Y(i) = 0;
        end
    end
end

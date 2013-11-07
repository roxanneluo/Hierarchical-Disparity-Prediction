function run_all_4
    name = {'cones';'teddy';'tsukuba';'venus'};
    maxDisp = [60, 60, 16, 20];
    level = 2;
    contract = 2;
    for i = 1:4
        drawMultiLayer(['data/Prob_Gen_',name{i},'__'], maxDisp(i), level, contract);
    end
end
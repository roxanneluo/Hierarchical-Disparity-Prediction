function [mu, sigma, Pi, K] = getGMMPara(full_or_half, datasetRatio, large)
  switch full_or_half
  case 'halfsize'
    if datasetRatio == 0.5
      switch large
      case 1
        mu = [1.049875, 0.176738, -5.205266];
        Pi = [0.123876, 0.705882, 0.170241];
        sigma = [2.700242, 0.594091, 10.932382];
        K=3; return;
      case 2
        mu = [-0.838225, 0.174887];
        Pi = [0.218403, 0.781597];
        sigma = [5.652754, 0.543740];
        K = 2; return
      end
    elseif datasetRatio == 1
        switch large
        case 1
            mu = [0.187744, -4.049134, 0.990802];
            Pi = [0.727650, 0.161147, 0.111203];
            sigma = [0.606504, 11.087519, 2.913190];
            K = 3; return
        case 2
            mu = [-2.133305, 1.058262, 0.177772];
            Pi = [0.157178, 0.094915, 0.747907];
            sigma = [6.284264, 2.368275, 0.525036];
            K = 3; return
        end
    end
  case 'fullsize'
    if datasetRatio == 1
      switch large
      case 1
        mu = [32.502699, -34.328304, 18.462280, 0.370448];
        Pi = [0.146335, 0.134981, 0.015143, 0.703540];
        sigma = [25.730198, 21.352337, 3.652555, 0.632237];
        K = 4; return;
      case 2
        mu = [9.983536, -17.626809, 18.114333, 0.467670];
        Pi = [0.030189, 0.119504, 0.121258, 0.729049];
        sigma = [1.114562, 10.611028, 10.518979, 0.627145];
        K = 4; return
      case 3
        mu = [-8.839617, 4.968906, 9.112209, 0.516049];
        sigma = [4.935000, 0.535987, 4.373850, 0.651702];
        Pi = [0.064693, 0.046168, 0.111436, 0.777703];
        K = 4; return
      end
    elseif datasetRatio == 0.5
      switch large
      case 1
        mu = [-38.335031, 0.383928, 22.106222, 0.383928];
        sigma = [19.879390, 0.629609, 26.036771, 0.629609];
        Pi = [0.131236, 0.310989, 0.133314, 0.424460];
        K = 4; return
      case 2
        mu = [0.452982, -21.247403, 11.133451, 10.101522];
        sigma = [0.617757, 9.580585, 14.423702, 0.604170];
        Pi = [0.747561, 0.100479, 0.126690, 0.025270];
        K = 4; return
      case 3
        mu = [4.580204, 10.864041, -8.720563, 0.476397];
        sigma = [1.036329, 4.002217, 4.461282, 0.637942];
        Pi = [0.076437, 0.048733, 0.060785, 0.814045];
        K = 4; return
      end
    end
  end
  assert(false)
end
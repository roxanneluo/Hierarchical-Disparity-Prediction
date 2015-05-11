function [ratio,intervalIX,IX] = getWrongCoverRatio(predict, A, thresholdGiven)
  totalPoints = sum(sum(A));
  [H,W] = size(predict);
  IX = false(H,W);
  IX(1,:)=true;
  for j = 1:W
    MAX = -1;
    for i = 1:H
      if predict(i,j)>MAX
        mid = i;
        MAX = predict(i,j);
      end
    end
%    mid = 2*j-1;
    tot = predict(mid,j);
    IX(mid,j)=true;
    threshold = thresholdGiven;
    l = mid-1; r = mid+1;
    expanded = true;
    while expanded && (l > 0 || r <= H)
      expanded = false;
      if r <= H && dcmp(predict(r,j)/(tot+predict(r,j)), threshold) == 1
        expanded = true; tot = tot + predict(r,j);
        IX(r,j) = true;
        r = r+1;
      end
      if l > 0 && dcmp(predict(l,j)/(predict(l,j)+tot), threshold) == 1
        expanded = true; tot = tot + predict(l,j);
        IX(l,j) = true;
        l = l-1;
      end
    end
  end

  intervalIX = IX;
  IX = (A~=0) &~IX;
  wrongPoints = A(IX);
  ratio = sum(wrongPoints)/totalPoints;
end

function res = dcmp(a, b)
    if a-b < -1e-9 
        res = -1;
    elseif a-b > 1e-9
        res = 1;
    else
        res = 0;
    end
end
%{
function [ratio,intervalIX,IX] = getWrongCoverRatio(predict, A, thresholdGiven, totThreshold)
  totalPoints = sum(sum(A));
  [H,W] = size(predict);
  IX = false(H,W);
  IX(1,:)=true;
  for j = 1:W
    MAX = -1;
    for i = 1:H
      if predict(i,j)>MAX
        mid = i;
        MAX = predict(i,j);
      end
    end
%    mid = 2*j-1;
    tot = predict(mid,j);
    IX(mid,j)=true;
    threshold = thresholdGiven;
    l = mid-1; r = mid+1;
    while l > 0 || r <= H
      expanded = false;
      inc = 0;
      if r <= H && predict(r,j)/(tot+predict(r,j))>=threshold
        expanded = true; inc = inc + predict(r,j);
        IX(r,j) = true;
        r = r+1;
      end
      if l > 0 && predict(l,j)/(predict(l,j)+tot)>=threshold
        expanded = true; inc = inc + predict(l,j);
        IX(l,j) = true;
        l = l-1;
      end
      tot = tot + inc;
      if ~expanded 
        if tot > totThreshold
          break
        else
          if r > H || l > 0 && predict(l,j) >= predict(r,j)
            IX(l,j) = true; tot = tot+predict(l,j); l = l-1;
          else
            IX(r,j) = true; tot = tot + predict(r,j); r = r+1;
          end
        end
      end
    end
  end

  intervalIX = IX;
  IX = (A~=0) &~IX;
  wrongPoints = A(IX);
  ratio = sum(wrongPoints)/totalPoints;
end
%}

%{
function [ratio,intervalIX,IX] = getWrongCoverRatio(predict, A, thresholdGiven, totThreshold)
    totalPoints = sum(sum(A));
    [H,W] = size(predict);
    IX = false(H,W);
    IX(1,:)=true;
    for j = 1:W
        MAX = -1;
       for i = 1:H
           if predict(i,j)>MAX
               mid = i;
               MAX = predict(i,j);
               
           end
       end
 %}
%       mid = 2*j-1;
%{
       tot = predict(mid,j);
       expanded = false;
       IX(mid,j)=true;
%}
%        if predict(mid,j)<0.3
%            threshold = thresholdGiven/10;
%        else
%            threshold = thresholdGiven;
%        end
%{
        threshold = thresholdGiven;
       l = mid; r = mid;
       for i = 1:max(mid-1,H-mid)
           if ~rstop
               r = r+1;
               if r<=H && predict(r,j)/(tot+predict(r,j))>=threshold
                   tot = tot+predict(r,j);
                   IX(r,j)=true;
               else
                   rstop = true;
               end
           end
           
           if ~lstop
               l = l-1;
               if l>=1 && predict(l,j)/(predict(l,j)+tot)>=threshold
                   tot = tot+predict(l,j);
                   IX(l,j)=true;
               else
                   lstop = true;
               end
           end
           
           
           if lstop && rstop
%}
%{
                while tot+totThreshold<1 && (l>=1 || r<=H)
                   if l>=1 
                       tot = predict(l,j)+tot;
                       IX(l,j) = true;
                   end
                   if r<=H && tot+totThreshold<1
                       tot = predict(r,j)+tot;
                       IX(r,j) = true;
                   end
                   l = l-1; r = r+1;
                end
%}
%{
               break;
           end
       end
    end
%}
%     for j = 1:W
%         mid = 2*j-1;
%         tot = predict(mid,j);
%         IX(mid,j)=true;
%         for i = 1:max(mid-1,H-mid)
%             l = mid-i;
%             if 1-tot>threshold && l>=1
%                 tot = tot+predict(l,j);
%                 IX(l,j)=true;
%             end
%             
%             r = mid+i;
%             if 1-tot > threshold && r<=H
%                tot = tot+predict(r,j);
%                IX(r,j) = true;
%             end
%         end
%     end
%{
    intervalIX = IX;
    IX = (A~=0) &~IX;
    wrongPoints = A(IX);
    ratio = sum(wrongPoints)/totalPoints;
end
%}

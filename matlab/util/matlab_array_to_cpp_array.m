function cpp = matlab_array_to_cpp_array(m_array)
  [m n] = size(m_array);
  cpp = '';
  for i = 1:m 
    cpp = [cpp,'{'];
    for j = 1:n
      cpp = sprintf('%s%f, ', cpp, m_array(i, j));
    end
    cpp = sprintf('%s},\n', cpp(1:end-2));
  end
  cpp = cpp(1:end-2);
end

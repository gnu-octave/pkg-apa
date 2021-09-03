function mpfr_nexttoward (x, y)
  if (isa (x, 'mpfr_t'))
    x = x.idx;
  end
  if (isa (y, 'mpfr_t'))
    y = y.idx;
  end
  gmp_mpfr_interface (166, x, y);
end

% This function was automatically generated by "generate_m_files".

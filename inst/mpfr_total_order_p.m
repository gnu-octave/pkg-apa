function ret = mpfr_total_order_p (x, y)
  if (isa (x, 'mpfr_t'))
    x = x.idx;
  end
  if (isa (y, 'mpfr_t'))
    y = y.idx;
  end
  ret = gmp_mpfr_interface (82, x, y);
end

% This function was automatically generated by "generate_m_files".

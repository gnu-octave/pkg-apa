function ret = mpfr_const_catalan (rop, rnd)
  if (isa (rop, 'mpfr_t'))
    rop = rop.idx;
  end
  ret = gmp_mpfr_interface (142, rop, rnd);
end

% This function was automatically generated by "generate_m_files".

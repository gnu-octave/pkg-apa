function ret = mpfr_fits_intmax_p (op, rnd)
  if (isa (op, 'mpfr_t'))
    op = op.idx;
  end
  ret = gmp_mpfr_interface (30, op, rnd);
end

% This function was automatically generated by "generate_m_files".

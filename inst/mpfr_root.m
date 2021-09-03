function ret = mpfr_root (rop, op, n, rnd)
  if (isa (rop, 'mpfr_t'))
    rop = rop.idx;
  end
  if (isa (op, 'mpfr_t'))
    op = op.idx;
  end
  ret = gmp_mpfr_interface (47, rop, op, n, rnd);
end

% This function was automatically generated by "generate_m_files".

function ret = mpfr_frac (rop, op, rnd)
  if (isa (rop, 'mpfr_t'))
    rop = rop.idx;
  end
  if (isa (op, 'mpfr_t'))
    op = op.idx;
  end
  ret = gmp_mpfr_interface (154, rop, op, rnd);
end

% This function was automatically generated by "generate_m_files".

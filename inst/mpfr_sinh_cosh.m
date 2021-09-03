function ret = mpfr_sinh_cosh (sop, cop, op, rnd)
  if (isa (sop, 'mpfr_t'))
    sop = sop.idx;
  end
  if (isa (cop, 'mpfr_t'))
    cop = cop.idx;
  end
  if (isa (op, 'mpfr_t'))
    op = op.idx;
  end
  ret = gmp_mpfr_interface (112, sop, cop, op, rnd);
end

% This function was automatically generated by "generate_m_files".

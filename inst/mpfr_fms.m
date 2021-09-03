function ret = mpfr_fms (rop, op1, op2, op3, rnd)
  if (isa (rop, 'mpfr_t'))
    rop = rop.idx;
  end
  if (isa (op1, 'mpfr_t'))
    op1 = op1.idx;
  end
  if (isa (op2, 'mpfr_t'))
    op2 = op2.idx;
  end
  if (isa (op3, 'mpfr_t'))
    op3 = op3.idx;
  end
  ret = gmp_mpfr_interface (57, rop, op1, op2, op3, rnd);
end

% This function was automatically generated by "generate_m_files".

function ret = mpfr_cmp_d (op1, op2)
  if (isa (op1, 'mpfr_t'))
    op1 = op1.idx;
  end
  ret = gmp_mpfr_interface (64, op1, op2);
end

% This function was automatically generated by "generate_m_files".
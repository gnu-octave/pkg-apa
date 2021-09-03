function ret = mpfr_div_2ui (rop, op1, op2, rnd)
% ret = mpfr_div_2ui (rop, op1, op2, rnd)
%
% Set ROP to OP1 divided by 2 raised to OP2 rounded in the direction
% RND.  Just decreases the exponent by OP2 when ROP and OP1 are
% identical.
%

  if (isa (rop, 'mpfr_t'))
    rop = rop.idx;
  end
  if (isa (op1, 'mpfr_t'))
    op1 = op1.idx;
  end
  ret = gmp_mpfr_interface (53, rop, op1, op2, rnd);
end

% This function was automatically generated by "generate_m_files".

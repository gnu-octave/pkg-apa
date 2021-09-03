function ret = mpfr_dim (rop, op1, op2, rnd)
% ret = mpfr_dim (rop, op1, op2, rnd)
%
% Set ROP to the positive difference of OP1 and OP2, i.e., OP1 - OP2
% rounded in the direction RND if OP1 > OP2, +0 if OP1 <= OP2, and
% NaN if OP1 or OP2 is NaN.
%

  if (isa (rop, 'mpfr_t'))
    rop = rop.idx;
  end
  if (isa (op1, 'mpfr_t'))
    op1 = op1.idx;
  end
  if (isa (op2, 'mpfr_t'))
    op2 = op2.idx;
  end
  ret = gmp_mpfr_interface (50, rop, op1, op2, rnd);
end

% This function was automatically generated by "generate_m_files".

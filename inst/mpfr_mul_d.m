function ret = mpfr_mul_d (rop, op1, op2, rnd)
% Set ROP to OP1 times OP2 rounded in the direction RND.  When a
% result is zero, its sign is the product of the signs of the
% operands (for types having no signed zeros, 0 is considered
% positive).  The same restrictions than for ‘mpfr_add_d’ apply to
% ‘mpfr_mul_d’.
%

  if (isa (rop, 'mpfr_t'))
    rop = rop.idx;
  end
  if (isa (op1, 'mpfr_t'))
    op1 = op1.idx;
  end
  ret = gmp_mpfr_interface (37, rop, op1, op2, rnd);
end

% This function was automatically generated by "generate_m_files".

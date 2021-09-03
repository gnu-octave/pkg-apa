function ret = mpfr_beta (rop, op1, op2, rnd)
% ret = mpfr_beta (rop, op1, op2, rnd)
%
% Set ROP to the value of the Beta function at arguments OP1 and OP2.
% Note: the current code does not try to avoid internal overflow or
% underflow, and might use a huge internal precision in some cases.
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
  ret = gmp_mpfr_interface (126, rop, op1, op2, rnd);
end

% This function was automatically generated by "generate_m_files".

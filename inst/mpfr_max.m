function ret = mpfr_max (rop, op1, op2, rnd)
% Set ROP to the minimum (resp. maximum) of OP1 and OP2.  If OP1 and
% OP2 are both NaN, then ROP is set to NaN.  If OP1 or OP2 is NaN,
% then ROP is set to the numeric value.  If OP1 and OP2 are zeros of
% different signs, then ROP is set to −0 (resp. +0).
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
  ret = gmp_mpfr_interface (170, rop, op1, op2, rnd);
end

% This function was automatically generated by "generate_m_files".

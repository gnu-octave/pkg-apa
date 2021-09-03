function ret = mpfr_cmpabs (op1, op2)
% Compare |OP1| and |OP2|.  Return a positive value if |OP1| > |OP2|,
% zero if |OP1| = |OP2|, and a negative value if |OP1| < |OP2|.  If
% one of the operands is NaN, set the _erange_ flag and return zero.
%

  if (isa (op1, 'mpfr_t'))
    op1 = op1.idx;
  end
  if (isa (op2, 'mpfr_t'))
    op2 = op2.idx;
  end
  ret = gmp_mpfr_interface (67, op1, op2);
end

% This function was automatically generated by "generate_m_files".

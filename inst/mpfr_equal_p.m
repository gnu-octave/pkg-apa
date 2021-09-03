function ret = mpfr_equal_p (op1, op2)
% ret = mpfr_equal_p (op1, op2)
%
% Return non-zero if OP1 > OP2, OP1 >= OP2, OP1 < OP2, OP1 <= OP2,
% OP1 = OP2 respectively, and zero otherwise.  Those functions return
% zero whenever OP1 and/or OP2 is NaN.
%

  if (isa (op1, 'mpfr_t'))
    op1 = op1.idx;
  end
  if (isa (op2, 'mpfr_t'))
    op2 = op2.idx;
  end
  ret = gmp_mpfr_interface (79, op1, op2);
end

% This function was automatically generated by "generate_m_files".

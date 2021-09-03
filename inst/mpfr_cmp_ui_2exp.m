function ret = mpfr_cmp_ui_2exp (op1, op2, e)
% ret = mpfr_cmp_ui_2exp (op1, op2, e)
%
% Compare OP1 and OP2 multiplied by two to the power E.  Similar as
% above.
%

  if (isa (op1, 'mpfr_t'))
    op1 = op1.idx;
  end
  ret = gmp_mpfr_interface (65, op1, op2, e);
end

% This function was automatically generated by "generate_m_files".

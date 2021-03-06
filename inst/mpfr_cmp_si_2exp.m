function ret = mpfr_cmp_si_2exp (op1, op2, e)
% ret = mpfr_cmp_si_2exp (op1, op2, e)
%
% Compare OP1 and OP2 multiplied by two to the power E.  Similar as
% above.
%

  ret = mex_apa_interface (1066, op1, op2, e);
end

% This function was automatically generated by "generate_m_files".

function ret = mpfr_sub_ui (rop, op1, op2, rnd)
% ret = mpfr_sub_ui (rop, op1, op2, rnd)
%
% Set ROP to OP1 - OP2 rounded in the direction RND.  The IEEE 754
% rules are used, in particular for signed zeros.  But for types
% having no signed zeros, 0 is considered unsigned (i.e., (+0) − 0 =
% (+0), (−0) − 0 = (−0), 0 − (+0) = (−0) and 0 − (−0) = (+0)).  The
% same restrictions than for ‘mpfr_add_d’ apply to ‘mpfr_d_sub’ and
% ‘mpfr_sub_d’.
%

  ret = mex_apa_interface (1341, rop, op1, op2, rnd);
end

% This function was automatically generated by "generate_m_files".

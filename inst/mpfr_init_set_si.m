function ret = mpfr_init_set_si (rop, op, rnd)
% ret = mpfr_init_set_si (rop, op, rnd)
%
% Initialize ROP and set its value from OP, rounded in the direction
% RND.  The precision of ROP will be taken from the active default
% precision, as set by ‘mpfr_set_default_prec’.
%

  ret = mex_apa_interface (1312, rop, op, rnd);
end

% This function was automatically generated by "generate_m_files".

function mpfr_init (x)
% mpfr_init (x)
%
% Initialize X, set its precision to the default precision, and set
% its value to NaN.  The default precision can be changed by a call
% to ‘mpfr_set_default_prec’.
%
% Warning!  In a given program, some other libraries might change the
% default precision and not restore it.  Thus it is safer to use
% ‘mpfr_init2’.
%

  mex_apa_interface (1218, x);
end

% This function was automatically generated by "generate_m_files".

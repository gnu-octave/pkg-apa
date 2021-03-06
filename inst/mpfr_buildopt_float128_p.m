function ret = mpfr_buildopt_float128_p ()
% ret = mpfr_buildopt_float128_p ()
%
% Return a non-zero value if MPFR was compiled with ‘_Float128’
% support (that is, MPFR was built with the ‘--enable-float128’
% configure option), return zero otherwise.
%

  ret = mex_apa_interface (1183);
end

% This function was automatically generated by "generate_m_files".

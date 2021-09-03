function ret = mpfr_buildopt_sharedcache_p ()
% ret = mpfr_buildopt_sharedcache_p ()
%
% Return a non-zero value if MPFR was compiled so that all threads
% share the same cache for one MPFR constant, like ‘mpfr_const_pi’ or
% ‘mpfr_const_log2’ (that is, MPFR was built with the
% ‘--enable-shared-cache’ configure option), return zero otherwise.
% If the return value is non-zero, MPFR applications may need to be
% compiled with the ‘-pthread’ option.
%

  ret = gmp_mpfr_interface (186);
end

% This function was automatically generated by "generate_m_files".

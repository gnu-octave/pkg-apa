function ret = mpfr_nanflag_p ()
% Return the corresponding (underflow, overflow, divide-by-zero,
% invalid, inexact, _erange_) flag, which is non-zero iff the flag is
% set.
%

  ret = gmp_mpfr_interface (214);
end

% This function was automatically generated by "generate_m_files".

function ret = mpfr_get_exp (x)
% ret = mpfr_get_exp (x)
%
% Return the exponent of X, assuming that X is a non-zero ordinary
% number and the significand is considered in [1/2,1).  For this
% function, X is allowed to be outside of the current range of
% acceptable values.  The behavior for NaN, infinity or zero is
% undefined.
%

  ret = mex_apa_interface (1176, x);
end

% This function was automatically generated by "generate_m_files".

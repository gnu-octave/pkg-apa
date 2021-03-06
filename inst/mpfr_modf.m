function ret = mpfr_modf (iop, fop, op, rnd)
% ret = mpfr_modf (iop, fop, op, rnd)
%
% Set simultaneously IOP to the integral part of OP and FOP to the
% fractional part of OP, rounded in the direction RND with the
% corresponding precision of IOP and FOP (equivalent to
% ‘mpfr_trunc(IOP, OP, RND)’ and ‘mpfr_frac(FOP, OP, RND)’).  The
% variables IOP and FOP must be different.  Return 0 iff both results
% are exact (see ‘mpfr_sin_cos’ for a more detailed description of
% the return value).
%

  ret = mex_apa_interface (1155, iop, fop, op, rnd);
end

% This function was automatically generated by "generate_m_files".

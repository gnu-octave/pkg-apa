function ret = mpfr_prec_round (x, prec, rnd)
% ret = mpfr_prec_round (x, prec, rnd)
%
% Round X according to RND with precision PREC, which must be an
% integer between ‘MPFR_PREC_MIN’ and ‘MPFR_PREC_MAX’ (otherwise the
% behavior is undefined).  If PREC is greater than or equal to the
% precision of X, then new space is allocated for the significand,
% and it is filled with zeros.  Otherwise, the significand is rounded
% to precision PREC with the given direction; no memory reallocation
% to free the unused limbs is done.  In both cases, the precision of
% X is changed to PREC.
%
% Here is an example of how to use ‘mpfr_prec_round’ to implement
% Newton’s algorithm to compute the inverse of A, assuming X is
% already an approximation to N bits:
%
% Warning!  You must not use this function if X was initialized with
% ‘MPFR_DECL_INIT’ or with ‘mpfr_custom_init_set’ (*note Custom
% Interface::).
%

  if (isa (x, 'mpfr_t'))
    x = x.idx;
  end
  ret = gmp_mpfr_interface (163, x, prec, rnd);
end

% This function was automatically generated by "generate_m_files".

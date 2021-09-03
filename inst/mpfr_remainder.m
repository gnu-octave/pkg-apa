function ret = mpfr_remainder (r, x, y, rnd)
% Set R to the value of X - NY, rounded according to the direction
% RND, where N is the integer quotient of X divided by Y, defined as
% follows: N is rounded toward zero for ‘mpfr_fmod’ and
% ‘mpfr_fmodquo’, and to the nearest integer (ties rounded to even)
% for ‘mpfr_remainder’ and ‘mpfr_remquo’.
%
% Special values are handled as described in Section F.9.7.1 of the
% ISO C99 standard: If X is infinite or Y is zero, R is NaN.  If Y is
% infinite and X is finite, R is X rounded to the precision of R.  If
% R is zero, it has the sign of X.  The return value is the ternary
% value corresponding to R.
%
% Additionally, ‘mpfr_fmodquo’ and ‘mpfr_remquo’ store the low
% significant bits from the quotient N in *Q (more precisely the
% number of bits in a ‘long’ minus one), with the sign of X divided
% by Y (except if those low bits are all zero, in which case zero is
% returned).  Note that X may be so large in magnitude relative to Y
% that an exact representation of the quotient is not practical.  The
% ‘mpfr_remainder’ and ‘mpfr_remquo’ functions are useful for
% additive argument reduction.
%

  if (isa (r, 'mpfr_t'))
    r = r.idx;
  end
  if (isa (x, 'mpfr_t'))
    x = x.idx;
  end
  if (isa (y, 'mpfr_t'))
    y = y.idx;
  end
  ret = gmp_mpfr_interface (158, r, x, y, rnd);
end

% This function was automatically generated by "generate_m_files".

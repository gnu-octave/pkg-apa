function ret = mpfr_fits_uintmax_p (op, rnd)
% ret = mpfr_fits_uintmax_p (op, rnd)
%
% Return non-zero if OP would fit in the respective C data type,
% respectively ‘unsigned long’, ‘long’, ‘unsigned int’, ‘int’,
% ‘unsigned short’, ‘short’, ‘uintmax_t’, ‘intmax_t’, when rounded to
% an integer in the direction RND.  For instance, with the
% ‘MPFR_RNDU’ rounding mode on −0.5, the result will be non-zero for
% all these functions.  For ‘MPFR_RNDF’, those functions return
% non-zero when it is guaranteed that the corresponding conversion
% function (for example ‘mpfr_get_ui’ for ‘mpfr_fits_ulong_p’), when
% called with faithful rounding, will always return a number that is
% representable in the corresponding type.  As a consequence, for
% ‘MPFR_RNDF’, ‘mpfr_fits_ulong_p’ will return non-zero for a
% non-negative number less than or equal to ‘ULONG_MAX’.
%

  if (isa (op, 'mpfr_t'))
    op = op.idx;
  end
  ret = gmp_mpfr_interface (29, op, rnd);
end

% This function was automatically generated by "generate_m_files".

function ret = mpfr_log2 (rop, op, rnd)
% ret = mpfr_log2 (rop, op, rnd)
%
% Set ROP to the natural logarithm of OP, log2(OP) or log10(OP),
% respectively, rounded in the direction RND.  Set ROP to +0 if OP is
% 1 (in all rounding modes), for consistency with the ISO C99 and
% IEEE 754-2008 standards.  Set ROP to −Inf if OP is ±0 (i.e., the
% sign of the zero has no influence on the result).
%

  if (isa (rop, 'mpfr_t'))
    rop = rop.idx;
  end
  if (isa (op, 'mpfr_t'))
    op = op.idx;
  end
  ret = gmp_mpfr_interface (85, rop, op, rnd);
end

% This function was automatically generated by "generate_m_files".

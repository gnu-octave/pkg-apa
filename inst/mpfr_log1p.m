function ret = mpfr_log1p (rop, op, rnd)
% ret = mpfr_log1p (rop, op, rnd)
%
% Set ROP to the logarithm of one plus OP, rounded in the direction
% RND.  Set ROP to −Inf if OP is −1.
%

  if (isa (rop, 'mpfr_t'))
    rop = rop.idx;
  end
  if (isa (op, 'mpfr_t'))
    op = op.idx;
  end
  ret = gmp_mpfr_interface (87, rop, op, rnd);
end

% This function was automatically generated by "generate_m_files".

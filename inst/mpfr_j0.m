function ret = mpfr_j0 (rop, op, rnd)
% ret = mpfr_j0 (rop, op, rnd)
%
% Set ROP to the value of the first kind Bessel function of order 0,
% (resp. 1 and N) on OP, rounded in the direction RND.  When OP is
% NaN, ROP is always set to NaN.  When OP is plus or minus Infinity,
% ROP is set to +0.  When OP is zero, and N is not zero, ROP is set
% to +0 or −0 depending on the parity and sign of N, and the sign of
% OP.
%

  if (isa (rop, 'mpfr_t'))
    rop = rop.idx;
  end
  if (isa (op, 'mpfr_t'))
    op = op.idx;
  end
  ret = gmp_mpfr_interface (131, rop, op, rnd);
end

% This function was automatically generated by "generate_m_files".

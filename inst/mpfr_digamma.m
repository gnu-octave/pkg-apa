function ret = mpfr_digamma (rop, op, rnd)
% ret = mpfr_digamma (rop, op, rnd)
%
% Set ROP to the value of the Digamma (sometimes also called Psi)
% function on OP, rounded in the direction RND.  When OP is a
% negative integer, set ROP to NaN.
%

  if (isa (rop, 'mpfr_t'))
    rop = rop.idx;
  end
  if (isa (op, 'mpfr_t'))
    op = op.idx;
  end
  ret = gmp_mpfr_interface (125, rop, op, rnd);
end

% This function was automatically generated by "generate_m_files".

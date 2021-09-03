function ret = mpfr_coth (rop, op, rnd)
% ret = mpfr_coth (rop, op, rnd)
%
% Set ROP to the hyperbolic secant of OP, cosecant of OP, cotangent
% of OP, rounded in the direction RND.
%

  if (isa (rop, 'mpfr_t'))
    rop = rop.idx;
  end
  if (isa (op, 'mpfr_t'))
    op = op.idx;
  end
  ret = gmp_mpfr_interface (115, rop, op, rnd);
end

% This function was automatically generated by "generate_m_files".

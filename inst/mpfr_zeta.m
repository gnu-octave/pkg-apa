function ret = mpfr_zeta (rop, op, rnd)
% Set ROP to the value of the Riemann Zeta function on OP, rounded in
% the direction RND.
%

  if (isa (rop, 'mpfr_t'))
    rop = rop.idx;
  end
  if (isa (op, 'mpfr_t'))
    op = op.idx;
  end
  ret = gmp_mpfr_interface (127, rop, op, rnd);
end

% This function was automatically generated by "generate_m_files".

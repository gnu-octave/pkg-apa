function ret = mpfr_fac_ui (rop, op, rnd)
% ret = mpfr_fac_ui (rop, op, rnd)
%
% Set ROP to the factorial of OP, rounded in the direction RND.
%

  if (isa (rop, 'mpfr_t'))
    rop = rop.idx;
  end
  ret = gmp_mpfr_interface (55, rop, op, rnd);
end

% This function was automatically generated by "generate_m_files".

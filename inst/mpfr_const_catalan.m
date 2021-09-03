function ret = mpfr_const_catalan (rop, rnd)
% Set ROP to the logarithm of 2, the value of Pi, of Euler’s constant
% 0.577..., of Catalan’s constant 0.915..., respectively, rounded in
% the direction RND.  These functions cache the computed values to
% avoid other calculations if a lower or equal precision is
% requested.  To free these caches, use ‘mpfr_free_cache’ or
% ‘mpfr_free_cache2’.
%

  if (isa (rop, 'mpfr_t'))
    rop = rop.idx;
  end
  ret = gmp_mpfr_interface (142, rop, rnd);
end

% This function was automatically generated by "generate_m_files".

function ret = mpfr_sin (rop, op, rnd)
% ret = mpfr_sin (rop, op, rnd)
%
% Set ROP to the cosine of OP, sine of OP, tangent of OP, rounded in
% the direction RND.
%

  ret = mex_apa_interface (1099, rop, op, rnd);
end

% This function was automatically generated by "generate_m_files".

function ret = mpfr_cos (rop, op, rnd)
% ret = mpfr_cos (rop, op, rnd)
%
% Set ROP to the cosine of OP, sine of OP, tangent of OP, rounded in
% the direction RND.
%

  ret = mex_apa_interface (1098, rop, op, rnd);
end

% This function was automatically generated by "generate_m_files".

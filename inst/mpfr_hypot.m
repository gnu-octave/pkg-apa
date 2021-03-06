function ret = mpfr_hypot (rop, x, y, rnd)
% ret = mpfr_hypot (rop, x, y, rnd)
%
% Set ROP to the Euclidean norm of X and Y, i.e., the square root of
% the sum of the squares of X and Y, rounded in the direction RND.
% Special values are handled as described in the ISO C99
% (Section F.9.4.3) and IEEE 754-2008 (Section 9.2.1) standards: If X
% or Y is an infinity, then +Inf is returned in ROP, even if the
% other number is NaN.
%

  ret = mex_apa_interface (1060, rop, x, y, rnd);
end

% This function was automatically generated by "generate_m_files".

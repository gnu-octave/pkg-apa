function ret = mpfr_sum (rop, tab, n, rnd)
% ret = mpfr_sum (rop, tab, n, rnd)
%
% Set ROP to the sum of all elements of TAB, whose size is N,
% correctly rounded in the direction RND.  Warning: for efficiency
% reasons, TAB is an array of pointers to ‘mpfr_t’, not an array of
% ‘mpfr_t’.  If N = 0, then the result is +0, and if N = 1, then the
% function is equivalent to ‘mpfr_set’.  For the special exact cases,
% the result is the same as the one obtained with a succession of
% additions (‘mpfr_add’) in infinite precision.  In particular, if
% the result is an exact zero and N >= 1:
% • if all the inputs have the same sign (i.e., all +0 or all −0),
% • otherwise, either because all inputs are zeros with at least a
%

  ret = mex_apa_interface (1061, rop, tab, n, rnd);
end

% This function was automatically generated by "generate_m_files".

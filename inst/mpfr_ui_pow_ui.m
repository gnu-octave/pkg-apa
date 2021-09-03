function ret = mpfr_ui_pow_ui (rop, op1, op2, rnd)
% ret = mpfr_ui_pow_ui (rop, op1, op2, rnd)
%
% Set ROP to OP1 raised to OP2, rounded in the direction RND.
% Special values are handled as described in the ISO C99 and
% IEEE 754-2008 standards for the ‘pow’ function:
% • ‘pow(±0, Y)’ returns plus or minus infinity for Y a negative
% • ‘pow(±0, Y)’ returns plus infinity for Y negative and not an
% • ‘pow(±0, Y)’ returns plus or minus zero for Y a positive odd
% • ‘pow(±0, Y)’ returns plus zero for Y positive and not an odd
% • ‘pow(-1, ±Inf)’ returns 1.
% • ‘pow(+1, Y)’ returns 1 for any Y, even a NaN.
% • ‘pow(X, ±0)’ returns 1 for any X, even a NaN.
% • ‘pow(X, Y)’ returns NaN for finite negative X and finite
% • ‘pow(X, -Inf)’ returns plus infinity for 0 < abs(x) < 1, and
% • ‘pow(X, +Inf)’ returns plus zero for 0 < abs(x) < 1, and plus
% • ‘pow(-Inf, Y)’ returns minus zero for Y a negative odd
% • ‘pow(-Inf, Y)’ returns plus zero for Y negative and not an odd
% • ‘pow(-Inf, Y)’ returns minus infinity for Y a positive odd
% • ‘pow(-Inf, Y)’ returns plus infinity for Y positive and not an
% • ‘pow(+Inf, Y)’ returns plus zero for Y negative, and plus
% Note: When 0 is of integer type, it is regarded as +0 by these
% functions.  We do not use the usual limit rules in this case, as
% these rules are not used for ‘pow’.
%

  if (isa (rop, 'mpfr_t'))
    rop = rop.idx;
  end
  ret = gmp_mpfr_interface (96, rop, op1, op2, rnd);
end

% This function was automatically generated by "generate_m_files".

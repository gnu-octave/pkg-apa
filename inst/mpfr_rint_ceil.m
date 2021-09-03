function ret = mpfr_rint_ceil (rop, op, rnd)
% ret = mpfr_rint_ceil (rop, op, rnd)
%
% Set ROP to OP rounded to an integer:
% • ‘mpfr_rint_ceil’: to the next higher or equal integer;
% • ‘mpfr_rint_floor’: to the next lower or equal integer;
% • ‘mpfr_rint_round’: to the nearest integer, rounding halfway
% • ‘mpfr_rint_roundeven’: to the nearest integer, rounding
% • ‘mpfr_rint_trunc’ to the next integer toward zero.
% If the result is not representable, it is rounded in the direction
% RND.  When OP is a zero or an infinity, set ROP to the same value
% (with the same sign).  The return value is the ternary value
% associated with the considered round-to-integer function (regarded
% in the same way as any other mathematical function).
%
% Contrary to ‘mpfr_rint’, those functions do perform a double
% rounding: first OP is rounded to the nearest integer in the
% direction given by the function name, then this nearest integer (if
% not representable) is rounded in the given direction RND.  Thus
% these round-to-integer functions behave more like the other
% mathematical functions, i.e., the returned result is the correct
% rounding of the exact result of the function in the real numbers.
%
% For example, ‘mpfr_rint_round’ with rounding to nearest and a
% precision of two bits rounds 6.5 to 7 (halfway cases away from
% zero), then 7 is rounded to 8 by the round-even rule, despite the
% fact that 6 is also representable on two bits, and is closer to 6.5
% than 8.
%

  if (isa (rop, 'mpfr_t'))
    rop = rop.idx;
  end
  if (isa (op, 'mpfr_t'))
    op = op.idx;
  end
  ret = gmp_mpfr_interface (149, rop, op, rnd);
end

% This function was automatically generated by "generate_m_files".

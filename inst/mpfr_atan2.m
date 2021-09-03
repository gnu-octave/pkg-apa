function ret = mpfr_atan2 (rop, y, x, rnd)
% ret = mpfr_atan2 (rop, y, x, rnd)
%
% Set ROP to the arc-tangent2 of Y and X, rounded in the direction
% RND: if ‘x > 0’, ‘atan2(y, x) = atan(y/x)’; if ‘x < 0’, ‘atan2(y,
% x) = sign(y)*(Pi - atan(abs(y/x)))’, thus a number from -Pi to Pi.
% As for ‘atan’, in case the exact mathematical result is +Pi or -Pi,
% its rounded result might be outside the function output range.
%
% ‘atan2(y, 0)’ does not raise any floating-point exception.  Special
% values are handled as described in the ISO C99 and IEEE 754-2008
% standards for the ‘atan2’ function:
% • ‘atan2(+0, -0)’ returns +Pi.
% • ‘atan2(-0, -0)’ returns -Pi.
% • ‘atan2(+0, +0)’ returns +0.
% • ‘atan2(-0, +0)’ returns −0.
% • ‘atan2(+0, x)’ returns +Pi for x < 0.
% • ‘atan2(-0, x)’ returns -Pi for x < 0.
% • ‘atan2(+0, x)’ returns +0 for x > 0.
% • ‘atan2(-0, x)’ returns −0 for x > 0.
% • ‘atan2(y, 0)’ returns -Pi/2 for y < 0.
% • ‘atan2(y, 0)’ returns +Pi/2 for y > 0.
% • ‘atan2(+Inf, -Inf)’ returns +3*Pi/4.
% • ‘atan2(-Inf, -Inf)’ returns -3*Pi/4.
% • ‘atan2(+Inf, +Inf)’ returns +Pi/4.
% • ‘atan2(-Inf, +Inf)’ returns -Pi/4.
% • ‘atan2(+Inf, x)’ returns +Pi/2 for finite x.
% • ‘atan2(-Inf, x)’ returns -Pi/2 for finite x.
% • ‘atan2(y, -Inf)’ returns +Pi for finite y > 0.
% • ‘atan2(y, -Inf)’ returns -Pi for finite y < 0.
% • ‘atan2(y, +Inf)’ returns +0 for finite y > 0.
% • ‘atan2(y, +Inf)’ returns −0 for finite y < 0.
%

  if (isa (rop, 'mpfr_t'))
    rop = rop.idx;
  end
  if (isa (y, 'mpfr_t'))
    y = y.idx;
  end
  if (isa (x, 'mpfr_t'))
    x = x.idx;
  end
  ret = gmp_mpfr_interface (108, rop, y, x, rnd);
end

% This function was automatically generated by "generate_m_files".

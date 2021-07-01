clc;
clear -f __mpfr__
mkoctfile --mex -Wall -Wextra __mpfr__.c -lmpfr -lgmp

% Good input
for i = [4, 53]
  __mpfr__ ("set_default_prec", i)
  assert (__mpfr__ ("get_default_prec"), i)
end
% Bad input
__mpfr__ ("set_default_prec", 42)
for i = {inf, -42, -2, 1/6, nan, 'c', eye(3)}
  __mpfr__ ("set_default_prec", i{1})
  assert (__mpfr__ ("get_default_prec"), 42)
end

% Good input
for i = -1:3
  __mpfr__ ("set_default_rounding_mode", i)
  assert (__mpfr__ ("get_default_rounding_mode"), i)
end
% Bad input
for i = {inf, -42, -2, 4, 1/6, nan, 'c', eye(3)}
  __mpfr__ ("set_default_rounding_mode", i{1})
  assert (__mpfr__ ("get_default_rounding_mode"), 0)
end
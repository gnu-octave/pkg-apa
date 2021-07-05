clear -f __mpfr__
mkoctfile --mex -Wall -Wextra __mpfr__.c -lmpfr -lgmp

% Good input
for i = [4, flintmax("double"), 53]
  __mpfr__ ("set_default_prec", i);
  assert (__mpfr__ ("get_default_prec"), i);
end
% Bad input
__mpfr__ ("set_default_prec", 42)
for i = {inf, -42, -2, 1/6, nan, 'c', eye(3), intmax("int64")}
  __mpfr__ ("set_default_prec", i{1});
  assert (__mpfr__ ("get_default_prec"), 42);
end

% Good input
for i = -1:3
  __mpfr__ ("set_default_rounding_mode", i);
  assert (__mpfr__ ("get_default_rounding_mode"), i);
end
% Bad input
__mpfr__ ("set_default_rounding_mode", 0);
for i = {inf, -42, -2, 4, 1/6, nan, 'c', eye(3)}
  __mpfr__ ("set_default_rounding_mode", i{1});
  assert (__mpfr__ ("get_default_rounding_mode"), 0);
end

% Good input
DATA_CHUNK_SIZE = 1000;
assert (__mpfr__ ("init2", 1, 50), 1);
assert (__mpfr__ ("data_size"), 1);
assert (__mpfr__ ("data_capacity"), DATA_CHUNK_SIZE);
assert (__mpfr__ ("init2", DATA_CHUNK_SIZE, 75), (1:DATA_CHUNK_SIZE)' + 1);
assert (__mpfr__ ("data_size"), DATA_CHUNK_SIZE + 1);
assert (__mpfr__ ("data_capacity"), 2 * DATA_CHUNK_SIZE);
% Bad input
for i = {1/2, inf, -42, -1, -2, nan, 'c', eye(3)}
  __mpfr__ ("init2", i{1}, 75)
  assert (__mpfr__ ("data_size"), DATA_CHUNK_SIZE + 1);
  assert (__mpfr__ ("data_capacity"), 2 * DATA_CHUNK_SIZE);
end

% Good input
for i = {inf, -42, -2, nan, 1, 2.5, pi}
  __mpfr__ ("set_d", 1, i{1}, 0)
  assert (__mpfr__ ("get_d", 1, 0), i{1})
end
% Good input (with eps tolerance)
for i = {1/6}
  __mpfr__ ("set_d", 1, i{1}, 0)
  assert (__mpfr__ ("get_d", 1, 0), i{1}, eps ())
end

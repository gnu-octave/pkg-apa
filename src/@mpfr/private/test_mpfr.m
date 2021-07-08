clear -f mpfr_
if (exist('OCTAVE_VERSION', 'builtin') == 5)
  mex --std=c99 -Wall -Wextra mpfr_.c -lmpfr -lgmp
else
  mex CFLAGS='$CFLAGS --std=c99 -Wall -Wextra' mpfr_.c -lmpfr -lgmp
end

% Good input
for i = [4, flintmax('double'), 53]
  mpfr_ ('set_default_prec', i);
  assert (mpfr_ ('get_default_prec') == i);
end
% Bad input
mpfr_ ('set_default_prec', 42)
for i = {inf, -42, -2, 1/6, nan, 'c', eye(3), intmax('int64')}
  try
    mpfr_ ('set_default_prec', i{1});
    error ('mp:test:missed', 'Should never be reached');
  catch e
    assert (strcmp (e.identifier, 'mp:mexFunction'));
  end
end

% Good input
for i = -1:3
  mpfr_ ('set_default_rounding_mode', i);
  assert (mpfr_ ('get_default_rounding_mode') == i);
end
% Bad input
mpfr_ ('set_default_rounding_mode', 0);
for i = {inf, -42, -2, 4, 1/6, nan, 'c', eye(3)}
  try
    mpfr_ ('set_default_rounding_mode', i{1});
    error ('mp:test:missed', 'Should never be reached');
  catch e
    assert (strcmp (e.identifier, 'mp:mexFunction'));
  end
end

% Good input
DATA_CHUNK_SIZE = 1000;
assert (all (mpfr_ ('init2', 1, 50) == [1; 1]));
assert (mpfr_ ('data_size') == 1);
assert (mpfr_ ('data_capacity') == DATA_CHUNK_SIZE);
assert (all (mpfr_ ('init2', DATA_CHUNK_SIZE, 75) == [2; DATA_CHUNK_SIZE + 1]));
assert (mpfr_ ('data_size') == DATA_CHUNK_SIZE + 1);
assert (mpfr_ ('data_capacity') == 2 * DATA_CHUNK_SIZE);
% Bad input
for i = {1/2, inf, -42, -1, -2, nan, 'c', eye(3)}
  try
    mpfr_ ('init2', i{1}, 75);
    error ('mp:test:missed', 'Should never be reached');
  catch e
    assert (strcmp (e.identifier, 'mp:mexFunction'));
    assert (mpfr_ ('data_size') == DATA_CHUNK_SIZE + 1);
    assert (mpfr_ ('data_capacity') == 2 * DATA_CHUNK_SIZE);
  end
end

% Good input
idx = mpfr_ ('init2', 1, 50);
for i = {inf, -42, -2, 1, 2.5, pi}
  mpfr_ ('set_d', idx, i{1}, 0);
  assert (mpfr_ ('get_d', idx, 0) == i{1});
end
% Good input (NaN)
mpfr_ ('set_d', idx, nan, 0);
assert (isnan (mpfr_ ('get_d', idx, 0)));
% Good input (with eps tolerance)
for i = {1/6}
  mpfr_ ('set_d', idx, i{1}, 0);
  assert ((mpfr_ ('get_d', idx, 0) - i{1}) <= eps ());
end

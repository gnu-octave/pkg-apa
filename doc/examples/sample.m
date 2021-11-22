function sample ()
% Example from https://www.mpfr.org/sample.html (2021-11-19).

t = mpfr_t (1.0, 200, MPFR_RNDD);
s = mpfr_t (1.0, 200, MPFR_RNDD);
u = mpfr_t (nan, 200);

for i = 1:100
  mpfr_mul_ui (t, t, i, MPFR_RNDU);
  mpfr_set_d (u, 1.0, MPFR_RNDD);
  mpfr_div (u, u, t, MPFR_RNDD);
  mpfr_add (s, s, u, MPFR_RNDD);
end

fprintf ('Sum is ');
disp (s, MPFR_RNDD);


% APA high-level interface implementation

warning ('off', 'mpfr_t:inexactOperation')
mpfr_set_default_rounding_mode (MPFR_RNDD);
mpfr_set_default_prec (200);

t = mpfr_t (1.0);
s = mpfr_t (1.0);

for i = 1:100
  mpfr_set_default_rounding_mode (MPFR_RNDU);
  t = t * i;
  mpfr_set_default_rounding_mode (MPFR_RNDD);
  s = s + (1 / t);
end

fprintf ('Sum is ');
disp (s);

end

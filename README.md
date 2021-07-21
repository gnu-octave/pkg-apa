# APA - Octave/Matlab arbitrary precision arithmetic.

## High-level MPFR Interface

The high-level MPFR interface is given through the `@mpfr_t` class.
A variable of that type "behaves" like a "normal" built-in Octave/Matlab
data type.

The high-level MPFR interface is the preferred choice for quick numerical
experiments.

However, if performance is more critical, please use the low-level MPFR
interface (explained below) and vectorization wherever possible.

> Please note that an interface from an interpreted high-level programming
> language like Octave/Matlab is most likely slower than a pre-compiled C
> program.
>
> If performance is highly-critical, use this tool for initial experiments
> and translate the developed algorithm to native MPFR C-code.


## Low-level MPFR Interface

> Implementation status: 70% of MPFR 4.1.0 functions are supported.

The low-level MPFR interface permits efficient access to all functions
specified by MPFR <https://www.mpfr.org/mpfr-current/mpfr.html>.

(Almost) all specified functions can be called from Octave/Matlab with the
`mpfr_`-helper function.

For example:

    int mpfr_add (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_rnd_t rnd)

can be called using the C-like notation with:

```octave
% Prepare input and output variables.
rop = mpfr_t(zeros (3));
op1 = mpfr_t(ones (3));
op2 = mpfr_t(4 * eye (3));
rnd = mpfr_t.get_default_rounding_mode ();

% Call mpfr_add.  Note unlike Octave/Matlab the
% left-hand side does NOT contain the result.
ret = mpfr_('add', rop, op1, op2, rnd);

double (rop)  % Note rop vs. ret!
```

    ans =
      5     1     1
      1     5     1
      1     1     5

Another benefit of using the low-level MPFR interface is that **in-place**
operations are permitted:
```octave
ret = mpfr_('add', op1, op1, op1, rnd);  % op1 += op1
```

### Initialization Functions

- [x] `void mpfr_init2 (mpfr_t x, mpfr_prec_t prec)`

- [x] `void mpfr_set_default_prec (mpfr_prec_t prec)`
- [x] `mpfr_prec_t mpfr_get_default_prec (void)`

- [x] `void mpfr_set_prec (mpfr_t x, mpfr_prec_t prec)`
- [x] `mpfr_prec_t mpfr_get_prec (mpfr_t x)`


### Assignment Functions

- [x] `int mpfr_set (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)`
- [x] `int mpfr_set_d (mpfr_t rop, double op, mpfr_rnd_t rnd)`

- [x] `int mpfr_set_ui_2exp (mpfr_t rop, unsigned long int op, mpfr_exp_t e, mpfr_rnd_t rnd)`
- [x] `int mpfr_set_si_2exp (mpfr_t rop, long int op, mpfr_exp_t e, mpfr_rnd_t rnd)`

- [x] `int mpfr_set_str (mpfr_t rop, const char *s, int base, mpfr_rnd_t rnd)`
- [x] `int mpfr_strtofr (mpfr_t rop, const char *nptr, char **endptr, int base, mpfr_rnd_t rnd)`

  > Note: The interface is changed to
  > `[returned_int_value, endptr] = mpfr_('strtofr', rop, nptr, base, rnd);`,
  > where `endptr` is the numerical index in the `nptr` input cell array of
  > strings that points to the character just after the valid data (if `endptr`
  > is not `-1`).  This means, if `endptr > numel (nptr)`, then all input was
  > valid and processed.

- [x] `void mpfr_set_nan (mpfr_t x)`
- [x] `void mpfr_set_inf (mpfr_t x, int sign)`
- [x] `void mpfr_set_zero (mpfr_t x, int sign)`

- [x] `void mpfr_swap (mpfr_t x, mpfr_t y)`


### Combined Initialization and Assignment Functions

- [x] `int mpfr_init_set (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)`
- [x] `int mpfr_init_set_d (mpfr_t rop, double op, mpfr_rnd_t rnd)`
- [x] `int mpfr_init_set_str (mpfr_t x, const char *s, int base, mpfr_rnd_t rnd)`


### Conversion Functions

- [x] `double mpfr_get_d (mpfr_t op, mpfr_rnd_t rnd)`
- [ ] `double mpfr_get_d_2exp (long *exp, mpfr_t op, mpfr_rnd_t rnd)`

- [ ] `int mpfr_frexp (mpfr_exp_t *exp, mpfr_t y, mpfr_t x, mpfr_rnd_t rnd)`

- [ ] `size_t mpfr_get_str_ndigits (int b, mpfr_prec_t p)`
- [x] `char * mpfr_get_str (char *str, mpfr_exp_t *expptr, int base, size_t n, mpfr_t op, mpfr_rnd_t rnd)`

  > Note: The interface is changed to
  > `[significant, expptr] = mpfr_('get_str', base, n, op, rnd);`,
  >
  > A call to `mpfr_free_str` is not necessary.

- [x] `void mpfr_free_str (char *str)`

- [x] `int mpfr_fits_ulong_p (mpfr_t op, mpfr_rnd_t rnd)`
- [x] `int mpfr_fits_slong_p (mpfr_t op, mpfr_rnd_t rnd)`
- [x] `int mpfr_fits_uint_p (mpfr_t op, mpfr_rnd_t rnd)`
- [x] `int mpfr_fits_sint_p (mpfr_t op, mpfr_rnd_t rnd)`
- [x] `int mpfr_fits_ushort_p (mpfr_t op, mpfr_rnd_t rnd)`
- [x] `int mpfr_fits_sshort_p (mpfr_t op, mpfr_rnd_t rnd)`
- [x] `int mpfr_fits_uintmax_p (mpfr_t op, mpfr_rnd_t rnd)`
- [x] `int mpfr_fits_intmax_p (mpfr_t op, mpfr_rnd_t rnd)`


### Arithmetic Functions

- [x] `int mpfr_add (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_rnd_t rnd)`
- [x] `int mpfr_add_d (mpfr_t rop, mpfr_t op1, double op2, mpfr_rnd_t rnd)`

- [x] `int mpfr_sub (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_rnd_t rnd)`
- [x] `int mpfr_d_sub (mpfr_t rop, double op1, mpfr_t op2, mpfr_rnd_t rnd)`
- [x] `int mpfr_sub_d (mpfr_t rop, mpfr_t op1, double op2, mpfr_rnd_t rnd)`

- [x] `int mpfr_mul (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_rnd_t rnd)`
- [x] `int mpfr_mul_d (mpfr_t rop, mpfr_t op1, double op2, mpfr_rnd_t rnd)`

- [x] `int mpfr_sqr (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)`

- [x] `int mpfr_div (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_rnd_t rnd)`
- [x] `int mpfr_d_div (mpfr_t rop, double op1, mpfr_t op2, mpfr_rnd_t rnd)`
- [x] `int mpfr_div_d (mpfr_t rop, mpfr_t op1, double op2, mpfr_rnd_t rnd)`

- [x] `int mpfr_sqrt (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)`
- [ ] `int mpfr_sqrt_ui (mpfr_t rop, unsigned long int op, mpfr_rnd_t rnd)`

- [x] `int mpfr_rec_sqrt (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)`

- [ ] `int mpfr_cbrt (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)`
- [ ] `int mpfr_rootn_ui (mpfr_t rop, mpfr_t op, unsigned long int n, mpfr_rnd_t rnd)`
- [ ] `int mpfr_root (mpfr_t rop, mpfr_t op, unsigned long int n, mpfr_rnd_t rnd)`

- [x] `int mpfr_neg (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)`
- [x] `int mpfr_abs (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)`

- [x] `int mpfr_dim (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_rnd_t rnd)`

- [ ] `int mpfr_mul_2ui (mpfr_t rop, mpfr_t op1, unsigned long int op2, mpfr_rnd_t rnd)`
- [ ] `int mpfr_mul_2si (mpfr_t rop, mpfr_t op1, long int op2, mpfr_rnd_t rnd)`

- [ ] `int mpfr_div_2ui (mpfr_t rop, mpfr_t op1, unsigned long int op2, mpfr_rnd_t rnd)`
- [ ] `int mpfr_div_2si (mpfr_t rop, mpfr_t op1, long int op2, mpfr_rnd_t rnd)`

- [ ] `int mpfr_fac_ui (mpfr_t rop, unsigned long int op, mpfr_rnd_t rnd)`

- [ ] `int mpfr_fma (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_t op3, mpfr_rnd_t rnd)`
- [ ] `int mpfr_fms (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_t op3, mpfr_rnd_t rnd)`

- [ ] `int mpfr_fmma (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_t op3, mpfr_t op4, mpfr_rnd_t rnd)`
- [ ] `int mpfr_fmms (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_t op3, mpfr_t op4, mpfr_rnd_t rnd)`

- [ ] `int mpfr_hypot (mpfr_t rop, mpfr_t x, mpfr_t y, mpfr_rnd_t rnd)`

- [ ] `int mpfr_sum (mpfr_t rop, const mpfr_ptr tab[], unsigned long int n, mpfr_rnd_t rnd)`

- [ ] `int mpfr_dot (mpfr_t rop, const mpfr_ptr a[], const mpfr_ptr b[], unsigned long int n, mpfr_rnd_t rnd)`


### Comparison Functions

- [x] `int mpfr_cmp (mpfr_t op1, mpfr_t op2)`
- [ ] `int mpfr_cmp_d (mpfr_t op1, double op2)`

- [ ] `int mpfr_cmp_ui_2exp (mpfr_t op1, unsigned long int op2, mpfr_exp_t e)`
- [ ] `int mpfr_cmp_si_2exp (mpfr_t op1, long int op2, mpfr_exp_t e)`

- [x] `int mpfr_cmpabs (mpfr_t op1, mpfr_t op2)`
- [ ] `int mpfr_cmpabs_ui (mpfr_t op1, unsigned long op2)`

- [x] `int mpfr_nan_p (mpfr_t op)`
- [x] `int mpfr_inf_p (mpfr_t op)`
- [x] `int mpfr_number_p (mpfr_t op)`
- [x] `int mpfr_zero_p (mpfr_t op)`
- [x] `int mpfr_regular_p (mpfr_t op)`

- [x] `int mpfr_sgn (mpfr_t op)`

- [x] `int mpfr_greater_p (mpfr_t op1, mpfr_t op2)`
- [x] `int mpfr_greaterequal_p (mpfr_t op1, mpfr_t op2)`
- [x] `int mpfr_less_p (mpfr_t op1, mpfr_t op2)`
- [x] `int mpfr_lessequal_p (mpfr_t op1, mpfr_t op2)`
- [x] `int mpfr_equal_p (mpfr_t op1, mpfr_t op2)`

- [x] `int mpfr_lessgreater_p (mpfr_t op1, mpfr_t op2)`

- [x] `int mpfr_unordered_p (mpfr_t op1, mpfr_t op2)`

- [x] `int mpfr_total_order_p (mpfr_t x, mpfr_t y)`


### Transcendental Functions

- [x] `int mpfr_log (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)`
- [ ] `int mpfr_log_ui (mpfr_t rop, unsigned long op, mpfr_rnd_t rnd)`
- [x] `int mpfr_log2 (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)`
- [x] `int mpfr_log10 (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)`

- [x] `int mpfr_log1p (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)`

- [x] `int mpfr_exp (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)`
- [x] `int mpfr_exp2 (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)`
- [x] `int mpfr_exp10 (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)`

- [x] `int mpfr_expm1 (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)`

- [x] `int mpfr_pow (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_rnd_t rnd)`
- [ ] `int mpfr_pow_ui (mpfr_t rop, mpfr_t op1, unsigned long int op2, mpfr_rnd_t rnd)`
- [ ] `int mpfr_pow_si (mpfr_t rop, mpfr_t op1, long int op2, mpfr_rnd_t rnd)`
- [ ] `int mpfr_pow_z (mpfr_t rop, mpfr_t op1, mpz_t op2, mpfr_rnd_t rnd)`
- [ ] `int mpfr_ui_pow_ui (mpfr_t rop, unsigned long int op1, unsigned long int op2, mpfr_rnd_t rnd)`
- [ ] `int mpfr_ui_pow (mpfr_t rop, unsigned long int op1, mpfr_t op2, mpfr_rnd_t rnd)`

- [x] `int mpfr_cos (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)`
- [x] `int mpfr_sin (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)`
- [x] `int mpfr_tan (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)`

- [ ] `int mpfr_sin_cos (mpfr_t sop, mpfr_t cop, mpfr_t op, mpfr_rnd_t rnd)`

- [x] `int mpfr_sec (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)`
- [x] `int mpfr_csc (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)`
- [x] `int mpfr_cot (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)`

- [x] `int mpfr_acos (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)`
- [x] `int mpfr_asin (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)`
- [x] `int mpfr_atan (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)`

- [ ] `int mpfr_atan2 (mpfr_t rop, mpfr_t y, mpfr_t x, mpfr_rnd_t rnd)`

- [x] `int mpfr_cosh (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)`
- [x] `int mpfr_sinh (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)`
- [x] `int mpfr_tanh (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)`

- [ ] `int mpfr_sinh_cosh (mpfr_t sop, mpfr_t cop, mpfr_t op, mpfr_rnd_t rnd)`

- [x] `int mpfr_sech (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)`
- [x] `int mpfr_csch (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)`
- [x] `int mpfr_coth (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)`

- [x] `int mpfr_acosh (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)`
- [x] `int mpfr_asinh (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)`
- [x] `int mpfr_atanh (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)`

- [x] `int mpfr_eint (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)`

- [x] `int mpfr_li2 (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)`

- [x] `int mpfr_gamma (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)`
- [ ] `int mpfr_gamma_inc (mpfr_t rop, mpfr_t op, mpfr_t op2, mpfr_rnd_t rnd)`

- [x] `int mpfr_lngamma (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)`

- [ ] `int mpfr_lgamma (mpfr_t rop, int *signp, mpfr_t op, mpfr_rnd_t rnd)`

- [x] `int mpfr_digamma (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)`

- [x] `int mpfr_beta (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_rnd_t rnd)`

- [x] `int mpfr_zeta (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)`
- [ ] `int mpfr_zeta_ui (mpfr_t rop, unsigned long op, mpfr_rnd_t rnd)`

- [x] `int mpfr_erf (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)`
- [x] `int mpfr_erfc (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)`

- [x] `int mpfr_j0 (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)`
- [x] `int mpfr_j1 (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)`
- [ ] `int mpfr_jn (mpfr_t rop, long n, mpfr_t op, mpfr_rnd_t rnd)`

- [x] `int mpfr_y0 (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)`
- [x] `int mpfr_y1 (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)`
- [ ] `int mpfr_yn (mpfr_t rop, long n, mpfr_t op, mpfr_rnd_t rnd)`

- [x] `int mpfr_agm (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_rnd_t rnd)`

- [ ] `int mpfr_ai (mpfr_t rop, mpfr_t x, mpfr_rnd_t rnd)`

- [ ] `int mpfr_const_log2 (mpfr_t rop, mpfr_rnd_t rnd)`
- [ ] `int mpfr_const_pi (mpfr_t rop, mpfr_rnd_t rnd)`
- [ ] `int mpfr_const_euler (mpfr_t rop, mpfr_rnd_t rnd)`
- [ ] `int mpfr_const_catalan (mpfr_t rop, mpfr_rnd_t rnd)`


### Integer and Remainder Related Functions

- [x] `int mpfr_rint (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)`
- [ ] `int mpfr_ceil (mpfr_t rop, mpfr_t op)`
- [ ] `int mpfr_floor (mpfr_t rop, mpfr_t op)`
- [ ] `int mpfr_round (mpfr_t rop, mpfr_t op)`
- [ ] `int mpfr_roundeven (mpfr_t rop, mpfr_t op)`
- [ ] `int mpfr_trunc (mpfr_t rop, mpfr_t op)`

- [x] `int mpfr_rint_ceil (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)`
- [x] `int mpfr_rint_floor (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)`
- [x] `int mpfr_rint_round (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)`
- [x] `int mpfr_rint_roundeven (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)`
- [x] `int mpfr_rint_trunc (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)`

- [x] `int mpfr_frac (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)`

- [ ] `int mpfr_modf (mpfr_t iop, mpfr_t fop, mpfr_t op, mpfr_rnd_t rnd)`

- [ ] `int mpfr_fmod (mpfr_t r, mpfr_t x, mpfr_t y, mpfr_rnd_t rnd)`
- [ ] `int mpfr_fmodquo (mpfr_t r, long* q, mpfr_t x, mpfr_t y, mpfr_rnd_t rnd)`
- [ ] `int mpfr_remainder (mpfr_t r, mpfr_t x, mpfr_t y, mpfr_rnd_t rnd)`
- [ ] `int mpfr_remquo (mpfr_t r, long* q, mpfr_t x, mpfr_t y, mpfr_rnd_t rnd)`

- [x] `int mpfr_integer_p (mpfr_t op)`


### Rounding-Related Functions

- [x] `void mpfr_set_default_rounding_mode (mpfr_rnd_t rnd)`
- [x] `mpfr_rnd_t mpfr_get_default_rounding_mode (void)`

- [ ] `int mpfr_prec_round (mpfr_t x, mpfr_prec_t prec, mpfr_rnd_t rnd)`

- [ ] `int mpfr_can_round (mpfr_t b, mpfr_exp_t err, mpfr_rnd_t rnd1, mpfr_rnd_t rnd2, mpfr_prec_t prec)`

- [x] `mpfr_prec_t mpfr_min_prec (mpfr_t x)`


### Miscellaneous Functions

- [x] `void mpfr_nexttoward (mpfr_t x, mpfr_t y)`

- [x] `void mpfr_nextabove (mpfr_t x)`
- [x] `void mpfr_nextbelow (mpfr_t x)`

- [x] `int mpfr_min (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_rnd_t rnd)`
- [x] `int mpfr_max (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_rnd_t rnd)`

- [ ] `int mpfr_urandomb (mpfr_t rop, gmp_randstate_t state)`

- [ ] `int mpfr_urandom (mpfr_t rop, gmp_randstate_t state, mpfr_rnd_t rnd)`

- [ ] `int mpfr_nrandom (mpfr_t rop1, gmp_randstate_t state, mpfr_rnd_t rnd)`
- [ ] `int mpfr_grandom (mpfr_t rop1, mpfr_t rop2, gmp_randstate_t state, mpfr_rnd_t rnd)`

- [ ] `int mpfr_erandom (mpfr_t rop1, gmp_randstate_t state, mpfr_rnd_t rnd)`

- [x] `mpfr_exp_t mpfr_get_exp (mpfr_t x)`
- [ ] `int mpfr_set_exp (mpfr_t x, mpfr_exp_t e)`

- [x] `int mpfr_signbit (mpfr_t op)`

- [ ] `int mpfr_setsign (mpfr_t rop, mpfr_t op, int s, mpfr_rnd_t rnd)`

- [x] `int mpfr_copysign (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_rnd_t rnd)`

- [x] `const char * mpfr_get_version (void)`

- [x] `int mpfr_buildopt_tls_p (void)`
- [x] `int mpfr_buildopt_float128_p (void)`
- [x] `int mpfr_buildopt_decimal_p (void)`
- [x] `int mpfr_buildopt_gmpinternals_p (void)`
- [x] `int mpfr_buildopt_sharedcache_p (void)`

- [x] `const char * mpfr_buildopt_tune_case (void)`


### Exception Related Functions

- [x] `mpfr_exp_t mpfr_get_emin (void)`
- [x] `mpfr_exp_t mpfr_get_emax (void)`

- [ ] `int mpfr_set_emin (mpfr_exp_t exp)`
- [ ] `int mpfr_set_emax (mpfr_exp_t exp)`

- [x] `mpfr_exp_t mpfr_get_emin_min (void)`
- [x] `mpfr_exp_t mpfr_get_emin_max (void)`
- [x] `mpfr_exp_t mpfr_get_emax_min (void)`
- [x] `mpfr_exp_t mpfr_get_emax_max (void)`

- [ ] `int mpfr_check_range (mpfr_t x, int t, mpfr_rnd_t rnd)`

- [ ] `int mpfr_subnormalize (mpfr_t x, int t, mpfr_rnd_t rnd)`

- [x] `void mpfr_clear_underflow (void)`
- [x] `void mpfr_clear_overflow (void)`
- [x] `void mpfr_clear_divby0 (void)`
- [x] `void mpfr_clear_nanflag (void)`
- [x] `void mpfr_clear_inexflag (void)`
- [x] `void mpfr_clear_erangeflag (void)`
- [x] `void mpfr_clear_flags (void)`

- [x] `void mpfr_set_underflow (void)`
- [x] `void mpfr_set_overflow (void)`
- [x] `void mpfr_set_divby0 (void)`
- [x] `void mpfr_set_nanflag (void)`
- [x] `void mpfr_set_inexflag (void)`
- [x] `void mpfr_set_erangeflag (void)`

- [x] `int mpfr_underflow_p (void)`
- [x] `int mpfr_overflow_p (void)`
- [x] `int mpfr_divby0_p (void)`
- [x] `int mpfr_nanflag_p (void)`
- [x] `int mpfr_inexflag_p (void)`
- [x] `int mpfr_erangeflag_p (void)`

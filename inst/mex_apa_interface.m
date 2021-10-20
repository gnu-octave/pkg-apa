% Help file for the GMP and MPFR MEX-interface.
%
% Octave/Matlab MEX interface for MPFR (Version 4.1.0).
%
%    https://www.mpfr.org/mpfr-4.1.0/mpfr.html
%
% The original order of arguments is preserved.  For example
%
%    int mpfr_add (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_rnd_t rnd)
%
% can be called via m-file functions
%
%    rop = mpfr_t (zeros (3));
%    op1 = mpfr_t (ones (3));
%    op2 = mpfr_t (4 * eye (3));
%    rnd = mpfr_default_rounding_mode ();
%
%    ret = mpfr_add (rop, op1, op2, rnd)
%
% where rop, op1, op2 are @mpfr_t variables.
%

% mpfr_interface.m  Help file for mpfr_interface MEX-file.
%
% Octave/Matlab MEX interface for MPFR (Version 4.1.0).
%
%    https://www.mpfr.org/mpfr-current/mpfr.html
%
% The functions are abbreviated like
%
%    mpfr_add   --> add
%    mpfr_add_d --> add_d
%    etc.
%
% and can be called from Octave/Matlab via
%
%    mpfr_interface ("add"  , ...)
%    mpfr_interface ("add_d", ...)
%
% The original order of arguments is preserved.  For example
%
%    int mpfr_add (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_rnd_t rnd)
%
% can be called via
%
%    mpfr_interface ("add", rop, op1, op2, rnd)
%
% where rop, op1, op2 are pairs of indices to the respective internal MPFR
% variables.
%

function [ret, endptr] = mpfr_strtofr (rop, nptr, base, rnd)
% [ret, endptr] = mpfr_strtofr (rop, nptr, base, rnd)
%
% Read a floating-point number from a string NPTR in base BASE,
% rounded in the direction RND; BASE must be either 0 (to detect the
% base, as described below) or a number from 2 to 62 (otherwise the
% behavior is undefined).  If NPTR starts with valid data, the result
% is stored in ROP and ‘*ENDPTR’ points to the character just after
% the valid data (if ENDPTR is not a null pointer); otherwise ROP is
% set to zero (for consistency with ‘strtod’) and the value of NPTR
% is stored in the location referenced by ENDPTR (if ENDPTR is not a
% null pointer).  The usual ternary value is returned.
%
% Parsing follows the standard C ‘strtod’ function with some
% extensions.  After optional leading whitespace, one has a subject
% sequence consisting of an optional sign (‘+’ or ‘-’), and either
% numeric data or special data.  The subject sequence is defined as
% the longest initial subsequence of the input string, starting with
% the first non-whitespace character, that is of the expected form.
%
% The form of numeric data is a non-empty sequence of significand
% digits with an optional decimal-point character, and an optional
% exponent consisting of an exponent prefix followed by an optional
% sign and a non-empty sequence of decimal digits.  A significand
% digit is either a decimal digit or a Latin letter (62 possible
% characters), with ‘A’ = 10, ‘B’ = 11, ..., ‘Z’ = 35; case is
% ignored in bases less than or equal to 36, in bases larger than 36,
% ‘a’ = 36, ‘b’ = 37, ..., ‘z’ = 61.  The value of a significand
% digit must be strictly less than the base.  The decimal-point
% character can be either the one defined by the current locale or
% the period (the first one is accepted for consistency with the C
% standard and the practice, the second one is accepted to allow the
% programmer to provide MPFR numbers from strings in a way that does
% not depend on the current locale).  The exponent prefix can be ‘e’
% or ‘E’ for bases up to 10, or ‘@’ in any base; it indicates a
% multiplication by a power of the base.  In bases 2 and 16, the
% exponent prefix can also be ‘p’ or ‘P’, in which case the exponent,
% called _binary exponent_, indicates a multiplication by a power of
% 2 instead of the base (there is a difference only for base 16); in
% base 16 for example ‘1p2’ represents 4 whereas ‘1@2’ represents
% 256.  The value of an exponent is always written in base 10.
%
% If the argument BASE is 0, then the base is automatically detected
% as follows.  If the significand starts with ‘0b’ or ‘0B’, base 2 is
% assumed.  If the significand starts with ‘0x’ or ‘0X’, base 16 is
% assumed.  Otherwise base 10 is assumed.
%
% Note: The exponent (if present) must contain at least a digit.
% Otherwise the possible exponent prefix and sign are not part of the
% number (which ends with the significand).  Similarly, if ‘0b’,
% ‘0B’, ‘0x’ or ‘0X’ is not followed by a binary/hexadecimal digit,
% then the subject sequence stops at the character ‘0’, thus 0 is
% read.
%
% Special data (for infinities and NaN) can be ‘@inf@’ or
% ‘@nan@(n-char-sequence-opt)’, and if BASE <= 16, it can also be
% ‘infinity’, ‘inf’, ‘nan’ or ‘nan(n-char-sequence-opt)’, all case
% insensitive.  A ‘n-char-sequence-opt’ is a possibly empty string
% containing only digits, Latin letters and the underscore (0, 1, 2,
% ..., 9, a, b, ..., z, A, B, ..., Z, _).  Note: one has an optional
% sign for all data, even NaN.  For example, ‘-@nAn@(This_Is_Not_17)’
% is a valid representation for NaN in base 17.
%

  [ret, endptr] = mex_apa_interface (1217, rop, nptr, base, rnd);
end

% This function was automatically generated by "generate_m_files".

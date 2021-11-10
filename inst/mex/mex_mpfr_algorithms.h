#ifndef MEX_MPFR_ALGORITHMS_H_
#define MEX_MPFR_ALGORITHMS_H_


/**
 * Octave/Matlab MEX interface for extra MPFR algorithms.
 *
 * @param nlhs MEX parameter.
 * @param plhs MEX parameter.
 * @param nrhs MEX parameter.
 * @param prhs MEX parameter.
 * @param cmd_code code of command to execute (2000 - 2999).
 */

void
mex_mpfr_algorithms (int nlhs, mxArray *plhs[],
                     int nrhs, const mxArray *prhs[],
                     uint64_t cmd_code);


/**
 * MPFR Dot product `rop = a' * b`.
 *
 * @param rop scalar @c mpfr_ptr.
 * @param a vector @c mpfr_ptr of length @c N.
 * @param b vector @c mpfr_ptr of length @c N.
 * @param N vector length of @c a and @c b.
 * @param prec MPFR precision for intermediate operations.
 * @param rnd  MPFR rounding mode for all operations.
 *
 * @returns MPFR ternary return value (logical OR of all return values).
 */

int
mpfr_apa_dot (mpfr_ptr rop, mpfr_ptr a, mpfr_ptr b, uint64_t N,
              mpfr_prec_t prec, mpfr_rnd_t rnd);


/**
 * MPFR Matrix-Matrix-Multiplication `C = A * B`.
 *
 * @param C [M x N] @c mpfr_ptr indexed by (i,j).
 * @param A [M x K] @c mpfr_ptr indexed by (i,k).
 * @param B [K x N] @c mpfr_ptr indexed by (k,j).
 * @param prec MPFR precision for intermediate operations.
 * @param rnd MPFR rounding mode for all operations.
 * @param M Matrix dimension (see above).
 * @param N Matrix dimension (see above).
 * @param K Matrix dimension (see above).
 * @param ret_ptr pointer to array of MPFR return values.
 * @param ret_stride equals 1, if the array of MPFR return values should be
 *                   filled and has the same size as C.  Otherwise 0 for
 *                   scalar (ignored) return value.
 * @param strategy for matrix multiplication.
 */

void
mpfr_apa_mmm (mpfr_ptr C, mpfr_ptr A, mpfr_ptr B,
              mpfr_prec_t prec, mpfr_rnd_t rnd,
              uint64_t M, uint64_t N, uint64_t K,
              double *ret_ptr, size_t ret_stride, uint64_t strategy);


#endif // MEX_MPFR_ALGORITHMS_H_


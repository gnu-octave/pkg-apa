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


/**
 * MPFR LU factorization of a general M-by-N matrix A using partial pivoting
 * with row interchanges.
 *
 * The factorization has the form
 *
 * A = P * L * U
 *
 * where P is a permutation matrix, L is lower triangular with unit diagonal
 * elements (lower trapezoidal if M > N), and U is upper triangular (upper
 * trapezoidal if m < n).
 *
 * This is the right-looking version of the algorithm.
 *
 * @param M The number of rows    of the matrix @c A.  `M >= 0`.
 * @param N The number of columns of the matrix @c A.  `N >= 0`.
 * @param A MPFR matrix of dimension LDA-by-N.
 *          On entry, the M-by-N matrix to be factored.
 *          On exit, the factors L and U from the factorization
 *          `A = P*L*U`; the unit diagonal elements of L are not stored.
 * @param LDA The leading dimension of the matrix @c A.  `LDA >= max(1,M)`.
 * @param IPIV vector of length `min(M,N)`.
 *             The pivot indices; for 1 <= i <= min(M,N),
 *             row `i` of the matrix @c A was interchanged with row `IPIV(i)`.
 * @param INFO = 0:  successful exit
 *             < 0:  if INFO = -i, the i-th argument had an illegal value
 *             > 0:  if INFO = i, U(i,i) is exactly zero.  The factorization
 *                   has been completed, but the factor U is exactly
 *                   singular, and division by zero will occur if it is used
 *                   to solve a system of equations.
 * @param prec MPFR precision for intermediate operations.
 * @param rnd  MPFR rounding mode for all operations.
 * @param ret_ptr pointer to array of MPFR return values.
 * @param ret_stride equals 1, if the array of MPFR return values should be
 *                   filled and has the same size as C.  Otherwise 0 for
 *                   scalar (ignored) return value.
 *
 * @returns MPFR ternary return value @c ret_ptr (logical OR of all return
 *          values).
 */

void
mpfr_apa_GETRF (uint64_t M, uint64_t N, mpfr_ptr A, uint64_t LDA,
                uint64_t *IPIV, int *INFO, mpfr_prec_t prec, mpfr_rnd_t rnd,
                double *ret_ptr, size_t ret_stride);


/**
 * Computes the solution to a real system of linear equations
 *
 *     A * X = B,
 *
 * where A is an N-by-N matrix and X and B are N-by-NRHS matrices.
 *
 * The LU decomposition with partial pivoting and row interchanges is
 * used to factor A as
 *
 *     A = P * L * U,
 * where P is a permutation matrix, L is unit lower triangular, and U is
 * upper triangular.  The factored form of A is then used to solve the
 * system of equations A * X = B.
 *
 * @param N The number of linear equations, i.e., the order of the matrix @c A.
 *          `N >= 0`.
 * @param NRHS The number of right hand sides, i.e., the number of columns
 *             of the matrix @c B.  `NRHS >= 0`.
 * @param A MPFR matrix of dimension LDA-by-N.
 *          On entry, the N-by-N coefficient matrix A.
 *          On exit, the factors L and U from the factorization A = P*L*U;
 *          the unit diagonal elements of L are not stored.
 * @param LDA The leading dimension of the matrix @c A.  `LDA >= max(1,N)`.
 * @param IPIV vector of length @c N.
 *             The pivot indices that define the permutation matrix P;
 *             row i of the matrix was interchanged with row IPIV(i).
 * @param B MPFR matrix of dimension LDB-by-NRHS.
 *          On entry, the N-by-NRHS matrix of right hand side matrix B.
 *          On exit, if INFO = 0, the N-by-NRHS solution matrix X.
 * @param LDB The leading dimension of the matrix @c B.  `LDB >= max(1,N)`.
 * @param INFO = 0:  successful exit
 *             < 0:  if INFO = -i, the i-th argument had an illegal value
 *             > 0:  if INFO = i, U(i,i) is exactly zero.  The factorization
 *                   has been completed, but the factor U is exactly singular,
 *                   so the solution could not be computed.
 * @param prec MPFR precision for intermediate operations.
 * @param rnd  MPFR rounding mode for all operations.
 * @param ret_ptr pointer to array of MPFR return values.
 * @param ret_stride equals 1, if the array of MPFR return values should be
 *                   filled and has the same size as C.  Otherwise 0 for
 *                   scalar (ignored) return value.
 *
 * @returns MPFR ternary return value @c ret_ptr (logical OR of all return
 *          values).
 */

void
mpfr_apa_GESV (uint64_t N, uint64_t NRHS, mpfr_ptr A, uint64_t LDA,
               uint64_t *IPIV, mpfr_ptr B, uint64_t LDB, int *INFO,
               mpfr_prec_t prec, mpfr_rnd_t rnd,
               double *ret_ptr, size_t ret_stride);


#endif // MEX_MPFR_ALGORITHMS_H_


#include "mex_mpfr_interface.h"

#define MAX(a, b)                                  \
  ({ __typeof__(a)_a = (a); __typeof__(b)_b = (b); \
     _a > _b ? _a : _b; })

#define MIN(a, b)                                  \
  ({ __typeof__(a)_a = (a); __typeof__(b)_b = (b); \
     _a < _b ? _a : _b; })

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
 * @returns MPFR ternary return value (logical OR of all return values).
 */

void
mpfr_apa_GETRF (uint64_t M, uint64_t N, mpfr_ptr A, uint64_t LDA,
                uint64_t *IPIV, int *INFO, mpfr_prec_t prec, mpfr_rnd_t rnd,
                double *ret_ptr, size_t ret_stride)
{
  if (INFO == NULL)
    return;

  if (A == NULL)
    {
      *INFO = -3;
      return;
    }
  if (LDA < M)  // LDA >= max(1,M)
    {
      *INFO = -4;
      return;
    }
  if (IPIV == NULL)
    {
      *INFO = -5;
      return;
    }

  mpfr_t piv, tmp;

  mpfr_init2 (piv, prec);
  mpfr_init2 (tmp, prec);

  // Golub, Van Loan: "Matrix Computations", 4th edition, Algorithm 3.2.1
  // with rectangular matrix modification (3.2.8, p. 118)
  // and with pivot search in current column k.

  for (uint64_t k = 0; k < MIN (M, N); k++)
    {
      // Find pivot in column k.
      mpfr_abs (piv, &A[k + k * LDA], rnd);
      IPIV[k] = k;
      for (uint64_t i = k + 1; i < M; i++)
        {
          mpfr_abs (tmp, &A[i + k * LDA], rnd);
          if (mpfr_less_p (piv, tmp))
            {
              mpfr_set (piv, tmp, rnd);
              IPIV[k] = i;
            }
        }

      // STOP: if pivot is zero.
      if (mpfr_zero_p (piv))
        {
          *INFO = k;
          break;
        }

      // Pivoting: swap rows k and IPIV[k] in A.
      if (IPIV[k] != k)
        for (uint64_t j = 0; j < N; j++)
          mpfr_swap (&A[k + j * LDA], &A[IPIV[k] + j * LDA]);

      // Gaussian elimination.
      for (uint64_t i = k + 1; i < M; i++)
        {
          // A[i][k] = A[i][k] / A[k][k];
          int ret = (int) ret_ptr[(i + k * LDA) * ret_stride];
          ret |= mpfr_div (&A[i + k * LDA], &A[i + k * LDA], &A[k + k * LDA],
                           rnd);
          ret_ptr[(i + k * LDA) * ret_stride] = (double) ret;

          #pragma omp parallel
          {
            mpfr_t t;
            mpfr_init2 (t, prec);
            #pragma omp for
            for (uint64_t j = k + 1; j < N; j++)
              {
                // A[i][j] = A[i][j] - A[i][k] * A[k][j];
                int ret = (int) ret_ptr[(i + j * LDA) * ret_stride];
                ret |= mpfr_mul (t, &A[i + k * LDA], &A[k + j * LDA], rnd);
                ret |= mpfr_sub (&A[i + j * LDA], &A[i + j * LDA], t, rnd);
                ret_ptr[(i + j * LDA) * ret_stride] = (double) ret;
              }
            mpfr_clear (t);
            mpfr_free_cache ();
          }
        }
    }

  mpfr_clear (piv);
  mpfr_clear (tmp);
}



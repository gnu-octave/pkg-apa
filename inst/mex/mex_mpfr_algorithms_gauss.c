/*
 * This file is part of APA.
 *
 *  APA is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  APA is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with APA.  If not, see <https://www.gnu.org/licenses/>.
 */

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
 *             > 0:  if INFO = i, U(i,i) is exactly zero.  1-based index.
 *                   The factorization has been completed, but the factor U is
 *                   exactly singular, and division by zero will occur if it is
 *                   used to solve a system of equations.
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
  *INFO = 0;

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
          *INFO = k + 1;  // 1-based index.
          break;
        }

      // Pivoting: swap rows k and IPIV[k] in A.
      if (IPIV[k] != k)
        {
        #pragma omp parallel for
          for (uint64_t j = 0; j < N; j++)
            mpfr_swap (&A[k + j * LDA], &A[IPIV[k] + j * LDA]);
        }

      // Gaussian elimination.
      for (uint64_t i = k + 1; i < M; i++)
        {
          // A[i][k] = A[i][k] / A[k][k];
          int ret = (int) ret_ptr[(i + k * LDA) * ret_stride];
          ret |= mpfr_div (&A[i + k * LDA], &A[i + k * LDA], &A[k + k * LDA],
                           rnd);
          ret_ptr[(i + k * LDA) * ret_stride] = (double) ret;

          #pragma omp parallel for
          for (uint64_t j = k + 1; j < N; j++)
            {
              // A[i][j] = A[i][j] - A[i][k] * A[k][j];
              int ret = (int) ret_ptr[(i + j * LDA) * ret_stride];
              ret |= mpfr_fms (&A[i + j * LDA],
                               &A[i + k * LDA], &A[k + j * LDA],
                               &A[i + j * LDA], rnd);
              ret |= mpfr_neg (&A[i + j * LDA], &A[i + j * LDA], rnd);
              ret_ptr[(i + j * LDA) * ret_stride] = (double) ret;
            }
        }
    }

  mpfr_clear (piv);
  mpfr_clear (tmp);
}


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
 *             > 0:  if INFO = i, U(i,i) is exactly zero.  1-based index.
 *                   The factorization has been completed, but the factor U is
 *                   exactly singular, so the solution could not be computed.
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
               double *ret_ptr, size_t ret_stride)
{
  if (INFO == NULL)
    return;

  if (A == NULL)
    {
      *INFO = -3;
      return;
    }
  if (LDA < N)  // LDA >= max(1,N)
    {
      *INFO = -4;
      return;
    }
  if (IPIV == NULL)
    {
      *INFO = -5;
      return;
    }
  if (B == NULL)
    {
      *INFO = -6;
      return;
    }
  if (LDB < N)  // LDB >= max(1,N)
    {
      *INFO = -7;
      return;
    }

  mpfr_apa_GETRF (N, N, A, LDA, IPIV, INFO, prec, rnd, ret_ptr, ret_stride);

  // Stop if not successful.
  if (*INFO != 0)
    {
    #pragma omp parallel for
      for (uint64_t j = 0; j < NRHS; j++)
        for (uint64_t i = 0; i < N; i++)
          mpfr_set_nan (&B[i + j * LDB]);
      return;
    }

  //FIXME: handle MPFR ternary return values.

  // Apply pivot.
  for (uint64_t i = 0; i < N; i++)
    if (IPIV[i] != i)
      for (uint64_t k = 0; k < NRHS; k++)
        mpfr_swap (&B[i + k * LDB], &B[IPIV[i] + k * LDB]);

  // For each column j of B.  Forward substitution.
  for (uint64_t k = 0; k < NRHS; k++)
    for (uint64_t i = 0; i < N; i++)
      for (uint64_t j = 0; j < i; j++)
        {
          // B[i,k] = B[i,k] - A[i,j] * B[j,k]; OR
          // B[i,k] = -(A[i,j] * B[j,k] - B[i,k]);
          mpfr_fms (&B[i + k * LDB], &A[i + j * LDA], &B[j + k * LDB],
                    &B[i + k * LDB], rnd);
          mpfr_neg (&B[i + k * LDB], &B[i + k * LDB], rnd);
        }

  // For each column j of B.  Backward substitution.
  for (uint64_t k = 0; k < NRHS; k++)
    for (uint64_t i = N - 1; i < N; i--)  // Count unsigned to zero!
      {
        for (uint64_t j = i + 1; j < N; j++)
          {
            // B[i,k] = B[i,k] - A[i,j] * B[j,k]; OR
            // B[i,k] = -(A[i,j] * B[j,k] - B[i,k]);
            mpfr_fms (&B[i + k * LDB], &A[i + j * LDA], &B[j + k * LDB],
                      &B[i + k * LDB], rnd);
            mpfr_neg (&B[i + k * LDB], &B[i + k * LDB], rnd);
          }
        // b[i] /= A[i][i];
        mpfr_div (&B[i + k * LDB], &B[i + k * LDB], &A[i + i * LDA], rnd);
      }
}


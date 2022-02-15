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

#define MIN(a, b)                                  \
  ({ __typeof__(a)_a = (a); __typeof__(b)_b = (b); \
     _a < _b ? _a : _b; })

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
                     uint64_t cmd_code)
{
  switch (cmd_code)
    {
      case 2000: // int mpfr_t.transpose (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd, uint64_t ropM)
      {
        MEX_NARGINCHK (5);
        MEX_MPFR_T (1, rop);
        MEX_MPFR_T (2, op);
        if (length (&rop) != length (&op))
          MEX_FCN_ERR ("%s.\n", "cmd[mpfr_t.transpose]:op Invalid size");
        MEX_MPFR_RND_T (3, rnd);
        uint64_t ropM = 0;
        if (! extract_ui (4, nrhs, prhs, &ropM) || (ropM == 0))
          MEX_FCN_ERR ("%s\n", "cmd[mpfr_t.transpose]:ropM must be a"
                       "positive numeric scalar.");
        DBG_PRINTF ("cmd[mpfr_t.transpose]: rop = [%d:%d], op = [%d:%d], "
                    "rnd = %d, ropM = %d\n", rop.start, rop.end,
                    op.start, op.end, (int) rnd, (int) ropM);

        uint64_t ropN = length (&rop) / ropM;

        plhs[0] = mxCreateNumericMatrix (nlhs ? length (&rop) : 1, 1,
                                         mxDOUBLE_CLASS, mxREAL);
        double * ret_ptr    = mxGetPr (plhs[0]);
        mpfr_ptr rop_ptr    = &mpfr_data[rop.start - 1];
        mpfr_ptr op_ptr     = &mpfr_data[op.start - 1];
        size_t   ret_stride = (nlhs) ? 1 : 0;

        for (uint64_t i = 0; i < ropM; i++)
          for (uint64_t j = 0; j < ropN; j++)
            ret_ptr[i * ret_stride] = (double) mpfr_set (rop_ptr + j * ropM + i,
                                                         op_ptr + i * ropN + j,
                                                         rnd);
        return;
      }

      case 2001: // int mpfr_t.mtimes (mpfr_t C, mpfr_t A, mpfr_t B, mpfr_prec_t prec, mpfr_rnd_t rnd, uint64_t M, int strategy)
      {
        MEX_NARGINCHK (8);
        MEX_MPFR_T (1, C);
        MEX_MPFR_T (2, A);
        MEX_MPFR_T (3, B);
        MEX_MPFR_PREC_T (4, prec);
        MEX_MPFR_RND_T (5, rnd);
        uint64_t M = 0;
        if (! extract_ui (6, nrhs, prhs, &M) || (M == 0))
          MEX_FCN_ERR ("%s\n", "cmd[mpfr_t.mtimes]:M must be a positive "
                       "numeric scalar denoting the rows of input rop.");
        uint64_t strategy = 0;
        if (! extract_ui (7, nrhs, prhs, &strategy))
          MEX_FCN_ERR ("%s\n", "cmd[mpfr_t.mtimes]:strategy must be a "
                       "positive numeric scalar.");
        DBG_PRINTF ("cmd[mpfr_t.mtimes]: C = [%d:%d], A = [%d:%d], "
                    "B = [%d:%d], prec = %d, rnd = %d, M = %d, strategy = %d\n",
                    C.start, C.end, A.start, A.end, B.start, B.end,
                    (int) prec, (int) rnd, (int) M, (int) strategy);

        // Check matrix dimensions to be sane.
        //   C [M x N]
        //   A [M x K]
        //   B [K x N]
        uint64_t N = length (&C) / M;
        if (length (&C) != (M * N))
          MEX_FCN_ERR ("%s\n", "cmd[mpfr_t.mtimes]:M does not denote the "
                       "number of rows of input matrix C.");
        uint64_t K = length (&A) / M;
        if (length (&A) != (M * K))
          MEX_FCN_ERR ("cmd[mpfr_t.mtimes]:Incompatible matrix A.  Expected "
                       "a [%d x %d] matrix\n", M, K);
        if (length (&B) != (K * N))
          MEX_FCN_ERR ("cmd[mpfr_t.mtimes]:Incompatible matrix B.  Expected "
                       "a [%d x %d] matrix\n", K, N);

        plhs[0] = mxCreateNumericMatrix (nlhs ? length (&C) : 1, 1,
                                         mxDOUBLE_CLASS, mxREAL);
        double * ret_ptr    = mxGetPr (plhs[0]);
        mpfr_ptr C_ptr      = &mpfr_data[C.start - 1];
        mpfr_ptr A_ptr      = &mpfr_data[A.start - 1];
        mpfr_ptr B_ptr      = &mpfr_data[B.start - 1];
        size_t   ret_stride = (nlhs) ? 1 : 0;

        mpfr_apa_mmm (C_ptr, A_ptr, B_ptr, prec, rnd, M, N, K, ret_ptr,
                      ret_stride, strategy);

        return;
      }

      case 2002: // int mpfr_t.lu (mpfr_t L, mpfr_t U, mpfr_t A, mpfr_prec_t prec, mpfr_rnd_t rnd, uint64_t M)
      {
        MEX_NARGINCHK (7);
        MEX_MPFR_T (1, L);
        MEX_MPFR_T (2, U);
        MEX_MPFR_T (3, A);
        MEX_MPFR_PREC_T (4, prec);
        MEX_MPFR_RND_T (5, rnd);
        uint64_t M = 0;
        if (! extract_ui (6, nrhs, prhs, &M) || (M == 0))
          MEX_FCN_ERR ("%s\n", "cmd[mpfr_t.lu]:M must be a positive "
                       "numeric scalar denoting the rows of input rop.");
        DBG_PRINTF ("cmd[mpfr_t.lu]: L = [%d:%d], U = [%d:%d], A = [%d:%d],"
                    "prec = %d, rnd = %d, M = %d\n",
                    L.start, L.end, U.start, U.end, A.start, A.end,
                    (int) prec, (int) rnd, (int) M);

        // Check matrix dimensions to be sane.
        //   L [       M x min(M,N)]
        //   U [min(M,N) x N       ]
        //   A [       M x N       ]
        uint64_t N = length (&A) / M;
        if (length (&A) != (M * N))
          MEX_FCN_ERR ("%s\n", "cmd[mpfr_t.lu]:M does not denote the "
                       "number of rows of input matrix A.");
        uint64_t K = MIN (M, N);
        if (length (&L) != (M * K))
          MEX_FCN_ERR ("cmd[mpfr_t.lu]:Incompatible matrix L.  Expected "
                       "a [%d x %d] matrix\n", M, K);
        if (length (&U) != (K * N))
          MEX_FCN_ERR ("cmd[mpfr_t.lu]:Incompatible matrix U.  Expected "
                       "a [%d x %d] matrix\n", K, N);

        plhs[0] = mxCreateNumericMatrix ((nlhs ? M : 1), (nlhs ? N : 1),
                                         mxDOUBLE_CLASS, mxREAL);
        mpfr_ptr L_ptr      = &mpfr_data[L.start - 1];
        mpfr_ptr U_ptr      = &mpfr_data[U.start - 1];
        mpfr_ptr A_ptr      = &mpfr_data[A.start - 1];
        double * ret_ptr    = mxGetPr (plhs[0]);
        size_t   ret_stride = (nlhs) ? 1 : 0;

        // Call GETRF.
        int       INFO = -1;
        uint64_t *IPIV = (uint64_t *) mxMalloc (K * sizeof(uint64_t));
        mpfr_apa_GETRF (M, N, A_ptr, M, IPIV, &INFO, prec, rnd,
                        ret_ptr, ret_stride);
        plhs[1] = mxCreateDoubleScalar ((double) INFO);

        // Handle zero pivot.
        uint64_t K_save = ((INFO == 0) ? K : (uint64_t) INFO);

        // Copy A to U.
        #pragma omp parallel for
        for (size_t j = 0; j < N; j++)
          for (size_t i = 0; (i < (j + 1)) && (i < K_save); i++)
            mpfr_set (&U_ptr[i + j * K], &A_ptr[i + j * M], rnd);

        // Copy A to L.
        #pragma omp parallel for
        for (size_t j = 0; j < K; j++)
          mpfr_set_ui (&L_ptr[j + j * M], 1, rnd);  // Set diagonal 1.
        #pragma omp parallel for
        for (size_t j = 0; j < K_save; j++)
          for (size_t i = j + 1; i < M; i++)
            mpfr_set (&L_ptr[i + j * M], &A_ptr[i + j * M], rnd);

        // Apply IPIV to L, if not returned `[L,U] = lu(A)`.
        if (nlhs <= 2)
          {
            for (size_t i = 0; i < K_save; i++)
              if (IPIV[i] != i)
                {
                  #pragma omp parallel for
                  for (size_t j = 0; j < K; j++)
                    mpfr_swap (&L_ptr[i + j * M], &L_ptr[IPIV[i] + j * M]);
                }
          }
        else  // Return and translate pivot vector.
          {
            plhs[2] = mxCreateNumericMatrix (1, M, mxDOUBLE_CLASS, mxREAL);
            double *P = mxGetPr (plhs[2]);
            for (size_t i = 0; i < M; i++)
              P[i] = (double) (i + 1);
            for (size_t i = 0; i < K_save; i++)
              if (IPIV[i] != i)
                {
                  double d = P[i];
                  P[i]       = P[IPIV[i]];
                  P[IPIV[i]] = d;
                }
          }

        mxFree (IPIV);

        return;
      }


      case 2003: // int mpfr_t.mldivide (mpfr_t A, mpfr_t B, mpfr_prec_t prec, mpfr_rnd_t rnd)
      {
        MEX_NARGINCHK (5);
        MEX_MPFR_T (1, A);
        MEX_MPFR_T (2, B);
        MEX_MPFR_PREC_T (3, prec);
        MEX_MPFR_RND_T (4, rnd);
        DBG_PRINTF ("cmd[mpfr_t.mldivide]: A = [%d:%d], B = [%d:%d], "
                    "prec = %d, rnd = %d\n", A.start, A.end, B.start, B.end,
                    (int) prec, (int) rnd);

        // Check matrix dimensions to be sane.
        //   A [N x N]
        //   B [N x NRHS]
        uint64_t N = (uint64_t) sqrt ((double) length (&A));
        if (length (&A) != (N * N))
          MEX_FCN_ERR ("%s\n", "cmd[mpfr_t.mldivide]:A must be a square "
                       "matrix.");
        uint64_t NRHS = length (&B) / N;
        if (length (&B) != (N * NRHS))
          MEX_FCN_ERR ("cmd[mpfr_t.mldivide]:Incompatible matrix B.  Expected "
                       "a [%d x NRHS] matrix\n", N);

        plhs[0] = mxCreateNumericMatrix ((nlhs ? N : 1), (nlhs ? N : 1),
                                         mxDOUBLE_CLASS, mxREAL);
        mpfr_ptr A_ptr      = &mpfr_data[A.start - 1];
        mpfr_ptr B_ptr      = &mpfr_data[B.start - 1];
        double * ret_ptr    = mxGetPr (plhs[0]);
        size_t   ret_stride = (nlhs) ? 1 : 0;

        // Call GETRF.
        int       INFO = -1;
        uint64_t *IPIV = (uint64_t *) mxMalloc (N * sizeof(uint64_t));
        mpfr_apa_GESV (N, NRHS, A_ptr, N, IPIV, B_ptr, N, &INFO,
                       prec, rnd, ret_ptr, ret_stride);
        mxFree (IPIV);

        // Return INFO.
        plhs[1] = mxCreateDoubleScalar ((double) INFO);

        return;
      }


      case 2004: // int mpfr_t.min (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      {
        MEX_NARGINCHK (4);
        MEX_MPFR_T (1, rop);
        MEX_MPFR_T (2, op);
        MEX_MPFR_RND_T (3, rnd);
        DBG_PRINTF ("cmd[mpfr_t.min]: rop = [%d:%d], op = [%d:%d], "
                    "prec = %d, rnd = %d\n", rop.start, rop.end,
                    op.start, op.end, (int) rnd);

        // Check matrix dimensions to be sane.
        //    op [M x N]
        //   rop [1 x N]
        uint64_t N = length (&rop);
        uint64_t M = length (&op) / N;
        if (length (&op) != (M * N))
          MEX_FCN_ERR ("cmd[mpfr_t.min]:Incompatible matrix A.  Expected "
                       "a [%d x %d] matrix\n", M, N);

        plhs[0] = mxCreateNumericMatrix (N, 1, mxDOUBLE_CLASS, mxREAL);
        double * ret_ptr = mxGetPr (plhs[0]);
        mpfr_ptr rop_ptr = &mpfr_data[rop.start - 1];
        mpfr_ptr op_ptr  = &mpfr_data[op.start - 1];
        #pragma omp parallel for
        for (size_t j = 0; j < N; j++)
          {
            int ret = mpfr_set (rop_ptr + j, op_ptr + M * j, rnd);
            for (size_t i = 1; i < M; i++)
              ret |= mpfr_min (rop_ptr + j, rop_ptr + j,
                               op_ptr + M * j + i, rnd);
            ret_ptr[j] = (double) ret;
          }

        return;
      }


      default:
        MEX_FCN_ERR ("Unknown command code '%d'\n", cmd_code);
    }
}


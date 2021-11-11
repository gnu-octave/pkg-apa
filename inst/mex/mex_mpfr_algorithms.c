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

        plhs[0] = mxCreateNumericMatrix (nlhs ? length (&A) : 1, 1,
                                         mxDOUBLE_CLASS, mxREAL);
        mpfr_ptr L_ptr      = &mpfr_data[L.start - 1];
        mpfr_ptr U_ptr      = &mpfr_data[U.start - 1];
        mpfr_ptr A_ptr      = &mpfr_data[A.start - 1];
        double * ret_ptr    = mxGetPr (plhs[0]);
        size_t   ret_stride = (nlhs) ? 1 : 0;

        // Call GETRF.
        int       INFO = 0;
        uint64_t *IPIV = (uint64_t *) mxMalloc (K * sizeof(uint64_t));
        mpfr_apa_GETRF (M, N, A_ptr, M, IPIV, &INFO, prec, rnd,
                        ret_ptr, ret_stride);

        // Copy A to L.
        for (size_t j = 0; j < K; j++)
          {
            mpfr_set_ui (&L_ptr[j + j * M], 1, rnd);  // diagonal 1
            for (size_t i = j + 1; i < M; i++)
              mpfr_set (&L_ptr[i + j * M], &A_ptr[i + j * M], rnd);
          }

        // Copy A to U.
        for (size_t j = 0; j < N; j++)
          for (size_t i = 0; (i < (j + 1)) && (i < K); i++)
            mpfr_set (&U_ptr[i + j * K], &A_ptr[i + j * M], rnd);

        // Return and translate pivot vector.
        plhs[1] = mxCreateNumericMatrix (1, M, mxDOUBLE_CLASS, mxREAL);
        double *P = mxGetPr (plhs[1]);
        for (size_t i = 0; i < M; i++)
          P[i] = (double) (i + 1);
        for (size_t i = 0; i < K; i++)
          if (IPIV[i] != i)
            {
              double d = P[i];
              P[i]       = P[IPIV[i]];
              P[IPIV[i]] = d;
            }
        mxFree (IPIV);

        // Return INFO.
        plhs[2] = mxCreateDoubleScalar ((double) INFO);

        return;
      }

      default:
        MEX_FCN_ERR ("Unknown command code '%d'\n", cmd_code);
    }
}


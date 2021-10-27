#ifndef MEX_MPFR_ALGORITHMS_MMM_H_
#define MEX_MPFR_ALGORITHMS_MMM_H_

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
              mpfr_prec_t prec, mpfr_rnd_t rnd)
{
  int ret = 0;

  #pragma omp parallel shared(ret)
  {
    int    r = 0;
    mpfr_t c;
    mpfr_init_set_d (c, 0.0, prec);

    #pragma omp for
    for (uint64_t i = 0; i < N; i++)
      r |= mpfr_fma (c, b + i, a + i, c, rnd);

    // Sum `c` to `rop`, one thread at a time.
    #pragma omp critical
    {
      ret |= (r | mpfr_add (rop, rop, c, rnd));
    }
    mpfr_clear (c);
    mpfr_free_cache ();
  }

  return (ret);
}


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
              double *ret_ptr, size_t ret_stride, uint64_t strategy)
{
  switch (strategy)
    {
      case 1:  // plain for-loop ijk
        for (uint64_t i = 0; i < M; i++)
          for (uint64_t j = 0; j < N; j++)
            {
              int ret = 0;
              for (uint64_t k = 0; k < K; k++)
                ret |= mpfr_fma (C + (M * j) + i,
                                 B + k + (K * j),
                                 A + i + (M * k),
                                 C + (M * j) + i, rnd);
              ret_ptr[((M * j) + i) * ret_stride] = (double) ret;
            }
        break;


      case 2:  // plain for-loop jik
        for (uint64_t j = 0; j < N; j++)
          for (uint64_t i = 0; i < M; i++)
            {
              int ret = 0;
              for (uint64_t k = 0; k < K; k++)
                ret |= mpfr_fma (C + (M * j) + i,
                                 B + k + (K * j),
                                 A + i + (M * k),
                                 C + (M * j) + i, rnd);
              ret_ptr[((M * j) + i) * ret_stride] = (double) ret;
            }
        break;


      case 3: // 1 omp for-loop ijk
        #pragma omp parallel for
        for (uint64_t i = 0; i < M; i++)
          for (uint64_t j = 0; j < N; j++)
            {
              int ret = 0;
              for (uint64_t k = 0; k < K; k++)
                ret |= mpfr_fma (C + (M * j) + i,
                                 B + k + (K * j),
                                 A + i + (M * k),
                                 C + (M * j) + i, rnd);
              ret_ptr[((M * j) + i) * ret_stride] = (double) ret;
            }
        break;


      case 4:  // 1 omp for-loop jik
        #pragma omp parallel for
        for (uint64_t j = 0; j < N; j++)
          for (uint64_t i = 0; i < M; i++)
            {
              int ret = 0;
              for (uint64_t k = 0; k < K; k++)
                ret |= mpfr_fma (C + (M * j) + i,
                                 B + k + (K * j),
                                 A + i + (M * k),
                                 C + (M * j) + i, rnd);
              ret_ptr[((M * j) + i) * ret_stride] = (double) ret;
            }
        break;


      case 5:  // 2 omp for-loops ijk
        #pragma omp parallel for
        for (uint64_t i = 0; i < M; i++)
          {
            #pragma omp parallel for
            for (uint64_t j = 0; j < N; j++)
              {
                int ret = 0;
                for (uint64_t k = 0; k < K; k++)
                  ret |= mpfr_fma (C + (M * j) + i,
                                   B + k + (K * j),
                                   A + i + (M * k),
                                   C + (M * j) + i, rnd);
                ret_ptr[((M * j) + i) * ret_stride] = (double) ret;
              }
          }
        break;

      case 6:  // 2 omp for-loops jik
        #pragma omp parallel for
        for (uint64_t j = 0; j < N; j++)
          {
            #pragma omp parallel for
            for (uint64_t i = 0; i < M; i++)
              {
                int ret = 0;
                for (uint64_t k = 0; k < K; k++)
                  ret |= mpfr_fma (C + (M * j) + i,
                                   B + k + (K * j),
                                   A + i + (M * k),
                                   C + (M * j) + i, rnd);
                ret_ptr[((M * j) + i) * ret_stride] = (double) ret;
              }
          }
        break;


      case 7:  // 2 omp for-loops ijk, copy transpose A
      {
        // If A is a M=1 x K vector, no copy and i-loop necessary.
        if (M == 1)
          {
            #pragma omp parallel for
            for (uint64_t j = 0; j < N; j++)
              ret_ptr[j * ret_stride] = (double) mpfr_apa_dot (C + j,
                                                               A, B + (K * j),
                                                               K, prec, rnd);
            break;  // Finished
          }

        // Memory for row i of matrix A.
        mpfr_ptr Ai = (mpfr_ptr) mxMalloc (K * sizeof(mpfr_t));
        #pragma omp parallel for
        for (uint64_t k = 0; k < K; k++)
          mpfr_init2 (Ai + k, prec);

        // For each row of A and C.
        for (uint64_t i = 0; i < M; i++)
          {
            // Copy row Ai.
            #pragma omp parallel for
            for (uint64_t k = 0; k < K; k++)
              mpfr_set (Ai + k, A + i + (M * k), rnd);

            #pragma omp parallel for
            for (uint64_t j = 0; j < N; j++)
              ret_ptr[((M * j) + i) * ret_stride] = (double) mpfr_apa_dot (
                C + (M * j) + i, Ai, B + (K * j), K, prec, rnd);
          }

        // Return memory of Ai
        for (uint64_t k = 0; k < K; k++)
          mpfr_clear (Ai + k);
        mxFree (Ai);
      }
      break;


      default:
        MEX_FCN_ERR ("mpfr_mmm: invalid strategy '%d'\n", (int) strategy);
    }
}


#endif // MEX_MPFR_ALGORITHMS_MMM_H_


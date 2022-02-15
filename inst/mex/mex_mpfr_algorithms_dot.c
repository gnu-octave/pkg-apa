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
      r |= mpfr_fma (c, a + i, b + i, c, rnd);

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


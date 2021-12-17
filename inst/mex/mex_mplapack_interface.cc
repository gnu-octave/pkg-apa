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

#ifdef MPLAPACK

#include <mplapack/mpblas_mpfr.h>

#include "mex_mplapack_interface.h"

/**
 * C wrapper for mplapack C++ function Rgemm.
 *
 * @param transa parameter.
 * @param transb parameter.
 * @param m parameter.
 * @param n parameter.
 * @param k parameter.
 * @param alpha parameter.
 * @param a parameter.
 * @param lda parameter.
 * @param b parameter.
 * @param ldb parameter.
 * @param beta parameter.
 * @param c parameter.
 * @param ldc parameter.
 */

void
mplapack_Rgemm (const char *transa,
                const char *transb,
                int64_t const m,
                int64_t const n,
                int64_t const k,
                mpfr_t const alpha,
                mpfr_ptr a, int64_t const lda,
                mpfr_ptr b, int64_t const ldb,
                mpfr_t const beta,
                mpfr_ptr c, int64_t const ldc)
{
  Rgemm (transa, transb,
         static_cast <mplapackint>(m),
         static_cast <mplapackint>(n),
         static_cast <mplapackint>(k),
         static_cast <mpreal>(alpha),
         static_cast <mpreal *>(a), static_cast <mplapackint>(lda),
         static_cast <mpreal *>(b), static_cast <mplapackint>(ldb),
         static_cast <mpreal>(beta),
         static_cast <mpreal *>(c), static_cast <mplapackint>(ldc));
}


#endif


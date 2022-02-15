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

#ifndef MEX_MPFR_INTERFACE_H_
#define MEX_MPFR_INTERFACE_H_

#include <inttypes.h> // intmax_t, etc. in mpfr.h

#include "mpfr.h"
#include "mex_apa_interface.h"
#include "mex_mpfr_algorithms.h"


// MPFR memory management
// ======================
//
// mex_mpfr_algorithms.c
//
// Similar to C++ std::vector:
// - xxx_capacity: number of elements that `xxx` has currently allocated
//                 space for.
// - xxx_size:     number of elements in `xxx`.

extern mpfr_ptr mpfr_data;
extern size_t   mpfr_data_capacity;
extern size_t   mpfr_data_size;


/**
 * Octave/Matlab MEX interface for MPFR.
 *
 * @param nlhs MEX parameter.
 * @param plhs MEX parameter.
 * @param nrhs MEX parameter.
 * @param prhs MEX parameter.
 * @param cmd_code code of command to execute (1000 - 1999).
 */
void
mex_mpfr_interface (int nlhs, mxArray *plhs[],
                    int nrhs, const mxArray *prhs[],
                    uint64_t cmd_code);


/**
 * Safely declare and read MPFR_T variable(-arrays) from MEX interface.
 *
 * @param mex_rhs Position (0-based) in MEX input.
 * @param name    Desired variable name.
 */
#define MEX_MPFR_T(mex_rhs, name)                                     \
  idx_t name;                                                         \
  if (! extract_idx ((mex_rhs), nrhs, prhs, &name))                   \
    MEX_FCN_ERR ("cmd[%d]:"#name " Invalid MPFR variable indices.\n", \
                 cmd_code);


/**
 * Safely declare and read MPFR_RND_T variable from MEX interface.
 *
 * @param mex_rhs Position (0-based) in MEX input.
 * @param name    Desired variable name.
 */
#define MEX_MPFR_RND_T(mex_rhs, name)                              \
  mpfr_rnd_t name = mpfr_get_default_rounding_mode ();             \
  if (! extract_rounding_mode ((mex_rhs), nrhs, prhs, &name))      \
    MEX_FCN_ERR ("cmd[%d]:"#name " Rounding must be a one of "     \
                 "MPFR_RNDD, MPFR_RNDN, MPFR_RNDU, MPFR_RNDZ, or " \
                 "MPFR_RNDA.\n", cmd_code);


/**
 * Safely declare and read MPFR_PREC_T variable from MEX interface.
 *
 * @param mex_rhs Position (0-based) in MEX input.
 * @param name    Desired variable name.
 */
#define MEX_MPFR_PREC_T(mex_rhs, name)                           \
  mpfr_prec_t name = mpfr_get_default_prec ();                   \
  if (! extract_prec ((mex_rhs), nrhs, prhs, &name))             \
    MEX_FCN_ERR ("cmd[%d]:"#name " Precision must be a numeric " \
                 "scalar between %ld and %ld.\n", cmd_code,      \
                 MPFR_PREC_MIN, MPFR_PREC_MAX);


/**
 * Safely declare and read MPFR_EXP_T variable from MEX interface.
 *
 * @param mex_rhs Position (0-based) in MEX input.
 * @param name    Desired variable name.
 */
#define MEX_MPFR_EXP_T(mex_rhs, name)                             \
  mpfr_exp_t name = mpfr_get_emin ();                             \
  {                                                               \
    int64_t si = 0;                                               \
    if (! extract_si ((mex_rhs), nrhs, prhs, &si))                \
      MEX_FCN_ERR ("cmd[%d]:"#name " Exponent must be a numeric " \
                   "scalar between %ld and %ld.\n", cmd_code,     \
                   mpfr_get_emin (), mpfr_get_emax ());           \
    name = (mpfr_exp_t) si;                                       \
  }


/**
 * Check for valid index range.
 *
 * @param[in] idx Pointer to index (1-based, idx_t) of MPFR variables.
 *
 * @returns `0` if `idx` is invalid, otherwise `idx` is valid.
 */
int
is_valid (idx_t *idx);


/**
 * Safely read MPFR index (idx_t) structure.
 *
 * @param[in] idx MEX input position index (0 is first).
 * @param[in] nrhs Number of right-hand sides.
 * @param[in] mxArray  MEX input array.
 * @param[out] idx_vec If function returns `1`, `idx_vec` contains a valid
 *                     index (idx_t) structure extracted from the MEX input,
 *                     otherwise `idx_vec` remains unchanged.
 *
 * @returns success of extraction.
 */
int
extract_idx (int idx, int nrhs, const mxArray *prhs[], idx_t *idx_vec);


/**
 * Safely read MPFR rounding mode from MEX input.
 *
 * @param[in] idx MEX input position index (0 is first).
 * @param[in] nrhs Number of right-hand sides.
 * @param[in] mxArray  MEX input array.
 * @param[out] rnd If function returns `1`, `rnd` contains the MPFR rounding
 *                 mode extracted from the MEX input, otherwise `rnd` remains
 *                 unchanged.
 *
 * Rounding mode translation (mpfr.h)
 * ==================================
 *
 *  0 = MPFR_RNDD: round toward minus infinity
 *                 (roundTowardNegative in IEEE 754-2008).
 *  1 = MPFR_RNDN: round to nearest, with the even rounding rule
 *                 (roundTiesToEven in IEEE 754-2008); see details below.
 *  2 = MPFR_RNDU: round toward plus infinity
 *                 (roundTowardPositive in IEEE 754-2008).
 *  3 = MPFR_RNDZ: round toward zero (roundTowardZero in IEEE 754-2008).
 *  4 = MPFR_RNDA: round away from zero.
 *
 * @returns success of extraction.
 */
int
extract_rounding_mode (int idx, int nrhs, const mxArray *prhs[],
                       mpfr_rnd_t *rnd);


/**
 * Safely read MPFR precision from MEX input.
 *
 * @param[in] idx MEX input position index (0 is first).
 * @param[in] nrhs Number of right-hand sides.
 * @param[in] mxArray  MEX input array.
 * @param[out] prec If function returns `1`, `prec` contains the MPFR precision
 *                  extracted from the MEX input, otherwise `prec` remains
 *                  unchanged.
 *
 * @returns success of extraction.
 */
int
extract_prec (int idx, int nrhs, const mxArray *prhs[], mpfr_prec_t *prec);


/**
 * Constructor for new MPFR variables.
 *
 * @param[in] count Number of MPFR variables to create.
 * @param[out] idx If function returns `1`, pointer to index (1-based, idx_t)
 *                 of MPFR variables, otherwise the value of `idx` remains
 *                 unchanged.
 *
 * @returns success of MPFR variable creation.
 */
int
mex_mpfr_allocate (size_t count, idx_t *idx);


/**
 * Mark MPFR variable as no longer used.
 *
 * @param[in] idx Pointer to index (1-based, idx_t) of MPFR variable to be no
 *                longer used.
 *
 * @returns success of MPFR variables creation.
 */
void
mex_mpfr_mark_free (idx_t *idx);

#endif  // MEX_MPFR_INTERFACE_H_


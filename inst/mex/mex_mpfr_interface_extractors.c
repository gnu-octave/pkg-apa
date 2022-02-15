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
extract_idx (int idx, int nrhs, const mxArray *prhs[], idx_t *idx_vec)
{
  if (idx >= nrhs)
    {
      DBG_PRINTF ("extract_idx: idx(%d) >= nrhs(%d).\n", idx, nrhs);
      return (0);
    }

  const mxArray *mpfr_t_idx = NULL;

  if (mxIsClass (prhs[idx], "mpfr_t"))
    {
      mpfr_t_idx = mxGetProperty (prhs[idx], 0, "idx");
      if (mpfr_t_idx == NULL)
        {
          DBG_PRINTF ("mpfr_t object in prhs[%d] has no 'idx' field.\n", idx);
          return (0);
        }
    }
  else
    mpfr_t_idx = prhs[idx];

  if (mxIsNumeric (mpfr_t_idx)
      && ((mxGetM (mpfr_t_idx) * mxGetN (mpfr_t_idx) == 2)))
    {
      double *vec = mxGetPr (mpfr_t_idx);
      if ((vec[0] >= 0.0) && mxIsFinite (vec[0]) && (floor (vec[0]) == vec[0])
          && (vec[1] >= 0.0) && mxIsFinite (vec[1])
          && (floor (vec[1]) == vec[1]))
        {
          idx_t tmp_vec = { (size_t) vec[0], (size_t) vec[1] };
          if (is_valid (&tmp_vec))
            {
              (*idx_vec).start = tmp_vec.start;
              (*idx_vec).end   = tmp_vec.end;
              return (1);
            }

          DBG_PRINTF ("Invalid index [%d:%d].\n", (*idx_vec).start,
                      (*idx_vec).end);
        }
    }

  DBG_PRINTF ("extract_idx: %s\n", "Failed.");
  return (0);
}


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
                       mpfr_rnd_t *rnd)
{
  int64_t si = 0;

  if (extract_si (idx, nrhs, prhs, &si))
    switch (si)
      {
        case MPFR_RNDD:
        case MPFR_RNDN:
        case MPFR_RNDU:
        case MPFR_RNDZ:
        case MPFR_RNDA:
          *rnd = si;
          return (1);

        default:
          break;
      }
  return (0);
}


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
extract_prec (int idx, int nrhs, const mxArray *prhs[], mpfr_prec_t *prec)
{
  uint64_t ui = 0;

  if (extract_ui (idx, nrhs, prhs, &ui) && (MPFR_PREC_MIN < ui)
      && (ui < MPFR_PREC_MAX))
    {
      *prec = (mpfr_prec_t) ui;
      return (1);
    }
  return (0);
}


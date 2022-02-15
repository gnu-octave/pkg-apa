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

#include "mex_gmp_interface.h"


/**
 * Octave/Matlab MEX interface for GMP library.
 *
 * @param nlhs MEX parameter.
 * @param plhs MEX parameter.
 * @param nrhs MEX parameter.
 * @param prhs MEX parameter.
 * @param cmd_code code of command to execute (1000 - 1999).
 */
void
mex_gmp_interface (int nlhs, mxArray *plhs[],
                   int nrhs, const mxArray *prhs[],
                   uint64_t cmd_code)
{
  nlhs += (prhs - prhs);  // FIXME: Avoid unused variable warning.

  #if (! defined(__GNU_MP_VERSION) || (__GNU_MP_VERSION < 6))
  # error "Oldest supported GMP version is 6."
  #endif

  switch (cmd_code)
    {
      case 3000:  // const char * const gmp_version
      {
        MEX_NARGINCHK (1);
        char *output_buf = (char *) mxCalloc (strlen (gmp_version),
                                              sizeof(char));
        strcpy (output_buf, gmp_version);
        plhs[0] = mxCreateString (output_buf);
        return;
      }

      default:
        MEX_FCN_ERR ("Unknown command code '%d'\n", cmd_code);
    }
}


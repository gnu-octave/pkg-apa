#ifndef MEX_GMP_INTERFACE_H_
#define MEX_GMP_INTERFACE_H_


#include "mex_apa_interface.h"
#include "gmp.h"

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
                   uint64_t cmd_code);

#endif  // MEX_GMP_INTERFACE_H_

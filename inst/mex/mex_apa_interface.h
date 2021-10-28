#ifndef MEX_APA_INTERFACE_H_
#define MEX_APA_INTERFACE_H_


#include <math.h>
#include <stdint.h>
#include <string.h>

#include <omp.h>

#include "mex.h"


#define DATA_CHUNK_SIZE    1000 // Allocate variables in chunks.


// State deciding about the output verbosity level
// - level = 0: no output at all (including no error messages)
// - level = 1: show error messages
// - level = 2: show error messages and precision warnings [default]
// - level = 3: very verbose debug output.
static int VERBOSE = 2;


// Macro to immediately return from current function.
// Depending on `VERBOSE` variable an error message is printed.

#define MEX_FCN_ERR(fmt, ...)                                   \
  { if (VERBOSE > 0)                                            \
      mexErrMsgIdAndTxt ("apa:mexFunction",                     \
                         "%s:%d:%s():" fmt, __FILE__, __LINE__, \
                         __func__, __VA_ARGS__);                \
    return; }

// Macro to print very verbose debug output, depends on `VERBOSE` variable.
#define DBG_PRINTF(fmt, ...)                                \
  { if (VERBOSE > 2)                                        \
      mexPrintf ("DBG %s:%d:%s():" fmt, __FILE__, __LINE__, \
                 __func__, __VA_ARGS__); }


/**
 * Check number or input arguments.
 *
 * @param num Desired number of MEX `nrhs`.
 */

#define MEX_NARGINCHK(num) \
  if (nrhs != (num))       \
    MEX_FCN_ERR ("cmd[%d]: Invalid number of arguments.\n", cmd_code);


// Data type to handle index ranges.

typedef struct
{
  size_t start;
  size_t end;
} idx_t;


/**
 * Get length of index range.
 *
 * @param[in] idx Pointer index range.
 *
 * @returns length of index range.
 */

inline size_t
length (idx_t *idx)
{
  return (idx->end - idx->start + 1);
}


/**
 * Safely read numeric double scalar from MEX input.
 *
 * @param[in] idx MEX input position index (0 is first).
 * @param[in] nrhs Number of right-hand sides.
 * @param[in] mxArray  MEX input array.
 * @param[out] d If function returns `1`, `d` contains the double scalar
 *               extracted from the MEX input, otherwise `d` remains
 *               unchanged.
 *
 * @returns success of extraction.
 */

int
extract_d (int idx, int nrhs, const mxArray *prhs[], double *d);


/**
 * Safely read scalar signed integer (si) from MEX input.
 *
 * @param[in] idx MEX input position index (0 is first).
 * @param[in] nrhs Number of right-hand sides.
 * @param[in] mxArray  MEX input array.
 * @param[out] si If function returns `1`, `si` contains the scalar signed
 *                integer extracted from the MEX input, otherwise `si` remains
 *                unchanged.
 *
 * @returns success of extraction.
 */

int
extract_si (int idx, int nrhs, const mxArray *prhs[], int64_t *si);


/**
 * Safely read scalar unsigned integer (ui) from MEX input.
 *
 * @param[in] idx MEX input position index (0 is first).
 * @param[in] nrhs Number of right-hand sides.
 * @param[in] mxArray  MEX input array.
 * @param[out] ui If function returns `1`, `ui` contains the scalar unsigned
 *                integer extracted from the MEX input, otherwise `ui` remains
 *                unchanged.
 *
 * @returns success of extraction.
 */

int
extract_ui (int idx, int nrhs, const mxArray *prhs[], uint64_t *ui);


/**
 * Safely read vector of unsigned integer (ui) from MEX input.
 *
 * @param[in] idx MEX input position index (0 is first).
 * @param[in] nrhs Number of right-hand sides.
 * @param[in] mxArray  MEX input array.
 * @param[in] len Expected vector length.
 * @param[out] ui If function returns `1`, `ui` contains a vector of unsigned
 *                integers of length `len` extracted from the MEX input,
 *                otherwise `ui` is NULL.
 *
 * @returns success of extraction.
 */

int
extract_ui_vector (int idx, int nrhs, const mxArray *prhs[], uint64_t **ui,
                   size_t len);

#endif  // MEX_APA_INTERFACE_H_


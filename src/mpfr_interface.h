#ifndef MPFR_INTERFACE_H_
#define MPFR_INTERFACE_H_


#include <math.h>
#include <stdint.h>
#include <string.h>

#include "mex.h"
#include "mpfr.h"


#define DATA_CHUNK_SIZE 1000  // Allocate space for MPFR variables in chunks.

#define DEBUG 0
#define DBG_PRINTF(fmt, ...) \
        do { if (DEBUG) mexPrintf ("DBG %s:%d:%s():" fmt, __FILE__, \
                                   __LINE__, __func__, __VA_ARGS__); } while (0)

// State deciding about the verbosity level
//   0 print no MEX_FCN_ERR to stdout
//   1 print    MEX_FCN_ERR to stdout
static int VERBOSE = 1;

// Macro to print a message to stdout and set mexFunction error state variable.
// IMPORTANT: Only call within mexFunction!!
#define MEX_FCN_ERR(fmt, ...) \
        do { if (VERBOSE) mexPrintf ("%s:%d:%s():" fmt, __FILE__, __LINE__, \
                                     __func__, __VA_ARGS__); throw_error = 1; \
           } while (0)

#define MEX_NARGINCHK(num, cmd_code) \
        if (nrhs != (num)) { \
          MEX_FCN_ERR ("cmd[%d]: Invalid number of arguments.\n", (cmd_code)); \
          break; \
        }

// MPFR memory management
// ======================
//
// Similar to C++ std::vector:
// - data_capacity: number of elements that `data` has currently allocated
//                  space for.
// - data_size:     number of elements in `data`.

static mpfr_ptr data = NULL;
static size_t data_capacity = 0;
static size_t data_size = 0;


// Data type to handle index ranges.

typedef struct
{
  size_t start;
  size_t end;
} idx_t;


/**
 * Check for valid index range.
 *
 * @param[in] idx Pointer to index (1-based, idx_t) of MPFR variables.
 *
 * @returns `0` if `idx` is invalid, otherwise `idx` is valid.
 */

int
is_valid (idx_t *idx)
{
  return ((1 <= (*idx).start) && ((*idx).start <= (*idx).end)
          && ((*idx).end <= data_size));
}


/**
 * Get length of index range.
 *
 * @param[in] idx Pointer to index (1-based, idx_t) of MPFR variables.
 *
 * @returns `0` if `idx` is invalid, otherwise `idx` is valid.
 */

inline size_t
length (idx_t *idx)
{
  return ((*idx).end - (*idx).start + 1);
}


/**
 * Function called at exit of the mex file to tidy up all memory.
 * After calling this function the initial state is restored.
 */

static void
mpfr_tidy_up (void)
{
  DBG_PRINTF ("%s\n", "Call");
  for (size_t i = 0; i < data_size; i++)
    mpfr_clear (&data[i]);
  mxFree (data);
  data = NULL;
  data_capacity = 0;
  data_size = 0;
}


/**
 * Constructor for new MPFR variables.
 *
 * @param[in] count Number of MPFR variables to create.
 * @param[out] idx If function returns `1`, pointer to index (1-based, idx_t)
 *                 of MPFR variables, otherwise the value of `idx` remains
 *                 unchanged.
 *
 * @returns success of MPFR variables creation.
 */

int
mex_mpfr_allocate (size_t count, idx_t *idx)
{
  // Check for trivial case, failure as indices do not make sense.
  if (count == 0)
    return 0;

  // Check if there is enough space to create new MPFR variables.
  if ((data_size + count) > data_capacity)
    {
      // Determine new capacity.
      size_t data_new_capacity = data_capacity;
      while ((data_size + count) > data_new_capacity)
        data_new_capacity += DATA_CHUNK_SIZE;

      DBG_PRINTF ("Increase capacity to '%d'.\n", data_new_capacity);
      // Reallocate memory.
      if (data == NULL)
        {
          data = (mpfr_ptr) mxMalloc (data_new_capacity * sizeof (mpfr_t));
          mexAtExit (mpfr_tidy_up);
        }
      else
        data = (mpfr_ptr) mxRealloc (data, data_new_capacity * sizeof (mpfr_t));
      if (data == NULL)
        return 0;  // Memory allocation failed.
      mexMakeMemoryPersistent (data);
      data_capacity = data_new_capacity;
    }

  // Initialize new MPFR variables and increase number of elements in `data`.
  for (size_t i = 0; i < count; i++)
    mpfr_init (&data[data_size + i]);

  (*idx).start = data_size + 1;
  data_size += count;
  (*idx).end = data_size;
  DBG_PRINTF ("New MPFR variables [%d:%d] allocated.\n",
              (*idx).start, (*idx).end);

  return is_valid (idx);
}


/**
 * Safely read numeric double scalar from MEX input.
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
extract_d (int idx, int nrhs, const mxArray *prhs[], double *d)
{
  if ((nrhs > idx) && mxIsScalar (prhs[idx]) && mxIsNumeric (prhs[idx]))
    {
      *d = mxGetScalar (prhs[idx]);
      return 1;
    }
  DBG_PRINTF ("%s\n", "Failed.");
  return 0;
}


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
extract_si (int idx, int nrhs, const mxArray *prhs[], int64_t *si)
{
  double d = 0.0;
  if (extract_d (idx, nrhs, prhs, &d) && mxIsFinite (d) && (floor (d) == d))
    {
      *si = (int64_t) d;
      return 1;
    }
  DBG_PRINTF ("%s\n", "Failed.");
  return 0;
}


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
extract_ui (int idx, int nrhs, const mxArray *prhs[], uint64_t *ui)
{
  int64_t si = 0.0;
  if (extract_si (idx, nrhs, prhs, &si) && (si >= 0))
    {
      *ui = (uint64_t) si;
      return 1;
    }
  DBG_PRINTF ("%s\n", "Failed.");
  return 0;
}


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
                   size_t len)
{
  if ((nrhs > idx) && mxIsNumeric (prhs[idx])
      && ((mxGetM (prhs[idx]) * mxGetN (prhs[idx]) >= len)))
    {
      double* vec = mxGetPr(prhs[idx]);
      *ui = (uint64_t*) mxMalloc (len * sizeof (uint64_t));
      int good = 1;
      for (size_t i = 0; i < len; i++)
        {
          if ((vec[i] >= 0.0) && mxIsFinite (vec[i])
              && (floor (vec[i]) == vec[i]))
            (*ui)[i] = (uint64_t) vec[i];
          else
            {
              good = 0;
              break;
            }
        }
      if (good)
        return 1;
      mxFree (*ui);  // In case of an error free space.
      *ui = NULL;
    }
  DBG_PRINTF ("%s\n", "Failed.");
  return 0;
}


/**
 * Safely read an index (idx_t) structure.
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
extract_idx (int idx, int nrhs, const mxArray *prhs[], idx_t* idx_vec)
{
  uint64_t *ui = NULL;
  if (extract_ui_vector (idx, nrhs, prhs, &ui, 2))
    {
      (*idx_vec).start = ui[0];
      (*idx_vec).end   = ui[1];
      mxFree (ui);
      if (is_valid (idx_vec))
        return 1;
      DBG_PRINTF ("Invalid index [%d:%d].\n", (*idx_vec).start, (*idx_vec).end);
    }
  DBG_PRINTF ("%s\n", "Failed.");
  return 0;
}


// Rounding mode translation
// =========================
//
// -1.0 = MPFR_RNDD: round toward minus infinity
//                   (roundTowardNegative in IEEE 754-2008).
//  0.0 = MPFR_RNDN: round to nearest, with the even rounding rule
//                   (roundTiesToEven in IEEE 754-2008); see details below.
//  1.0 = MPFR_RNDU: round toward plus infinity
//                   (roundTowardPositive in IEEE 754-2008).
//  2.0 = MPFR_RNDZ: round toward zero (roundTowardZero in IEEE 754-2008).
//  3.0 = MPFR_RNDA: round away from zero.


/**
 * Export MPFR rounding mode to `double`.
 *
 * @param[in] rnd MPFR rounding mode.
 *
 * @returns `double` representation of rounding mode.  In case of an invalid
 *          rounding mode `rnd`, `FP_NAN` is returned.
 */

double
export_rounding_mode (mpfr_rnd_t rnd)
{
  double d = FP_NAN;
  switch (rnd)
    {
      case MPFR_RNDD:
        d = -1.0;
        break;
      case MPFR_RNDN:
        d = 0.0;
        break;
      case MPFR_RNDU:
        d = 1.0;
        break;
      case MPFR_RNDZ:
        d = 2.0;
        break;
      case MPFR_RNDA:
        d = 3.0;
        break;
      default:
        DBG_PRINTF ("%s\n", "Failed.");
        d = FP_NAN;
        break;
    }
  return d;
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
 * @returns success of extraction.
 */

int
extract_rounding_mode (int idx, int nrhs, const mxArray *prhs[], mpfr_rnd_t *rnd)
{
  int64_t si = 0;
  if (extract_si (idx, nrhs, prhs, &si))
    {
      switch (si)
        {
          case -1:
            *rnd = MPFR_RNDD;
            return 1;
          case 0:
            *rnd = MPFR_RNDN;
            return 1;
          case 1:
            *rnd = MPFR_RNDU;
            return 1;
          case 2:
            *rnd = MPFR_RNDZ;
            return 1;
          case 3:
            *rnd = MPFR_RNDA;
            return 1;
          default:
            break;
        }
    }
  return 0;
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
      return 1;
    }
  return 0;
}

#endif  // MPFR_INTERFACE_H_

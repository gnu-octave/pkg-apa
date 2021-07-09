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
 * @param[in] prec MPFR precision of the new MPFR variables.
 * @param[out] idx If function returns `1`, pointer to index (1-based, idx_t)
 *                 of MPFR variables, otherwise the value of `idx` remains
 *                 unchanged.
 *
 * @returns success of MPFR variables creation.
 */

int
mpfr_create (size_t count, mpfr_prec_t prec, idx_t *idx)
{
  DBG_PRINTF ("%s\n", "Call");
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

      DBG_PRINTF ("Increase capacity to '%d'\n", data_new_capacity);
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
    mpfr_init2 (&data[data_size + i], prec);

  (*idx).start = data_size + 1;
  data_size += count;
  (*idx).end = data_size;
  DBG_PRINTF ("New MPFR variable(s) [%d:%d] (prec = %d)\n", (*idx).start,
              (*idx).end, (int) prec);

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


/**
 * MEX entrance routine.
 */

void
mexFunction (int nlhs, mxArray *plhs[],
             int nrhs, const mxArray *prhs[])
{
  // Check for sufficient input parameters
  if ((nrhs < 1) || ! mxIsChar (prhs[0])) {
    mexErrMsgIdAndTxt ("mp:mexFunction", "First input must be a string.");
  }
  // Read command to execute.
  size_t buf_len = (mxGetM (prhs[0]) * mxGetN (prhs[0])) + 1;
  char *cmd_buf = mxCalloc (buf_len, sizeof (char));
  cmd_buf = mxArrayToString(prhs[0]);

  // Marker if error should be thrown.
  int throw_error = 0;

  // Index vector for MPFR variables.
  size_t* idx_vec = NULL;
  DBG_PRINTF ("Command: '%s'\n", cmd_buf);
  do
    {
      /**
       * size_t get_data_capacity (void)
       *
       * MPFR memory management helper function.
       */
      if (strcmp (cmd_buf, "get_data_capacity") == 0)
        {
          if (nrhs != 1)
            {
              MEX_FCN_ERR ("%s: Invalid number of arguments.\n", cmd_buf);
              break;
            }
          plhs[0] = mxCreateDoubleScalar ((double) data_capacity);
        }

      /**
       * size_t get_data_size (void)
       *
       * MPFR memory management helper function.
       */
      else if (strcmp (cmd_buf, "get_data_size") == 0)
        {
          if (nrhs != 1)
            {
              MEX_FCN_ERR ("%s: Invalid number of arguments.\n", cmd_buf);
              break;
            }
          plhs[0] = mxCreateDoubleScalar ((double) data_size);
        }

      /**
       * void set_verbose (int level)
       *
       *   0 print no MEX_FCN_ERR to stdout
       *   1 print    MEX_FCN_ERR to stdout
       */
      else if (strcmp (cmd_buf, "set_verbose") == 0)
        {
          if (nrhs != 2)
            {
              MEX_FCN_ERR ("%s: Invalid number of arguments.\n", cmd_buf);
              break;
            }
          int64_t level = 1;
          if (extract_si (1, nrhs, prhs, &level)
              && ((level == 0) || (level == 1)))
            VERBOSE = (int) level;
          else
            MEX_FCN_ERR ("%s: VERBOSE must be 0 or 1.\n", cmd_buf);
        }

      /**
       * mpfr_prec_t mpfr_get_default_prec (void)
       *
       * Return the current default MPFR precision in bits.  See the
       * documentation of mpfr_set_default_prec.
       */
      else if (strcmp (cmd_buf, "get_default_prec") == 0)
        {
          if (nrhs != 1)
            {
              MEX_FCN_ERR ("%s: Invalid number of arguments.\n", cmd_buf);
              break;
            }
          plhs[0] = mxCreateDoubleScalar ((double) mpfr_get_default_prec ());
        }

      /**
       * void mpfr_set_default_prec (mpfr_prec_t prec)
       *
       * Set the default precision to be exactly prec bits, where prec can be
       * any integer between MPFR_PREC_MIN and MPFR_PREC_MAX.  The precision of
       * a variable means the number of bits used to store its significand. All
       * subsequent calls to mpfr_init or mpfr_inits will use this precision,
       * but previously initialized variables are unaffected.  The default
       * precision is set to 53 bits initially.
       */
      else if (strcmp (cmd_buf, "set_default_prec") == 0)
        {
          if (nrhs != 2)
            {
              MEX_FCN_ERR ("%s: Invalid number of arguments.\n", cmd_buf);
              break;
            }
          mpfr_prec_t prec = mpfr_get_default_prec ();
          if (extract_prec (1, nrhs, prhs, &prec))
            mpfr_set_default_prec (prec);
          else
            MEX_FCN_ERR ("%s: Precision must be a numeric scalar between "
                         "%ld and %ld.\n", cmd_buf, MPFR_PREC_MIN,
                         MPFR_PREC_MAX);
        }

      /**
       * mpfr_rnd_t mpfr_get_default_rounding_mode (void)
       *
       * Get the default rounding mode.
       */
      else if (strcmp (cmd_buf, "get_default_rounding_mode") == 0)
        {
          if (nrhs != 1)
            {
              MEX_FCN_ERR ("%s: Invalid number of arguments.\n", cmd_buf);
              break;
            }
          plhs[0] = mxCreateDoubleScalar (export_rounding_mode (
            mpfr_get_default_rounding_mode ()));
        }

      /**
       * void mpfr_set_default_rounding_mode (mpfr_rnd_t rnd)
       *
       * Set the default rounding mode to rnd.  The default rounding mode is
       * to nearest initially.
       */
      else if (strcmp (cmd_buf, "set_default_rounding_mode") == 0)
        {
          if (nrhs != 2)
            {
              MEX_FCN_ERR ("%s: Invalid number of arguments.\n", cmd_buf);
              break;
            }
          mpfr_rnd_t rnd = mpfr_get_default_rounding_mode ();
          if (extract_rounding_mode (1, nrhs, prhs, &rnd))
            mpfr_set_default_rounding_mode (rnd);
          else
            MEX_FCN_ERR ("%s: Rounding must be a numeric scalar between "
                         "-1 and 3.\n", cmd_buf);
        }

      /**
       * void mpfr_init2 (mpfr_t x, mpfr_prec_t prec)
       *
       * Initialize x, set its precision to be exactly prec bits and its value
       * to NaN. (Warning: the corresponding MPF function initializes to zero
       * instead.)
       *
       * Normally, a variable should be initialized once only or at least be
       * cleared, using mpfr_clear, between initializations.  To change the
       * precision of a variable that has already been initialized, use
       * mpfr_set_prec or mpfr_prec_round; note that if the precision is
       * decreased, the unused memory will not be freed, so that it may be
       * wise to choose a large enough initial precision in order to avoid
       * reallocations. The precision prec must be an integer between
       * MPFR_PREC_MIN and MPFR_PREC_MAX (otherwise the behavior is undefined).
       */
      else if (strcmp (cmd_buf, "init2") == 0)
        {
          if (nrhs != 3)
            {
              MEX_FCN_ERR ("%s: Invalid number of arguments.\n", cmd_buf);
              break;
            }
          size_t count = 0;
          if (! extract_ui (1, nrhs, prhs, &count))
            {
              MEX_FCN_ERR ("%s: Count must be a positive numeric scalar.\n",
                           cmd_buf);
              break;
            }
          mpfr_prec_t prec = mpfr_get_default_prec ();
          if (! extract_prec (2, nrhs, prhs, &prec))
            {
              MEX_FCN_ERR ("%s: Precision must be a numeric scalar between "
                           "%ld and %ld.\n", cmd_buf, MPFR_PREC_MIN,
                          MPFR_PREC_MAX);
              break;
            }

          idx_t idx;
          if (! mpfr_create (count, prec, &idx))
            {
              MEX_FCN_ERR ("%s\n", "Memory allocation failed.");
              break;
            }
          // Return start and end indices (1-based).
          plhs[0] = mxCreateNumericMatrix (2, 1, mxDOUBLE_CLASS, mxREAL);
          double* ptr = mxGetPr (plhs[0]);
          ptr[0] = (double) idx.start;
          ptr[1] = (double) idx.end;
        }

      /**
       * int mpfr_set_d (mpfr_t rop, double op, mpfr_rnd_t rnd)
       *
       * Set the value of rop from op, rounded toward the given direction rnd.
       * Note that the input 0 is converted to +0 by mpfr_set_ui, mpfr_set_si,
       * mpfr_set_uj, mpfr_set_sj, The mpfr_set_float128 function is built
       * only with the configure option ‘--enable-float128’, which requires
       * the compiler or system provides the ‘_Float128’ data type (GCC 4.3
       * or later supports this data type); to use mpfr_set_float128, one
       * should define the macro MPFR_WANT_FLOAT128 before including mpfr.h.
       * mpfr_set_z, mpfr_set_q and mpfr_set_f, regardless of the rounding mode.
       * If the system does not support the IEEE 754 standard, mpfr_set_flt,
       * mpfr_set_d, mpfr_set_ld, mpfr_set_decimal64 and mpfr_set_decimal128
       * might not preserve the signed zeros. The mpfr_set_decimal64 and
       * mpfr_set_decimal128 functions are built only with the configure
       * option ‘--enable-decimal-float’, and when the compiler or system
       * provides the ‘_Decimal64’ and ‘_Decimal128’ data type; to use those
       * functions, one should define the macro MPFR_WANT_DECIMAL_FLOATS
       * before including mpfr.h. mpfr_set_q might fail if the numerator
       * (or the denominator) cannot be represented as a mpfr_t.
       *
       * For mpfr_set, the sign of a NaN is propagated in order to mimic the
       * IEEE 754 copy operation.  But contrary to IEEE 754, the NaN flag is
       * set as usual.
       *
       * Note: If you want to store a floating-point constant to a mpfr_t,
       * you should use mpfr_set_str (or one of the MPFR constant functions,
       * such as mpfr_const_pi for Pi) instead of mpfr_set_flt, mpfr_set_d,
       * mpfr_set_ld, mpfr_set_decimal64 or mpfr_set_decimal128.  Otherwise
       * the floating-point constant will be first converted into a reduced-
       * precision (e.g., 53-bit) binary (or decimal, for mpfr_set_decimal64
       * and mpfr_set_decimal128) number before MPFR can work with it.
       */
      else if (strcmp (cmd_buf, "set_d") == 0)
        {
          if (nrhs != 4)
            {
              MEX_FCN_ERR ("%s: Invalid number of arguments.\n", cmd_buf);
              break;
            }
          idx_t idx;
          if (! extract_idx (1, nrhs, prhs, &idx))
            {
              MEX_FCN_ERR ("%s: Invalid MPFR variable indices.\n", cmd_buf);
              break;
            }
          if (! mxIsDouble (prhs[2])
              || ((mxGetM (prhs[2]) * mxGetN (prhs[2])) != length (&idx)))
            {
              MEX_FCN_ERR ("%s: Invalid number of double values.\n", cmd_buf);
              break;
            }
          double* op_vec = mxGetPr (prhs[2]);
          mpfr_rnd_t rnd = mpfr_get_default_rounding_mode ();
          if (! extract_rounding_mode (3, nrhs, prhs, &rnd))
            {
              MEX_FCN_ERR ("%s: Rounding must be a numeric scalar between "
                           "-1 and 3.\n", cmd_buf);
              break;
            }
          DBG_PRINTF ("Set values to '%d:%d'\n", idx.start, idx.end);
          for (size_t i = 0; i < length (&idx); i++)
            mpfr_set_d (&data[(idx.start - 1) + i], op_vec[i], rnd);
        }

      /**
       * double mpfr_get_d (mpfr_t op, mpfr_rnd_t rnd)
       *
       * Convert op to a float (respectively double, long double, _Decimal64,
       * or _Decimal128) using the rounding mode rnd.
       * If op is NaN, some fixed NaN (either quiet or signaling) or the result
       * of 0.0/0.0 is returned.
       * If op is ±Inf, an infinity of the same sign or the result of ±1.0/0.0
       * is returned.
       * If op is zero, these functions return a zero, trying to preserve its
       * sign, if possible.
       * The mpfr_get_float128, mpfr_get_decimal64 and mpfr_get_decimal128
       * functions are built only under some conditions: see the documentation
       * of mpfr_set_float128, mpfr_set_decimal64 and mpfr_set_decimal128
       * respectively.
       */
      else if (strcmp (cmd_buf, "get_d") == 0)
        {
          if (nrhs != 3)
            {
              MEX_FCN_ERR ("%s: Invalid number of arguments.\n", cmd_buf);
              break;
            }
          idx_t idx;
          if (! extract_idx (1, nrhs, prhs, &idx))
            {
              MEX_FCN_ERR ("%s: Invalid MPFR variable indices.\n", cmd_buf);
              break;
            }
          mpfr_rnd_t rnd = mpfr_get_default_rounding_mode ();
          if (! extract_rounding_mode (2, nrhs, prhs, &rnd))
            {
              MEX_FCN_ERR ("%s: Rounding must be a numeric scalar between "
                           "-1 and 3.\n", cmd_buf);
              break;
            }
          plhs[0] = mxCreateNumericMatrix (length (&idx), 1, mxDOUBLE_CLASS,
                                           mxREAL);
          double* op_vec = mxGetPr (plhs[0]);
          for (size_t i = 0; i < length (&idx); i++)
            op_vec[i] = mpfr_get_d (&data[(idx.start - 1) + i], rnd);
        }

      /**
       * int mpfr_add (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_rnd_t rnd)
       *
       * Set rop to op1 + op2 rounded in the direction rnd.  The IEEE 754
       * rules are used, in particular for signed zeros.  But for types having
       * no signed zeros, 0 is considered unsigned (i.e., (+0) + 0 = (+0) and
       * (-0) + 0 = (-0)).  The mpfr_add_d function assumes that the radix of
       * the double type is a power of 2, with a precision at most that
       * declared by the C implementation (macro IEEE_DBL_MANT_DIG, and if not
       * defined 53 bits).
       */
      else if (strcmp (cmd_buf, "add") == 0)
        {
          if (nrhs != 5)
            {
              MEX_FCN_ERR ("%s: Invalid number of arguments.\n", cmd_buf);
              break;
            }
          idx_t rop;
          if (! extract_idx (1, nrhs, prhs, &rop))
            {
              MEX_FCN_ERR ("%s:rop Invalid MPFR variable indices.\n", cmd_buf);
              break;
            }
          idx_t op1;
          if (! extract_idx (2, nrhs, prhs, &op1)
              || (length (&rop) != length (&op1)))
            {
              MEX_FCN_ERR ("%s:op1 Invalid MPFR variable indices.\n", cmd_buf);
              break;
            }
          idx_t op2;
          if (! extract_idx (3, nrhs, prhs, &op2)
              || (length (&rop) != length (&op2)))
            {
              MEX_FCN_ERR ("%s:op2 Invalid MPFR variable indices.\n", cmd_buf);
              break;
            }
          mpfr_rnd_t rnd = mpfr_get_default_rounding_mode ();
          if (! extract_rounding_mode (4, nrhs, prhs, &rnd))
            {
              MEX_FCN_ERR ("%s: Rounding must be a numeric scalar between "
                           "-1 and 3.\n", cmd_buf);
              break;
            }
          DBG_PRINTF ("Add '%d:%d' = '%d:%d' + '%d:%d' using %d\n",
                      rop.start, rop.end, op1.start, op1.end,
                      op2.start, op2.end, (int) rnd);
          for (size_t i = 0; i < length (&rop); i++)
            mpfr_add (&data[(rop.start - 1) + i], &data[(op1.start - 1) + i],
                      &data[(op2.start - 1) + i], rnd);
        }
      else
        MEX_FCN_ERR ("Unknown command '%s'\n", cmd_buf);
    }
  while (0);

  // Tidy up.
  mxFree (cmd_buf);

  if (idx_vec != NULL)
    mxFree (idx_vec);

  if (throw_error)
    mexErrMsgIdAndTxt ("mp:mexFunction", "See previous error message.");
}

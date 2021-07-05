#include <math.h>
#include <string.h>

#include "mex.h"
#include "mpfr.h"

#define DATA_CHUNK_SIZE 1000  // Allocate space for MPFR variables in chunks.

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


// Function called at exit of the mex file to tidy up all memory.

static void
mpfr_tidy_up (void)
{
  mexPrintf ("__mpfr__(mpfr_tidy_up): Call.\n");
  for (size_t i = 0; i < data_size; i++)
    mpfr_clear (&data[i]);
  mxFree (data);
}


// Constructor for new MPFR variables.
//
// @returns 1-based start and end index of the MPFR variables `idx_start` and
//          `idx_end`.

int mpfr_create (size_t count, mpfr_prec_t prec,
                 size_t* idx_start, size_t* idx_end)
{
  mexPrintf ("__mpfr__(mpfr_create): Call.\n");
  // Check if there is enough space to create new MPFR variables.
  if ((data_size + count) > data_capacity)
    {
      // Determine new capacity.
      size_t data_new_capacity = data_capacity;
      while ((data_size + count) > data_new_capacity)
        data_new_capacity += DATA_CHUNK_SIZE;

      mexPrintf ("__mpfr__(mpfr_create): Increase capacity to '%d'\n",
        data_new_capacity);
      // Realloate memory.
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
  
  *idx_start = data_size + 1;
  data_size += count;
  *idx_end = data_size;
  mexPrintf ("__mpfr__(mpfr_create): New MPFR variable(s) '%d:%d'"
    " (prec = %d)\n", *idx_start, *idx_end, (int) prec);

  return 1;
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


mpfr_rnd_t
rounding_mode_double2mpfr (double dbl_rnd_mode)
{
  mpfr_rnd_t rnd_mode = MPFR_RNDN;
  switch ((int) dbl_rnd_mode)
    {
      case -1:
        rnd_mode = MPFR_RNDD;
        break;
      case 0:
        rnd_mode = MPFR_RNDN;
        break;
      case 1:
        rnd_mode = MPFR_RNDU;
        break;
      case 2:
        rnd_mode = MPFR_RNDZ;
        break;
      case 3:
        rnd_mode = MPFR_RNDA;
        break;
      default:
        mexPrintf ("__mpfr__: Invalid rounding mode '%d'.  "
          "Using default rounding mode.\n", (int) dbl_rnd_mode);
        rnd_mode = mpfr_get_default_rounding_mode ();
        break;
    }
  return rnd_mode;
}


double
rounding_mode_mpfr2double (mpfr_rnd_t mpfr_rnd_mode)
{
  double rnd_mode = FP_NAN;
  switch (mpfr_rnd_mode)
    {
      case MPFR_RNDD:
        rnd_mode = -1.0;
        break;
      case MPFR_RNDN:
        rnd_mode = 0.0;
        break;
      case MPFR_RNDU:
        rnd_mode = 1.0;
        break;
      case MPFR_RNDZ:
        rnd_mode = 2.0;
        break;
      case MPFR_RNDA:
        rnd_mode = 3.0;
        break;
      default:
        rnd_mode = FP_NAN;
        break;
    }
  return rnd_mode;
}


// Safely convert a double to MPFR precision.

mpfr_prec_t
double2prec (double d)
{
  mpfr_prec_t p = (mpfr_prec_t) d;

  if (mxIsFinite (d) && (p > MPFR_PREC_MIN) && (p < MPFR_PREC_MAX))
    return p;

  mexPrintf ("__mpfr__: Precision must be a numeric scalar beween %ld and %ld.\n",
             MPFR_PREC_MIN, MPFR_PREC_MAX);
  return mpfr_get_default_prec ();
}


// MEX entrance routine.

void
mexFunction (int nlhs, mxArray *plhs[],
             int nrhs, const mxArray *prhs[])
{
  // Check for sufficient input parameters
  if ((nrhs < 1) || ! mxIsChar (prhs[0])) {
    mexErrMsgIdAndTxt ("mp:mex:rhs", "First input must be a string.");
  }
  // Read command to execute.
  size_t buf_len = (mxGetM (prhs[0]) * mxGetN (prhs[0])) + 1;
  char *cmd_buf = mxCalloc (buf_len, sizeof (char));
  cmd_buf = mxArrayToString(prhs[0]);

  mexPrintf("Command: '%s'\n", cmd_buf);
  
  if (strcmp (cmd_buf, "get_default_prec") == 0)
    plhs[0] = mxCreateDoubleScalar ((double) mpfr_get_default_prec ());
  else if (strncmp (cmd_buf, "set_default_prec",
                     strlen ("set_default_prec")) == 0)
    {
      if ((nrhs == 2) && mxIsScalar (prhs[1]) && mxIsNumeric (prhs[1]))
        mpfr_set_default_prec (double2prec (mxGetScalar (prhs[1])));
      else
        mexPrintf ("__mpfr__(%s): Precision must be a numeric scalar.\n",
                   cmd_buf);
    }
  else if (strcmp (cmd_buf, "get_default_rounding_mode") == 0)
    plhs[0] = mxCreateDoubleScalar (
                rounding_mode_mpfr2double (mpfr_get_default_rounding_mode ()));
  else if (strcmp (cmd_buf, "set_default_rounding_mode") == 0)
    {
      if ((nrhs == 2) && mxIsScalar (prhs[1]) && mxIsNumeric (prhs[1]))
        mpfr_set_default_rounding_mode (
          rounding_mode_double2mpfr (mxGetScalar (prhs[1])));
      else
        mexPrintf ("__mpfr__(%s): Rounding mode must be a numeric scalar.\n",
                   cmd_buf);
    }
  else if (strcmp (cmd_buf, "init2") == 0)
    {
      if (nrhs != 3)
        mexPrintf ("__mpfr__(%s): Invalid number of arguments.\n", cmd_buf);
      else if (! (mxIsScalar (prhs[1]) && mxIsNumeric (prhs[1])))
        mexPrintf ("__mpfr__(%s): Count must be a positive numeric scalar.\n",
                   cmd_buf);
      else if (! mxIsScalar (prhs[2]) || ! mxIsNumeric (prhs[2]))
        mexPrintf ("__mpfr__(%s): Precision must be a numeric scalar.\n",
                   cmd_buf);
      else
        {
          mpfr_prec_t prec = double2prec (mxGetScalar (prhs[2]));
          double dcount = mxGetScalar (prhs[1]);
          size_t count = (size_t) dcount;
          if ((count > 0) && (dcount > 0.0) && mxIsFinite (dcount))
            {
              size_t idx_start = 0;
              size_t idx_end = 0;
              if (mpfr_create (count, prec, &idx_start, &idx_end)
                  && (idx_start > 0) && (idx_end >= idx_start)
                  && ((idx_end - idx_start + 1) == count))
                {
                  plhs[0] = mxCreateNumericMatrix (count, 1, mxDOUBLE_CLASS,
                                                   mxREAL);
                  double* ptr = mxGetPr (plhs[0]);
                  for (size_t i = 0; i < count; i++)
                    ptr[i] = (double) idx_start++;
                }
              else
                mexPrintf ("__mpfr__(%s): Memory allocation failed.\n",
                           cmd_buf);
            }
          else
            mexPrintf ("__mpfr__(%s): Count must be a positive numeric scalar.\n",
                       cmd_buf);
        }
    }
  else if (strcmp (cmd_buf, "data_capacity") == 0)
    plhs[0] = mxCreateDoubleScalar ((double) data_capacity);
  else if (strcmp (cmd_buf, "data_size") == 0)
    plhs[0] = mxCreateDoubleScalar ((double) data_size);
  else if (strcmp (cmd_buf, "set_d") == 0)
    {
      if (nrhs != 4)
        mexPrintf ("__mpfr__(%s): Invalid number of arguments.\n", cmd_buf);
      else if (! mxIsNumeric (prhs[1]))
        mexPrintf ("__mpfr__(%s): Invalid MPFR variable indices.\n", cmd_buf);
      else if (! mxIsScalar (prhs[3]) || ! mxIsNumeric (prhs[3]))
        mexPrintf ("__mpfr__(%s): Rounding mode must be a numeric scalar.\n",
                   cmd_buf);
      else
        {
          double idx = mxGetScalar (prhs[1]);
          size_t data_0_idx = ((size_t) idx) - 1;
          double val = mxGetScalar (prhs[2]);
          if ((data_size > data_0_idx) && (idx >= 1.0) && mxIsFinite (idx))
            idx = mpfr_set_d (&data[data_0_idx], val,
                              rounding_mode_double2mpfr (mxGetScalar (prhs[3])));
              
          else
            mexPrintf ("__mpfr__(%s): Invalid MPFR variable indices.\n",
                       cmd_buf);
        }
    }
  else if (strcmp (cmd_buf, "get_d") == 0)
    {
      if (nrhs != 3)
        mexPrintf ("__mpfr__(%s): Invalid number of arguments.\n", cmd_buf);
      else if (! mxIsNumeric (prhs[1]))
        mexPrintf ("__mpfr__(%s): Invalid MPFR variable indices.\n", cmd_buf);
      else if (! mxIsScalar (prhs[2]) || ! mxIsNumeric (prhs[2]))
        mexPrintf ("__mpfr__(%s): Rounding mode must be a numeric scalar.\n",
                   cmd_buf);
      else
        {
          double idx = mxGetScalar (prhs[1]);
          size_t data_0_idx = ((size_t) idx) - 1;
          if ((data_size > data_0_idx) && (idx >= 1.0) && mxIsFinite (idx))
            idx = mpfr_get_d (&data[data_0_idx],
                              rounding_mode_double2mpfr (mxGetScalar (prhs[2])));
              
          else
            {
              idx = FP_NAN;
              mexPrintf ("__mpfr__(%s): Invalid MPFR variable indices.\n",
                         cmd_buf);
            }
          plhs[0] = mxCreateDoubleScalar (idx);
        }
    }
  else
    mexPrintf ("__mpfr__: Unknown command '%s'\n", cmd_buf);

  mxFree (cmd_buf);
}

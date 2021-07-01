#include <math.h>
#include <string.h>

#include "mex.h"
#include "mpfr.h"


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
          "Using round to nearest 'MPFR_RNDN'.\n", (int) dbl_rnd_mode);
        rnd_mode = MPFR_RNDN;
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


mpfr_prec_t
double2prec (double d)
{
  mpfr_prec_t p = (mpfr_prec_t) d;

  if (mxIsFinite (d) && (p > MPFR_PREC_MIN) && (p < MPFR_PREC_MAX))
    return p;

  mexPrintf ("__mpfr__: Precision must be a numeric scalar beween %d and %d.\n",
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
  
  if (strncmp (cmd_buf, "get_default_prec",
                strlen ("get_default_prec")) == 0)
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
  else if (strncmp (cmd_buf, "get_default_rounding_mode",
                     strlen ("get_default_rounding_mode")) == 0)
    plhs[0] = mxCreateDoubleScalar (
                rounding_mode_mpfr2double (mpfr_get_default_rounding_mode ()));
  else if (strncmp (cmd_buf, "set_default_rounding_mode",
                     strlen ("set_default_rounding_mode")) == 0)
    {
      if ((nrhs == 2) && mxIsScalar (prhs[1]) && mxIsNumeric (prhs[1]))
        mpfr_set_default_rounding_mode (
          rounding_mode_double2mpfr (mxGetScalar (prhs[1])));
      else
        mexPrintf ("__mpfr__(%s): Rounding mode must be a numeric scalar.\n",
                   cmd_buf);
    }
  else if (strncmp (cmd_buf, "mp", strlen ("mp")) == 0)
    {
      if ((nrhs < 2) || (nrhs > 3))
        mexPrintf ("__mpfr__(%s): Too few arguments.\n", cmd_buf);
      else
        {
          mpfr_rnd_t rnd_mode = ((nrhs == 3) && mxIsScalar (prhs[2]))
                              ? rounding_mode_double2mpfr (
                                  mxGetScalar (prhs[2]))
                              : mpfr_get_default_rounding_mode ();
        }
    }
  else
    mexPrintf ("__mpfr__: Unknown command '%s'\n", cmd_buf);

  mxFree (cmd_buf);
}


//TODO: int mexAtExit(void (*ExitFcn)(void));
//      void mexMakeMemoryPersistent(void *ptr);
//      void mexLock(void);
//      void mexUnlock(void);
//      bool mexIsLocked(void);
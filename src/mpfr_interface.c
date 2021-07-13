#include "mpfr_interface.h"

/**
 * Octave/Matlab MEX interface for MPFR (Version 4.1.0).
 *
 * https://www.mpfr.org/mpfr-current/mpfr.html
 *
 * See mpfr_interface.m for documentation.
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
  DBG_PRINTF ("Command: [%d] = %s(%d)\n", nlhs, cmd_buf, nrhs);
  do
    {
      /*
      ========================
      == Non-MPFR functions ==
      ========================
      */


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
       * idx_t mex_mpfr_allocate (size_t count)
       *
       * Allocate memory for `count` new MPFR variables and initialize them to
       * default precision.
       *
       * Returns the start and end index (1-based) of the internally newly
       * created MPFR variables.
       */
      else if (strcmp (cmd_buf, "allocate") == 0)
        {
          if (nrhs != 2)
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

          DBG_PRINTF ("allocate '%d' new MPFR variables\n", count);
          idx_t idx;
          if (! mex_mpfr_allocate (count, &idx))
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


      /*
      ====================
      == MPFR functions ==
      ====================
      */


      /**
       * mpfr_prec_t mpfr_get_default_prec (void)
       * mpfr_rnd_t mpfr_get_default_rounding_mode (void)
       * int mpfr_buildopt_tls_p (void)
       * int mpfr_buildopt_float128_p (void)
       * int mpfr_buildopt_decimal_p (void)
       * int mpfr_buildopt_gmpinternals_p (void)
       * int mpfr_buildopt_sharedcache_p (void)
       * mpfr_exp_t mpfr_get_emin (void)
       * mpfr_exp_t mpfr_get_emax (void)
       * mpfr_exp_t mpfr_get_emin_min
       * mpfr_exp_t mpfr_get_emin_max
       * mpfr_exp_t mpfr_get_emax_min
       * mpfr_exp_t mpfr_get_emax_max
       * void mpfr_clear_underflow (void)
       * void mpfr_clear_overflow (void)
       * void mpfr_clear_divby0 (void)
       * void mpfr_clear_nanflag (void)
       * void mpfr_clear_inexflag (void)
       * void mpfr_clear_erangeflag (void)
       * void mpfr_clear_flags (void)
       * void mpfr_set_underflow (void)
       * void mpfr_set_overflow (void)
       * void mpfr_set_divby0 (void)
       * void mpfr_set_nanflag (void)
       * void mpfr_set_inexflag (void)
       * void mpfr_set_erangeflag (void)
       * int mpfr_underflow_p (void)
       * int mpfr_overflow_p (void)
       * int mpfr_divby0_p (void)
       * int mpfr_nanflag_p (void)
       * int mpfr_inexflag_p (void)
       * int mpfr_erangeflag_p (void)
       * const char * mpfr_get_version (void)
       * const char * mpfr_buildopt_tune_case (void)
       */
      else if ((strcmp (cmd_buf, "get_default_prec") == 0)
               || (strcmp (cmd_buf, "get_default_rounding_mode") == 0)
               || (strcmp (cmd_buf, "buildopt_tls_p") == 0)
               || (strcmp (cmd_buf, "buildopt_float128_p") == 0)
               || (strcmp (cmd_buf, "buildopt_decimal_p") == 0)
               || (strcmp (cmd_buf, "buildopt_gmpinternals_p") == 0)
               || (strcmp (cmd_buf, "buildopt_sharedcache_p") == 0)
               || (strcmp (cmd_buf, "get_emin") == 0)
               || (strcmp (cmd_buf, "get_emax") == 0)
               || (strcmp (cmd_buf, "get_emin_min") == 0)
               || (strcmp (cmd_buf, "get_emin_max") == 0)
               || (strcmp (cmd_buf, "get_emax_min") == 0)
               || (strcmp (cmd_buf, "get_emax_max") == 0)
               || (strcmp (cmd_buf, "clear_underflow") == 0)
               || (strcmp (cmd_buf, "clear_overflow") == 0)
               || (strcmp (cmd_buf, "clear_divby0") == 0)
               || (strcmp (cmd_buf, "clear_nanflag") == 0)
               || (strcmp (cmd_buf, "clear_inexflag") == 0)
               || (strcmp (cmd_buf, "clear_erangeflag") == 0)
               || (strcmp (cmd_buf, "clear_flags") == 0)
               || (strcmp (cmd_buf, "set_underflow") == 0)
               || (strcmp (cmd_buf, "set_overflow") == 0)
               || (strcmp (cmd_buf, "set_divby0") == 0)
               || (strcmp (cmd_buf, "set_nanflag") == 0)
               || (strcmp (cmd_buf, "set_inexflag") == 0)
               || (strcmp (cmd_buf, "set_erangeflag") == 0)
               || (strcmp (cmd_buf, "underflow_p") == 0)
               || (strcmp (cmd_buf, "overflow_p") == 0)
               || (strcmp (cmd_buf, "divby0_p") == 0)
               || (strcmp (cmd_buf, "nanflag_p") == 0)
               || (strcmp (cmd_buf, "inexflag_p") == 0)
               || (strcmp (cmd_buf, "erangeflag_p") == 0)
               || (strcmp (cmd_buf, "get_version") == 0)
               || (strcmp (cmd_buf, "buildopt_tune_case") == 0))
        {
          if (nrhs != 1)
            {
              MEX_FCN_ERR ("%s: Invalid number of arguments.\n", cmd_buf);
              break;
            }
          if (strcmp (cmd_buf, "get_default_prec") == 0)
            plhs[0] = mxCreateDoubleScalar ((double) mpfr_get_default_prec ());
          else if (strcmp (cmd_buf, "get_default_rounding_mode") == 0)
            plhs[0] = mxCreateDoubleScalar (export_rounding_mode (
              mpfr_get_default_rounding_mode ()));
          else if (strcmp (cmd_buf, "buildopt_tls_p") == 0)
            plhs[0] = mxCreateDoubleScalar ((double) mpfr_buildopt_tls_p ());
          else if (strcmp (cmd_buf, "buildopt_float128_p") == 0)
            plhs[0] = mxCreateDoubleScalar ((double)
                                            mpfr_buildopt_float128_p ());
          else if (strcmp (cmd_buf, "buildopt_decimal_p") == 0)
            plhs[0] = mxCreateDoubleScalar ((double)
                                            mpfr_buildopt_decimal_p ());
          else if (strcmp (cmd_buf, "buildopt_gmpinternals_p") == 0)
            plhs[0] = mxCreateDoubleScalar ((double)
                                            mpfr_buildopt_gmpinternals_p ());
          else if (strcmp (cmd_buf, "buildopt_sharedcache_p") == 0)
            plhs[0] = mxCreateDoubleScalar ((double)
                                            mpfr_buildopt_sharedcache_p ());
          else if (strcmp (cmd_buf, "get_emin") == 0)
            plhs[0] = mxCreateDoubleScalar ((double) mpfr_get_emin ());
          else if (strcmp (cmd_buf, "get_emax") == 0)
            plhs[0] = mxCreateDoubleScalar ((double) mpfr_get_emax ());
          else if (strcmp (cmd_buf, "get_emin_min") == 0)
            plhs[0] = mxCreateDoubleScalar ((double) mpfr_get_emin_min ());
          else if (strcmp (cmd_buf, "get_emin_max") == 0)
            plhs[0] = mxCreateDoubleScalar ((double) mpfr_get_emin_max ());
          else if (strcmp (cmd_buf, "get_emax_min") == 0)
            plhs[0] = mxCreateDoubleScalar ((double) mpfr_get_emax_min ());
          else if (strcmp (cmd_buf, "get_emax_max") == 0)
            plhs[0] = mxCreateDoubleScalar ((double) mpfr_get_emax_max ());
          else if (strcmp (cmd_buf, "clear_underflow") == 0)
            mpfr_clear_underflow ();
          else if (strcmp (cmd_buf, "clear_overflow") == 0)
            mpfr_clear_overflow ();
          else if (strcmp (cmd_buf, "clear_divby0") == 0)
            mpfr_clear_divby0 ();
          else if (strcmp (cmd_buf, "clear_nanflag") == 0)
            mpfr_clear_nanflag ();
          else if (strcmp (cmd_buf, "clear_inexflag") == 0)
            mpfr_clear_inexflag ();
          else if (strcmp (cmd_buf, "clear_erangeflag") == 0)
            mpfr_clear_erangeflag ();
          else if (strcmp (cmd_buf, "clear_flags") == 0)
            mpfr_clear_flags ();
          else if (strcmp (cmd_buf, "set_underflow") == 0)
            mpfr_set_underflow ();
          else if (strcmp (cmd_buf, "set_overflow") == 0)
            mpfr_set_overflow ();
          else if (strcmp (cmd_buf, "set_divby0") == 0)
            mpfr_set_divby0 ();
          else if (strcmp (cmd_buf, "set_nanflag") == 0)
            mpfr_set_nanflag ();
          else if (strcmp (cmd_buf, "set_inexflag") == 0)
            mpfr_set_inexflag ();
          else if (strcmp (cmd_buf, "set_erangeflag") == 0)
            mpfr_set_erangeflag ();
          else if (strcmp (cmd_buf, "underflow_p") == 0)
            plhs[0] = mxCreateDoubleScalar ((double) mpfr_underflow_p ());
          else if (strcmp (cmd_buf, "overflow_p") == 0)
            plhs[0] = mxCreateDoubleScalar ((double) mpfr_overflow_p ());
          else if (strcmp (cmd_buf, "divby0_p") == 0)
            plhs[0] = mxCreateDoubleScalar ((double) mpfr_divby0_p ());
          else if (strcmp (cmd_buf, "nanflag_p") == 0)
            plhs[0] = mxCreateDoubleScalar ((double) mpfr_nanflag_p ());
          else if (strcmp (cmd_buf, "inexflag_p") == 0)
            plhs[0] = mxCreateDoubleScalar ((double) mpfr_inexflag_p ());
          else if (strcmp (cmd_buf, "erangeflag_p") == 0)
            plhs[0] = mxCreateDoubleScalar ((double) mpfr_erangeflag_p ());
          else if (strcmp (cmd_buf, "get_version") == 0)
            {
              char* output_buf = (char*) mxCalloc (
                strlen (mpfr_get_version ()), sizeof(char));
              strcpy (output_buf, mpfr_get_version ());
              plhs[0] = mxCreateString (output_buf);
            }
          else if (strcmp (cmd_buf, "buildopt_tune_case") == 0)
            {
              char* output_buf = (char*) mxCalloc (
                strlen (mpfr_buildopt_tune_case ()), sizeof(char));
              strcpy (output_buf, mpfr_buildopt_tune_case ());
              plhs[0] = mxCreateString (output_buf);
            }
          else
            {
              MEX_FCN_ERR ("%s: Bad operator.\n", cmd_buf);
              break;
            }
        }

      /**
       * void mpfr_set_default_prec (mpfr_prec_t prec)
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
       * void mpfr_set_default_rounding_mode (mpfr_rnd_t rnd)
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
       * mpfr_prec_t mpfr_get_prec (mpfr_t x)
       * mpfr_prec_t mpfr_min_prec (mpfr_t x)
       */
      else if ((strcmp (cmd_buf, "get_prec") == 0)
               || (strcmp (cmd_buf, "min_prec") == 0))
        {
          if (nrhs != 2)
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

          DBG_PRINTF ("%s [%d:%d]\n", cmd_buf, idx.start, idx.end);
          plhs[0] = mxCreateNumericMatrix (length (&idx), 1, mxDOUBLE_CLASS,
                                           mxREAL);
          double* plhs_0_pr = mxGetPr (plhs[0]);

          mpfr_prec_t (*fcn)(const mpfr_t);
          if (strcmp (cmd_buf, "get_prec") == 0)
            fcn = mpfr_get_prec;
          else if (strcmp (cmd_buf, "min_prec") == 0)
            fcn = mpfr_min_prec;
          else
            {
              MEX_FCN_ERR ("%s: Bad operator.\n", cmd_buf);
              break;
            }

          for (size_t i = 0; i < length (&idx); i++)
            plhs_0_pr[i] = (double) fcn (&data[(idx.start - 1) + i]);
        }

      /**
       * mpfr_exp_t mpfr_get_exp (mpfr_t x)
       */
      else if (strcmp (cmd_buf, "get_exp") == 0)
        {
          if (nrhs != 2)
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

          DBG_PRINTF ("%s [%d:%d]\n", cmd_buf, idx.start, idx.end);
          plhs[0] = mxCreateNumericMatrix (length (&idx), 1, mxDOUBLE_CLASS,
                                           mxREAL);
          double* plhs_0_pr = mxGetPr (plhs[0]);

          for (size_t i = 0; i < length (&idx); i++)
            plhs_0_pr[i] = (double) mpfr_get_exp (&data[(idx.start - 1) + i]);
        }

      /**
       * int mpfr_nan_p (mpfr_t op)
       * int mpfr_inf_p (mpfr_t op)
       * int mpfr_number_p (mpfr_t op)
       * int mpfr_zero_p (mpfr_t op)
       * int mpfr_regular_p (mpfr_t op)
       * int mpfr_sgn (mpfr_t op)
       * int mpfr_integer_p (mpfr_t op)
       * int mpfr_signbit (mpfr_t op)
       */
      else if ((strcmp (cmd_buf, "nan_p") == 0)
               || (strcmp (cmd_buf, "inf_p") == 0)
               || (strcmp (cmd_buf, "number_p") == 0)
               || (strcmp (cmd_buf, "zero_p") == 0)
               || (strcmp (cmd_buf, "regular_p") == 0)
               || (strcmp (cmd_buf, "sgn") == 0)
               || (strcmp (cmd_buf, "integer_p") == 0)
               || (strcmp (cmd_buf, "signbit") == 0))
        {
          if (nrhs != 2)
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

          DBG_PRINTF ("%s [%d:%d]\n", cmd_buf, idx.start, idx.end);
          plhs[0] = mxCreateNumericMatrix (length (&idx), 1, mxDOUBLE_CLASS,
                                           mxREAL);
          double* plhs_0_pr = mxGetPr (plhs[0]);

          int (*fcn)(const mpfr_t);
          if (strcmp (cmd_buf, "nan_p") == 0)
            fcn = mpfr_nan_p;
          else if (strcmp (cmd_buf, "inf_p") == 0)
            fcn = mpfr_inf_p;
          else if (strcmp (cmd_buf, "number_p") == 0)
            fcn = mpfr_number_p;
          else if (strcmp (cmd_buf, "zero_p") == 0)
            fcn = mpfr_zero_p;
          else if (strcmp (cmd_buf, "regular_p") == 0)
            fcn = mpfr_regular_p;
          else if (strcmp (cmd_buf, "sgn") == 0)
            fcn = mpfr_sgn;
          else if (strcmp (cmd_buf, "integer_p") == 0)
            fcn = mpfr_integer_p;
          else if (strcmp (cmd_buf, "signbit") == 0)
            fcn = mpfr_signbit;
          else
            {
              MEX_FCN_ERR ("%s: Bad operator.\n", cmd_buf);
              break;
            }

          for (size_t i = 0; i < length (&idx); i++)
            plhs_0_pr[i] = (double) fcn (&data[(idx.start - 1) + i]);
        }

      /**
       * void mpfr_set_nan (mpfr_t x)
       * void mpfr_nextabove (mpfr_t x)
       * void mpfr_nextbelow (mpfr_t x)
       */
      else if ((strcmp (cmd_buf, "set_nan") == 0)
               || (strcmp (cmd_buf, "nextabove") == 0)
               || (strcmp (cmd_buf, "nextbelow") == 0))
        {
          if (nrhs != 2)
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

          DBG_PRINTF ("%s [%d:%d]\n", cmd_buf, idx.start, idx.end);

          void (*fcn)(mpfr_t);
          if (strcmp (cmd_buf, "set_nan") == 0)
            fcn = mpfr_set_nan;
          else if (strcmp (cmd_buf, "nextabove") == 0)
            fcn = mpfr_nextabove;
          else if (strcmp (cmd_buf, "nextbelow") == 0)
            fcn = mpfr_nextbelow;
          else
            {
              MEX_FCN_ERR ("%s: Bad operator.\n", cmd_buf);
              break;
            }

          for (size_t i = 0; i < length (&idx); i++)
            fcn (&data[(idx.start - 1) + i]);
        }

      /**
       * void mpfr_set_prec (mpfr_t x, mpfr_prec_t prec)
       * void mpfr_init2 (mpfr_t x, mpfr_prec_t prec)
       */
      else if ((strcmp (cmd_buf, "set_prec") == 0)
               || (strcmp (cmd_buf, "init2") == 0))
        {
          // Note combined, as due to this MEX interface, there are no
          // uninitialized MPFR variables.
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
          mpfr_prec_t prec = mpfr_get_default_prec ();
          if (! extract_prec (2, nrhs, prhs, &prec))
            {
              MEX_FCN_ERR ("%s: Precision must be a numeric scalar between "
                           "%ld and %ld.\n", cmd_buf, MPFR_PREC_MIN,
                          MPFR_PREC_MAX);
              break;
            }

          DBG_PRINTF ("%s: [%d:%d] (prec = %d)\n",
                      cmd_buf, idx.start, idx.end, (int) prec);
          for (size_t i = 0; i < length (&idx); i++)
            mpfr_set_prec (&data[(idx.start - 1) + i], prec);
        }

      /**
       * double mpfr_get_d (mpfr_t op, mpfr_rnd_t rnd)
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

          DBG_PRINTF ("get_d [%d:%d]\n", idx.start, idx.end);
          plhs[0] = mxCreateNumericMatrix (length (&idx), 1, mxDOUBLE_CLASS,
                                           mxREAL);
          double* plhs_0_pr = mxGetPr (plhs[0]);
          for (size_t i = 0; i < length (&idx); i++)
            plhs_0_pr[i] = mpfr_get_d (&data[(idx.start - 1) + i], rnd);
        }

      /**
       * int mpfr_set_d (mpfr_t rop, double op, mpfr_rnd_t rnd)
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
          double* op_pr = mxGetPr (prhs[2]);
          mpfr_rnd_t rnd = mpfr_get_default_rounding_mode ();
          if (! extract_rounding_mode (3, nrhs, prhs, &rnd))
            {
              MEX_FCN_ERR ("%s: Rounding must be a numeric scalar between "
                           "-1 and 3.\n", cmd_buf);
              break;
            }

          DBG_PRINTF ("set_d [%d:%d]\n", idx.start, idx.end);
          for (size_t i = 0; i < length (&idx); i++)
            mpfr_set_d (&data[(idx.start - 1) + i], op_pr[i], rnd);
        }

      /**
       * int mpfr_add (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_rnd_t rnd)
       * int mpfr_sub (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_rnd_t rnd)
       * int mpfr_mul (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_rnd_t rnd)
       * int mpfr_div (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_rnd_t rnd)
       * int mpfr_dim (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_rnd_t rnd)
       * int mpfr_pow (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_rnd_t rnd)
       * int mpfr_beta (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_rnd_t rnd)
       * int mpfr_agm (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_rnd_t rnd)
       * int mpfr_min (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_rnd_t rnd)
       * int mpfr_max (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_rnd_t rnd)
       * int mpfr_copysign (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_rnd_t rnd)
       */
      else if ((strcmp (cmd_buf, "add") == 0)
               || (strcmp (cmd_buf, "sub") == 0)
               || (strcmp (cmd_buf, "mul") == 0)
               || (strcmp (cmd_buf, "div") == 0)
               || (strcmp (cmd_buf, "dim") == 0)
               || (strcmp (cmd_buf, "pow") == 0)
               || (strcmp (cmd_buf, "beta") == 0)
               || (strcmp (cmd_buf, "agm") == 0)
               || (strcmp (cmd_buf, "min") == 0)
               || (strcmp (cmd_buf, "max") == 0)
               || (strcmp (cmd_buf, "copysign") == 0))
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
              || ((length (&rop) != length (&op1)) && (length (&op1) != 1)))
            {
              MEX_FCN_ERR ("%s:op1 Invalid MPFR variable indices.\n", cmd_buf);
              break;
            }
          idx_t op2;
          if (! extract_idx (3, nrhs, prhs, &op2)
              || ((length (&rop) != length (&op2)) && (length (&op2) != 1)))
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

          DBG_PRINTF ("%s [%d:%d] = [%d:%d] + [%d:%d] (rnd = %d)\n", cmd_buf,
                      rop.start, rop.end, op1.start, op1.end,
                      op2.start, op2.end, (int) rnd);

          int (*operator) (mpfr_t, const mpfr_t, const mpfr_t, mpfr_rnd_t);
          if (strcmp (cmd_buf, "add") == 0)
            operator = mpfr_add;
          else if (strcmp (cmd_buf, "sub") == 0)
            operator = mpfr_sub;
          else if (strcmp (cmd_buf, "mul") == 0)
            operator = mpfr_mul;
          else if (strcmp (cmd_buf, "div") == 0)
            operator = mpfr_div;
          else if (strcmp (cmd_buf, "dim") == 0)
            operator = mpfr_dim;
          else if (strcmp (cmd_buf, "pow") == 0)
            operator = mpfr_pow;
          else if (strcmp (cmd_buf, "beta") == 0)
            operator = mpfr_beta;
          else if (strcmp (cmd_buf, "agm") == 0)
            operator = mpfr_agm;
          else if (strcmp (cmd_buf, "min") == 0)
            operator = mpfr_min;
          else if (strcmp (cmd_buf, "max") == 0)
            operator = mpfr_max;
          else if (strcmp (cmd_buf, "copysign") == 0)
            operator = mpfr_copysign;
          else
            {
              MEX_FCN_ERR ("%s: Bad operator.\n", cmd_buf);
              break;
            }

          plhs[0] = mxCreateNumericMatrix (nlhs ? length (&rop): 1, 1,
                                           mxDOUBLE_CLASS, mxREAL);
          double*  ret_ptr = mxGetPr (plhs[0]);
          mpfr_ptr rop_ptr = &data[rop.start - 1];
          mpfr_ptr op1_ptr = &data[op1.start - 1];
          mpfr_ptr op2_ptr = &data[op2.start - 1];
          size_t ret_stride = (nlhs) ? 1 : 0;
          size_t rop_stride = 1;
          size_t op1_stride = (length (&op1) == 1) ? 0 : 1;
          size_t op2_stride = (length (&op2) == 1) ? 0 : 1;
          for (size_t i = 0; i < length (&rop); i++)
            {
              ret_ptr[i * ret_stride] =
                (double) operator (rop_ptr + (i * rop_stride),
                                   op1_ptr + (i * op1_stride),
                                   op2_ptr + (i * op2_stride),
                                   rnd);
            }
        }

      /**
       * int mpfr_add_d (mpfr_t rop, mpfr_t op1, double op2, mpfr_rnd_t rnd)
       * int mpfr_sub_d (mpfr_t rop, mpfr_t op1, double op2, mpfr_rnd_t rnd)
       * int mpfr_mul_d (mpfr_t rop, mpfr_t op1, double op2, mpfr_rnd_t rnd)
       * int mpfr_div_d (mpfr_t rop, mpfr_t op1, double op2, mpfr_rnd_t rnd)
       */
      else if ((strcmp (cmd_buf, "add_d") == 0)
               || (strcmp (cmd_buf, "sub_d") == 0)
               || (strcmp (cmd_buf, "mul_d") == 0)
               || (strcmp (cmd_buf, "div_d") == 0))
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
          size_t op2M = mxGetM (prhs[3]);
          size_t op2N = mxGetN (prhs[3]);
          if (! mxIsDouble (prhs[3])
              || (((op2M * op2N) != length (&rop)) && ((op2M * op2N) != 1)))
            {
              MEX_FCN_ERR ("%s:op2 Invalid.\n", cmd_buf);
              break;
            }
          mpfr_rnd_t rnd = mpfr_get_default_rounding_mode ();
          if (! extract_rounding_mode (4, nrhs, prhs, &rnd))
            {
              MEX_FCN_ERR ("%s: Rounding must be a numeric scalar between "
                           "-1 and 3.\n", cmd_buf);
              break;
            }

          DBG_PRINTF ("%s [%d:%d] = [%d:%d] + [%d:%d] (rnd = %d)\n", cmd_buf,
                      rop.start, rop.end, op1.start, op1.end,
                      op2M, op2N, (int) rnd);

          int (*operator) (mpfr_t, const mpfr_t, const double, mpfr_rnd_t);
          if (strcmp (cmd_buf, "add_d") == 0)
            operator = mpfr_add_d;
          else if (strcmp (cmd_buf, "sub_d") == 0)
            operator = mpfr_sub_d;
          else if (strcmp (cmd_buf, "mul_d") == 0)
            operator = mpfr_mul_d;
          else if (strcmp (cmd_buf, "div_d") == 0)
            operator = mpfr_div_d;
          else
            {
              MEX_FCN_ERR ("%s: Bad operator.\n", cmd_buf);
              break;
            }

          plhs[0] = mxCreateNumericMatrix (nlhs ? length (&rop): 1, 1,
                                           mxDOUBLE_CLASS, mxREAL);
          double*  ret_ptr = mxGetPr (plhs[0]);
          mpfr_ptr rop_ptr = &data[rop.start - 1];
          mpfr_ptr op1_ptr = &data[op1.start - 1];
          double*  op2_ptr = mxGetPr (prhs[3]);
          size_t ret_stride = (nlhs) ? 1 : 0;
          size_t op2_stride = ((op2M * op2N) == 1) ? 0 : 1;
          for (size_t i = 0; i < length (&rop); i++)
            {
              ret_ptr[i * ret_stride] =
                (double) operator (rop_ptr + i,
                                   op1_ptr + i,
                                   op2_ptr[i * op2_stride],
                                   rnd);
            }
        }

      /**
       * int mpfr_d_sub (mpfr_t rop, double op1, mpfr_t op2, mpfr_rnd_t rnd)
       * int mpfr_d_div (mpfr_t rop, double op1, mpfr_t op2, mpfr_rnd_t rnd)
       */
      else if ((strcmp (cmd_buf, "d_sub") == 0)
               || (strcmp (cmd_buf, "d_div") == 0))
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
          size_t op1M = mxGetM (prhs[2]);
          size_t op1N = mxGetN (prhs[2]);
          if (! mxIsDouble (prhs[2])
              || (((op1M * op1N) != length (&rop)) && ((op1M * op1N) != 1)))
            {
              MEX_FCN_ERR ("%s:op1 Invalid.\n", cmd_buf);
              break;
            }
          idx_t op2;
          if (! extract_idx (3, nrhs, prhs, &op2)
              || (length (&rop) != length (&op2)))
            {
              MEX_FCN_ERR ("%s:op1 Invalid MPFR variable indices.\n", cmd_buf);
              break;
            }
          mpfr_rnd_t rnd = mpfr_get_default_rounding_mode ();
          if (! extract_rounding_mode (4, nrhs, prhs, &rnd))
            {
              MEX_FCN_ERR ("%s: Rounding must be a numeric scalar between "
                           "-1 and 3.\n", cmd_buf);
              break;
            }

          DBG_PRINTF ("%s [%d:%d] = [%d:%d] + [%d:%d] (rnd = %d)\n", cmd_buf,
                      rop.start, rop.end, op1M, op1N,
                      op2.start, op2.end, (int) rnd);

          int (*operator) (mpfr_t, const double, const mpfr_t, mpfr_rnd_t);
          if (strcmp (cmd_buf, "d_sub") == 0)
            operator = mpfr_d_sub;
          else if (strcmp (cmd_buf, "d_div") == 0)
            operator = mpfr_d_div;
          else
            {
              MEX_FCN_ERR ("%s: Bad operator.\n", cmd_buf);
              break;
            }

          plhs[0] = mxCreateNumericMatrix (nlhs ? length (&rop): 1, 1,
                                           mxDOUBLE_CLASS, mxREAL);
          double*  ret_ptr = mxGetPr (plhs[0]);
          mpfr_ptr rop_ptr = &data[rop.start - 1];
          double*  op1_ptr = mxGetPr (prhs[2]);
          mpfr_ptr op2_ptr = &data[op2.start - 1];
          size_t ret_stride = (nlhs) ? 1 : 0;
          size_t op1_stride = ((op1M * op1N) == 1) ? 0 : 1;
          for (size_t i = 0; i < length (&rop); i++)
            {
              ret_ptr[i * ret_stride] =
                (double) operator (rop_ptr + i,
                                   op1_ptr[i * op1_stride],
                                   op2_ptr + i,
                                   rnd);
            }
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

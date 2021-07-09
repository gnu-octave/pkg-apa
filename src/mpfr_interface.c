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
  DBG_PRINTF ("Command: '%s'\n", cmd_buf);
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
       * idx_t mpfr_create (size_t count, mpfr_prec_t prec)
       *
       * Where `count` is the number of MPFR variables to create.
       *
       * For all created MPFR variables the precision is set to be exactly
       * prec bits and its values are set to NaN.
       *
       * Returned is the start and end index (1-based) of the internally created MPFR
       * variables.
       */
      else if (strcmp (cmd_buf, "create") == 0)
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


      /*
      ====================
      == MPFR functions ==
      ====================
      */


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

          DBG_PRINTF ("init2 '%d:%d' (prec = %d)\n", idx.start, idx.end,
                      (int) prec);
          for (size_t i = 0; i < length (&idx); i++)
            mpfr_init2 (&data[(idx.start - 1) + i], prec);
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
          double* op_pr = mxGetPr (prhs[2]);
          mpfr_rnd_t rnd = mpfr_get_default_rounding_mode ();
          if (! extract_rounding_mode (3, nrhs, prhs, &rnd))
            {
              MEX_FCN_ERR ("%s: Rounding must be a numeric scalar between "
                           "-1 and 3.\n", cmd_buf);
              break;
            }

          DBG_PRINTF ("set_d '%d:%d'\n", idx.start, idx.end);
          for (size_t i = 0; i < length (&idx); i++)
            mpfr_set_d (&data[(idx.start - 1) + i], op_pr[i], rnd);
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

          DBG_PRINTF ("get_d '%d:%d'\n", idx.start, idx.end);
          plhs[0] = mxCreateNumericMatrix (length (&idx), 1, mxDOUBLE_CLASS,
                                           mxREAL);
          double* plhs_0_pr = mxGetPr (plhs[0]);
          for (size_t i = 0; i < length (&idx); i++)
            plhs_0_pr[i] = mpfr_get_d (&data[(idx.start - 1) + i], rnd);
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

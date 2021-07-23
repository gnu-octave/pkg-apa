#include "mpfr_interface.h"

/**
 * Octave/Matlab MEX interface for MPFR (Version 4.1.0).
 *
 * https://www.mpfr.org/mpfr-current/mpfr.html
 *
 * See mpfr_interface.m for documentation.
 */

#define MAX(a,b) \
        ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); \
         _a > _b ? _a : _b; })

void
mexFunction (int nlhs, mxArray *plhs[],
             int nrhs, const mxArray *prhs[])
{
  #if (! defined(MPFR_VERSION) || (MPFR_VERSION < MPFR_VERSION_NUM(4,0,0)))
  # error "Oldest supported MPFR version is 4.0.0."
  #endif

  // Check code
  uint64_t cmd_code = 0;
  if (! extract_ui (0, nrhs, prhs, &cmd_code))
    mexErrMsgIdAndTxt ("mp:mexFunction",
                       "First input must be an MPFR command code.");

  int throw_error = 0;  // Marker if error should be thrown.

  DBG_PRINTF ("Command: [%d] = code[%d] (%d)\n", nlhs, cmd_code, nrhs);

  switch (cmd_code)
    {
      /*
      ========================
      == Non-MPFR functions ==
      ========================
      */


      case 9000:  // size_t get_data_capacity (void)
      {
        MEX_NARGINCHK(1);
        plhs[0] = mxCreateDoubleScalar ((double) data_capacity);
        break;
      }

      case 9001:  // size_t get_data_size (void)
      {
        MEX_NARGINCHK(1);
        plhs[0] = mxCreateDoubleScalar ((double) data_size);
        break;
      }

      case 9002:  // void set_verbose (int level)
      {
        MEX_NARGINCHK(2);
        int64_t level = 1;
        if (extract_si (1, nrhs, prhs, &level)
            && ((level == 0) || (level == 1)))
          VERBOSE = (int) level;
        else
          MEX_FCN_ERR ("cmd[%d]: VERBOSE must be 0 or 1.\n", cmd_code);
        break;
      }

      case 9003:  // idx_t mex_mpfr_allocate (size_t count)
      {
        MEX_NARGINCHK(2);
        size_t count = 0;
        if (! extract_ui (1, nrhs, prhs, &count))
          {
            MEX_FCN_ERR ("cmd[%d]: Count must be a positive numeric scalar.\n",
                         cmd_code);
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
        break;
      }


      /*
      ====================
      == MPFR functions ==
      ====================
      */

      case 1:  // void mpfr_set_default_prec (mpfr_prec_t prec)
      {
        MEX_NARGINCHK(2);
        MEX_MPFR_PREC_T(1, prec);
        mpfr_set_default_prec (prec);
        break;
      }

      case 2:  // mpfr_prec_t mpfr_get_default_prec (void)
      {
        MEX_NARGINCHK(1);
        plhs[0] = mxCreateDoubleScalar ((double) mpfr_get_default_prec ());
        break;
      }

      case 161:  // void mpfr_set_default_rounding_mode (mpfr_rnd_t rnd)
      {
        MEX_NARGINCHK(2);
        MEX_MPFR_RND_T(1, rnd);
        mpfr_set_default_rounding_mode (rnd);
        break;
      }

      case 162:  // mpfr_rnd_t mpfr_get_default_rounding_mode (void)
      {
        MEX_NARGINCHK(1);
        plhs[0] = mxCreateDoubleScalar (export_rounding_mode (
          mpfr_get_default_rounding_mode ()));
        break;
      }

      case 181:  // const char * mpfr_get_version (void)
      {
        MEX_NARGINCHK(1);
        char* output_buf = (char*) mxCalloc (strlen (mpfr_get_version ()),
                                             sizeof(char));
        strcpy (output_buf, mpfr_get_version ());
        plhs[0] = mxCreateString (output_buf);
        break;
      }

      case 182:  // int mpfr_buildopt_tls_p (void)
      {
        MEX_NARGINCHK(1);
        plhs[0] = mxCreateDoubleScalar ((double) mpfr_buildopt_tls_p ());
        break;
      }

      case 183:  // int mpfr_buildopt_float128_p (void)
      {
        MEX_NARGINCHK(1);
        plhs[0] = mxCreateDoubleScalar ((double) mpfr_buildopt_float128_p ());
        break;
      }

      case 184:  // int mpfr_buildopt_decimal_p (void)
      {
        MEX_NARGINCHK(1);
        plhs[0] = mxCreateDoubleScalar ((double) mpfr_buildopt_decimal_p ());
        break;
      }

      case 185:  // int mpfr_buildopt_gmpinternals_p (void)
      {
        MEX_NARGINCHK(1);
        plhs[0] = mxCreateDoubleScalar ((double)
                                        mpfr_buildopt_gmpinternals_p ());
        break;
      }

      case 186:  // int mpfr_buildopt_sharedcache_p (void)
      {
        MEX_NARGINCHK(1);
        plhs[0] = mxCreateDoubleScalar ((double)
                                        mpfr_buildopt_sharedcache_p ());
        break;
      }

      case 187:  // const char * mpfr_buildopt_tune_case (void)
      {
        MEX_NARGINCHK(1);
        char* output_buf = (char*) mxCalloc (
            strlen (mpfr_buildopt_tune_case ()), sizeof(char));
        strcpy (output_buf, mpfr_buildopt_tune_case ());
        plhs[0] = mxCreateString (output_buf);
        break;
      }

      case 188:  // mpfr_exp_t mpfr_get_emin (void)
      {
        MEX_NARGINCHK(1);
        plhs[0] = mxCreateDoubleScalar ((double) mpfr_get_emin ());
        break;
      }

      case 189:  // mpfr_exp_t mpfr_get_emax (void)
      {
        MEX_NARGINCHK(1);
        plhs[0] = mxCreateDoubleScalar ((double) mpfr_get_emax ());
        break;
      }

      case 192:  // mpfr_exp_t mpfr_get_emin_min
      {
        MEX_NARGINCHK(1);
        plhs[0] = mxCreateDoubleScalar ((double) mpfr_get_emin_min ());
        break;
      }

      case 193:  // mpfr_exp_t mpfr_get_emin_max
      {
        MEX_NARGINCHK(1);
        plhs[0] = mxCreateDoubleScalar ((double) mpfr_get_emin_max ());
        break;
      }

      case 194:  // mpfr_exp_t mpfr_get_emax_min
      {
        MEX_NARGINCHK(1);
        plhs[0] = mxCreateDoubleScalar ((double) mpfr_get_emax_min ());
        break;
      }

      case 195:  // mpfr_exp_t mpfr_get_emax_max
      {
        MEX_NARGINCHK(1);
        plhs[0] = mxCreateDoubleScalar ((double) mpfr_get_emax_max ());
        break;
      }

      case 198:  // void mpfr_clear_underflow (void)
      {
        MEX_NARGINCHK(1);
        mpfr_clear_underflow ();
        break;
      }

      case 199:  // void mpfr_clear_overflow (void)
      {
        MEX_NARGINCHK(1);
        mpfr_clear_overflow ();
        break;
      }

      case 200:  // void mpfr_clear_divby0 (void)
      {
        MEX_NARGINCHK(1);
        mpfr_clear_divby0 ();
        break;
      }

      case 201:  // void mpfr_clear_nanflag (void)
      {
        MEX_NARGINCHK(1);
        mpfr_clear_nanflag ();
        break;
      }

      case 202:  // void mpfr_clear_inexflag (void)
      {
        MEX_NARGINCHK(1);
        mpfr_clear_inexflag ();
        break;
      }

      case 203:  // void mpfr_clear_erangeflag (void)
      {
        MEX_NARGINCHK(1);
        mpfr_clear_erangeflag ();
        break;
      }

      case 204:  // void mpfr_clear_flags (void)
      {
        MEX_NARGINCHK(1);
        mpfr_clear_flags ();
        break;
      }

      case 205:  // void mpfr_set_underflow (void)
      {
        MEX_NARGINCHK(1);
        mpfr_set_underflow ();
        break;
      }

      case 206:  // void mpfr_set_overflow (void)
      {
        MEX_NARGINCHK(1);
        mpfr_set_overflow ();
        break;
      }

      case 207:  // void mpfr_set_divby0 (void)
      {
        MEX_NARGINCHK(1);
        mpfr_set_divby0 ();
        break;
      }

      case 208:  // void mpfr_set_nanflag (void)
      {
        MEX_NARGINCHK(1);
        mpfr_set_nanflag ();
        break;
      }

      case 209:  // void mpfr_set_inexflag (void)
      {
        MEX_NARGINCHK(1);
        mpfr_set_inexflag ();
        break;
      }

      case 210:  // void mpfr_set_erangeflag (void)
      {
        MEX_NARGINCHK(1);
        mpfr_set_erangeflag ();
        break;
      }

      case 211:  // int mpfr_underflow_p (void)
      {
        MEX_NARGINCHK(1);
        plhs[0] = mxCreateDoubleScalar ((double) mpfr_underflow_p ());
        break;
      }

      case 212:  // int mpfr_overflow_p (void)
      {
        MEX_NARGINCHK(1);
        plhs[0] = mxCreateDoubleScalar ((double) mpfr_overflow_p ());
        break;
      }

      case 213:  // int mpfr_divby0_p (void)
      {
        MEX_NARGINCHK(1);
        plhs[0] = mxCreateDoubleScalar ((double) mpfr_divby0_p ());
        break;
      }

      case 214:  // int mpfr_nanflag_p (void)
      {
        MEX_NARGINCHK(1);
        plhs[0] = mxCreateDoubleScalar ((double) mpfr_nanflag_p ());
        break;
      }

      case 215:  // int mpfr_inexflag_p (void)
      {
        MEX_NARGINCHK(1);
        plhs[0] = mxCreateDoubleScalar ((double) mpfr_inexflag_p ());
        break;
      }

      case 216:  // int mpfr_erangeflag_p (void)
      {
        MEX_NARGINCHK(1);
        plhs[0] = mxCreateDoubleScalar ((double) mpfr_erangeflag_p ());
        break;
      }

      case 4:    // mpfr_prec_t mpfr_get_prec (mpfr_t x)
      case 165:  // mpfr_prec_t mpfr_min_prec (mpfr_t x)
      {
        MEX_NARGINCHK(2);
        MEX_MPFR_T(1, idx);
        DBG_PRINTF ("cmd[%d]: [%d:%d]\n", cmd_code, idx.start, idx.end);
        plhs[0] = mxCreateNumericMatrix (length (&idx), 1, mxDOUBLE_CLASS,
                                         mxREAL);
        double* plhs_0_pr = mxGetPr (plhs[0]);

        mpfr_prec_t (*fcn)(const mpfr_t) = ((cmd_code == 4) ? mpfr_get_prec
                                                            : mpfr_min_prec);

        for (size_t i = 0; i < length (&idx); i++)
          plhs_0_pr[i] = (double) fcn (&data[(idx.start - 1) + i]);
        break;
      }

      case 176:  // mpfr_exp_t mpfr_get_exp (mpfr_t x)
      {
        MEX_NARGINCHK(2);
        MEX_MPFR_T(1, idx);
        DBG_PRINTF ("cmd[%d]: [%d:%d]\n", cmd_code, idx.start, idx.end);
        plhs[0] = mxCreateNumericMatrix (length (&idx), 1, mxDOUBLE_CLASS,
                                         mxREAL);
        double* plhs_0_pr = mxGetPr (plhs[0]);

        for (size_t i = 0; i < length (&idx); i++)
          plhs_0_pr[i] = (double) mpfr_get_exp (&data[(idx.start - 1) + i]);
        break;
      }

      case 69:   // int mpfr_nan_p (mpfr_t op)
      case 70:   // int mpfr_inf_p (mpfr_t op)
      case 71:   // int mpfr_number_p (mpfr_t op)
      case 72:   // int mpfr_zero_p (mpfr_t op)
      case 73:   // int mpfr_regular_p (mpfr_t op)
      case 74:   // int mpfr_sgn (mpfr_t op)
      case 160:  // int mpfr_integer_p (mpfr_t op)
      case 178:  // int mpfr_signbit (mpfr_t op)
      {
        MEX_NARGINCHK(2);
        MEX_MPFR_T(1, idx);
        DBG_PRINTF ("cmd[%d]: [%d:%d]\n", cmd_code, idx.start, idx.end);
        plhs[0] = mxCreateNumericMatrix (length (&idx), 1, mxDOUBLE_CLASS,
                                         mxREAL);
        double* plhs_0_pr = mxGetPr (plhs[0]);

        int (*fcn)(const mpfr_t);
        if (cmd_code == 69)
          fcn = mpfr_nan_p;
        else if (cmd_code == 70)
          fcn = mpfr_inf_p;
        else if (cmd_code == 71)
          fcn = mpfr_number_p;
        else if (cmd_code == 72)
          fcn = mpfr_zero_p;
        else if (cmd_code == 73)
          fcn = mpfr_regular_p;
        else if (cmd_code == 74)
          fcn = mpfr_sgn;
        else if (cmd_code == 160)
          fcn = mpfr_integer_p;
        else if (cmd_code == 178)
            fcn = mpfr_signbit;
        else
          {
            MEX_FCN_ERR ("cmd[%d]: Bad operator.\n", cmd_code);
            break;
          }

        for (size_t i = 0; i < length (&idx); i++)
          plhs_0_pr[i] = (double) fcn (&data[(idx.start - 1) + i]);
        break;
      }

      case 10:   // void mpfr_set_nan (mpfr_t x)
      case 167:  // void mpfr_nextabove (mpfr_t x)
      case 168:  // void mpfr_nextbelow (mpfr_t x)
      {
        MEX_NARGINCHK(2);
        MEX_MPFR_T(1, idx);
        DBG_PRINTF ("cmd[%d]: [%d:%d]\n", cmd_code, idx.start, idx.end);

        void (*fcn)(mpfr_t);
        if (cmd_code == 10)
          fcn = mpfr_set_nan;
        else if (cmd_code == 167)
          fcn = mpfr_nextabove;
        else if (cmd_code == 168)
          fcn = mpfr_nextbelow;
        else
          {
            MEX_FCN_ERR ("cmd[%d]: Bad operator.\n", cmd_code);
            break;
          }

        for (size_t i = 0; i < length (&idx); i++)
          fcn (&data[(idx.start - 1) + i]);
        break;
      }

      case 7:  // `int mpfr_set_ui_2exp (mpfr_t rop, unsigned long int op, mpfr_exp_t e, mpfr_rnd_t rnd)`
      case 8:  // `int mpfr_set_si_2exp (mpfr_t rop, long int op, mpfr_exp_t e, mpfr_rnd_t rnd)`
      {
        MEX_NARGINCHK(5);
        MEX_MPFR_T(1, rop);
        size_t opM = mxGetM (prhs[2]);
        size_t opN = mxGetN (prhs[2]);
        if (! mxIsDouble (prhs[2])
            || (((opM * opN) != length (&rop)) && ((opM * opN) != 1)))
          {
            MEX_FCN_ERR ("cmd[%d]:op must be a numerical vector of length 1"
                         "or %d.\n", cmd_code, length (&rop));
            break;
          }
        size_t expM = mxGetM (prhs[3]);
        size_t expN = mxGetN (prhs[3]);
        if (! mxIsDouble (prhs[3])
            || (((expM * expN) != length (&rop)) && ((expM * expN) != 1)))
          {
            MEX_FCN_ERR ("cmd[%d]:e must be a numerical vector of length 1"
                         "or %d.\n", cmd_code, length (&rop));
            break;
          }
        MEX_MPFR_RND_T(4, rnd);
        DBG_PRINTF ("cmd[%d]: [%d:%d] (rnd: %d)\n", cmd_code, rop.start,
                    rop.end, (int) rnd);

        plhs[0] = mxCreateNumericMatrix (nlhs ? length (&rop): 1, 1,
                                         mxDOUBLE_CLASS, mxREAL);
        double*  ret_ptr = mxGetPr (plhs[0]);
        mpfr_ptr rop_ptr = &data[rop.start - 1];
        double*   op_ptr = mxGetPr (prhs[2]);
        double*  exp_ptr = mxGetPr (prhs[3]);
        size_t ret_stride = (nlhs) ? 1 : 0;
        size_t  op_stride = (( opM *  opN) == 1) ? 0 : 1;
        size_t exp_stride = ((expM * expN) == 1) ? 0 : 1;
        if (cmd_code ==  7)
          {
            for (size_t i = 0; i < length (&rop); i++)
              {
                ret_ptr[i * ret_stride] =
                  (double) mpfr_set_ui_2exp (rop_ptr + i,
                    (unsigned long) op_ptr[i *  op_stride],
                    (mpfr_exp_t)   exp_ptr[i * exp_stride], rnd);
              }
          }
        else
          {
            for (size_t i = 0; i < length (&rop); i++)
              {
                ret_ptr[i * ret_stride] =
                  (double) mpfr_set_si_2exp (rop_ptr + i,
                    (unsigned long) op_ptr[i *  op_stride],
                    (mpfr_exp_t)   exp_ptr[i * exp_stride], rnd);
              }
          }
        break;
        break;
      }

      case 9:  // int mpfr_set_str (mpfr_t rop, const char *s, int base, mpfr_rnd_t rnd)
      case 16:  // int mpfr_init_set_str (mpfr_t x, const char *s, int base, mpfr_rnd_t rnd)
      case 217:  // int mpfr_strtofr (mpfr_t rop, const char *nptr, char **endptr, int base, mpfr_rnd_t rnd)
      {
        // Note: In `mpfr_strtofr`, the parameter `char **endptr` is treated
        // as output variable and activated, if two output variables are
        // requested.
        MEX_NARGINCHK(5);
        MEX_MPFR_T(1, idx);
        size_t strM = mxGetM (prhs[2]);
        size_t strN = mxGetN (prhs[2]);
        if (! mxIsCell (prhs[2])
            || (((strM * strN) != length (&idx)) && ((strM * strN) != 1)))
          {
            MEX_FCN_ERR ("cmd[%d]:s must be a cell vector of strings "
                         "of length 1 or %d\n.", cmd_code, length (&idx));
            break;
          }
        size_t baseM = mxGetM (prhs[3]);
        size_t baseN = mxGetN (prhs[3]);
        if (! mxIsDouble (prhs[3])
            || (((baseM * baseN) != length (&idx)) && ((baseM * baseN) != 1)))
          {
            MEX_FCN_ERR ("cmd[%d]:base must be a numeric vector "
                         "of length 1 or %d\n.", cmd_code, length (&idx));
            break;
          }
        MEX_MPFR_RND_T(4, rnd);
        DBG_PRINTF ("cmd[%d]: [%d:%d] (rnd: %d)\n", cmd_code, idx.start,
                    idx.end, (int) rnd);

        plhs[0] = mxCreateNumericMatrix (nlhs ? length (&idx): 1, 1,
                                         mxDOUBLE_CLASS, mxREAL);
        double*  ret_ptr = mxGetPr (plhs[0]);
        double* base_ptr = mxGetPr (prhs[3]);
        size_t  ret_stride = (nlhs) ? 1 : 0;
        size_t  str_stride = (( strM *  strN) == 1) ? 0 : 1;
        size_t base_stride = ((baseM * baseN) == 1) ? 0 : 1;

        char* str = mxArrayToString(mxGetCell(prhs[2], 0));
        if (cmd_code == 217)  // mpfr_strtofr
          {
            plhs[1] = mxCreateNumericMatrix ((nlhs == 2) ? length (&idx): 1, 1,
                                             mxDOUBLE_CLASS, mxREAL);
            double* end_ptr = mxGetPr (plhs[1]);
            size_t end_ptr_stride = (nlhs == 2) ? 1 : 0;
            for (size_t i = 0; i < length (&idx); i++)
              {
                if (str_stride && (i > 0))
                  {
                    mxFree(str);
                    str = mxArrayToString(mxGetCell(prhs[2], i * str_stride));
                  }
                char *endptr = NULL;
                ret_ptr[i * ret_stride] =
                  mpfr_strtofr (&data[(idx.start - 1) + i], str, &endptr,
                                (int) base_ptr[i * base_stride], rnd);
                end_ptr[i * end_ptr_stride] = (endptr == NULL)
                                            ? -1.0
                                            : (double) (endptr - str + 1);
              }
          }
        else  // mpfr_set_str OR mpfr_init_set_str
          {
            int (*fcn)(mpfr_t, const char *, int, mpfr_rnd_t) =
              (cmd_code == 9) ? mpfr_set_str : mpfr_init_set_str;

            for (size_t i = 0; i < length (&idx); i++)
              {
                if (str_stride && (i > 0))
                  {
                    mxFree(str);
                    str = mxArrayToString(mxGetCell(prhs[2], i * str_stride));
                  }
                if (cmd_code == 9)
                  mpfr_clear (&data[(idx.start - 1) + i]);
                ret_ptr[i * ret_stride] = fcn (&data[(idx.start - 1) + i],
                                               str,
                                               (int) base_ptr[i * base_stride],
                                               rnd);
              }
          }
        mxFree(str);
        break;
      }

      case 11:  // void mpfr_set_inf (mpfr_t x, int sign)
      case 12:  // void mpfr_set_zero (mpfr_t x, int sign)
      {
        MEX_NARGINCHK(3);
        MEX_MPFR_T(1, idx);
        size_t signM = mxGetM (prhs[2]);
        size_t signN = mxGetN (prhs[2]);
        if (! mxIsDouble (prhs[2])
            || (((signM * signN) != length (&idx)) && ((signM * signN) != 1)))
          {
            MEX_FCN_ERR ("cmd[%d]:sign must be a numeric vector "
                         "of length 1 or %d\n.", cmd_code, length (&idx));
            break;
          }
        DBG_PRINTF ("cmd[%d]: [%d:%d]\n", cmd_code, idx.start, idx.end);

        void (*fcn)(mpfr_t, int) = ((cmd_code == 11) ? mpfr_set_inf
                                                     : mpfr_set_zero);

        double* sign_ptr = mxGetPr (prhs[2]);
        size_t sign_stride = ((signM * signN) == 1) ? 0 : 1;

        for (size_t i = 0; i < length (&idx); i++)
          fcn (&data[(idx.start - 1) + i], (int) sign_ptr[i * sign_stride]);
        break;
      }

      case 0:  // void mpfr_init2 (mpfr_t x, mpfr_prec_t prec)
      case 3:  // void mpfr_set_prec (mpfr_t x, mpfr_prec_t prec)
      {
        // Note combined, as due to this MEX interface, there are no
        // uninitialized MPFR variables.
        MEX_NARGINCHK(3);
        MEX_MPFR_T(1, idx);
        MEX_MPFR_PREC_T(2, prec);
        DBG_PRINTF ("cmd[%d]: [%d:%d] (prec = %d)\n",
                    cmd_code, idx.start, idx.end, (int) prec);
        for (size_t i = 0; i < length (&idx); i++)
          mpfr_set_prec (&data[(idx.start - 1) + i], prec);
        break;
      }

      case 13:  // void mpfr_swap (mpfr_t x, mpfr_t y)
      {
        MEX_NARGINCHK(3);
        MEX_MPFR_T(1, op1);
        MEX_MPFR_T(2, op2);
        if (length (&op1) != length (&op2))
          {
            MEX_FCN_ERR ("cmd[%d]:op2 Invalid size.\n", cmd_code);
            break;
          }
        DBG_PRINTF ("cmd[%d]: op1 = [%d:%d], op2 = [%d:%d]\n",
                    cmd_code, op1.start, op1.end, op2.start, op2.end);

        for (size_t i = 0; i < length (&op1); i++)
          mpfr_swap (&data[op1.start - 1 + i], &data[op2.start - 1 + i]);
        break;
      }

      case 14:  // int mpfr_init_set (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      {
        MEX_NARGINCHK(4);
        MEX_MPFR_T(1, rop);
        MEX_MPFR_T(2, op);
        if (length (&rop) != length (&op))
          {
            MEX_FCN_ERR ("cmd[%d]:op Invalid size.\n", cmd_code);
            break;
          }
        MEX_MPFR_RND_T(3, rnd);
        DBG_PRINTF ("cmd[%d]: rop = [%d:%d], op = [%d:%d] (rnd = %d)\n",
                    cmd_code, rop.start, rop.end, op.start, op.end, (int) rnd);

        plhs[0] = mxCreateNumericMatrix (nlhs ? length (&rop): 1, 1,
                                         mxDOUBLE_CLASS, mxREAL);
        double*  ret_ptr = mxGetPr (plhs[0]);
        size_t ret_stride = (nlhs) ? 1 : 0;

        for (size_t i = 0; i < length (&rop); i++)
          {
            mpfr_clear (&data[(rop.start - 1) + i]);
            ret_ptr[i * ret_stride] = mpfr_init_set (
              &data[(rop.start - 1) + i], &data[(op.start - 1) + i], rnd);
          }
        break;
      }

      case 17:  // double mpfr_get_d (mpfr_t op, mpfr_rnd_t rnd)
      {
        MEX_NARGINCHK(3);
        MEX_MPFR_T(1, op);
        MEX_MPFR_RND_T(2, rnd);
        DBG_PRINTF ("cmd[%d]: [%d:%d] (rnd = %d)\n", cmd_code, op.start,
                    op.end, (int) rnd);

        plhs[0] = mxCreateNumericMatrix (length (&op), 1, mxDOUBLE_CLASS,
                                         mxREAL);
        double* ret_ptr = mxGetPr (plhs[0]);
        for (size_t i = 0; i < length (&op); i++)
          ret_ptr[i] = mpfr_get_d (&data[(op.start - 1) + i], rnd);
        break;
      }

      case 18:  // double mpfr_get_d_2exp (long *exp, mpfr_t op, mpfr_rnd_t rnd)
      {
        // Note `long *exp` is return parameter.
        MEX_NARGINCHK(3);
        MEX_MPFR_T(1, op);
        MEX_MPFR_RND_T(2, rnd);
        DBG_PRINTF ("cmd[%d]: [%d:%d] (rnd = %d)\n", cmd_code, op.start,
                    op.end, (int) rnd);

        plhs[0] = mxCreateNumericMatrix (length (&op), 1, mxDOUBLE_CLASS,
                                         mxREAL);
        plhs[1] = mxCreateNumericMatrix (length (&op), 1, mxDOUBLE_CLASS,
                                         mxREAL);
        double* ret_ptr = mxGetPr (plhs[0]);
        double* exp_ptr = mxGetPr (plhs[1]);
        for (size_t i = 0; i < length (&op); i++)
          {
            long exp = 0;
            ret_ptr[i] = mpfr_get_d_2exp (&exp, &data[(op.start - 1) + i], rnd);
            exp_ptr[i] = (double) exp;
          }
        break;
      }

      case 19:  // int mpfr_frexp (mpfr_exp_t *exp, mpfr_t y, mpfr_t x, mpfr_rnd_t rnd)
      {
        // Note `mpfr_exp_t *exp` is return parameter.
        MEX_NARGINCHK(4);
        MEX_MPFR_T(1, y);
        MEX_MPFR_T(2, x);
        if (length (&y) != length (&x))
          {
            MEX_FCN_ERR ("cmd[%d]:x and y must have the same size.\n",
                         cmd_code);
            break;
          }
        MEX_MPFR_RND_T(3, rnd);
        DBG_PRINTF ("cmd[%d]: [%d:%d] [%d:%d] (rnd = %d)\n", cmd_code,
                    y.start, y.end, x.start, x.end, (int) rnd);

        plhs[0] = mxCreateNumericMatrix (length (&y), 1, mxDOUBLE_CLASS,
                                         mxREAL);
        plhs[1] = mxCreateNumericMatrix (length (&y), 1, mxDOUBLE_CLASS,
                                         mxREAL);
        double* ret_ptr = mxGetPr (plhs[0]);
        double* exp_ptr = mxGetPr (plhs[1]);
        for (size_t i = 0; i < length (&y); i++)
          {
            mpfr_exp_t exp = 0;
            ret_ptr[i] = mpfr_frexp (&exp, &data[(y.start - 1) + i],
                                     &data[(x.start - 1) + i], rnd);
            exp_ptr[i] = (double) exp;
          }
        break;
      }

      case 20:  // size_t mpfr_get_str_ndigits (int b, mpfr_prec_t p)
      {
      #if (MPFR_VERSION < MPFR_VERSION_NUM(4,1,0))
        MEX_FCN_ERR ("cmd[%d]: Not supported in MPFR %s.\n",
                      cmd_code, MPFR_VERSION_STRING);
      #else
        MEX_NARGINCHK(3);
        if (! mxIsDouble (prhs[1]) || ! mxIsDouble (prhs[2]))
          {
            MEX_FCN_ERR ("cmd[%d]:b and p must be a numeric vectors\n.",
                         cmd_code);
            break;
          }
        size_t op1Dim = mxGetM (prhs[1]) * mxGetN (prhs[1]);
        size_t op2Dim = mxGetM (prhs[2]) * mxGetN (prhs[2]);
        if ((op1Dim != 1) && (op2Dim != 1) && (op1Dim != op2Dim))
          {
            MEX_FCN_ERR ("cmd[%d]:b and p must be scalar or their dimensions "
                         "must agree\n.",cmd_code);
            break;
          }
        DBG_PRINTF ("cmd[%d]: dim(b) = %d, dim(p) = %d\n", op1Dim, op2Dim);

        plhs[0] = mxCreateNumericMatrix (MAX(op1Dim, op2Dim), 1,
                                         mxDOUBLE_CLASS, mxREAL);
        double* ret_ptr = mxGetPr (plhs[0]);
        double* op1_ptr = mxGetPr (prhs[1]);
        double* op2_ptr = mxGetPr (prhs[2]);
        size_t op1_stride = (op1Dim == 1) ? 0 : 1;
        size_t op2_stride = (op2Dim == 1) ? 0 : 1;

        for (size_t i = 0; i < MAX(op1Dim, op2Dim); i++)
          ret_ptr[i] = mpfr_get_str_ndigits ((int) op1_ptr[i * op1_stride],
                                     (mpfr_prec_t) op2_ptr[i * op2_stride]);
      #endif
        break;
      }

      case 6:  // int mpfr_set_d (mpfr_t rop, double op, mpfr_rnd_t rnd)
      case 15:  // int mpfr_init_set_d (mpfr_t rop, double op, mpfr_rnd_t rnd)
      {
        MEX_NARGINCHK(4);
        MEX_MPFR_T(1, rop);
        size_t opM = mxGetM (prhs[2]);
        size_t opN = mxGetN (prhs[2]);
        if (! mxIsDouble (prhs[2])
            || (((opM * opN) != length (&rop)) && ((opM * opN) != 1)))
          {
            MEX_FCN_ERR ("cmd[%d]:op must be a numeric vector "
                         "of length 1 or %d\n.", cmd_code, length (&rop));
            break;
          }
        MEX_MPFR_RND_T(3, rnd);
        DBG_PRINTF ("cmd[%d]: [%d:%d]\n", cmd_code, rop.start, rop.end);

        double* op_pr = mxGetPr (prhs[2]);
        size_t op_stride = ((opM * opN) == 1) ? 0 : 1;

        if (cmd_code == 6)
          for (size_t i = 0; i < length (&rop); i++)
              mpfr_set_d(&data[(rop.start - 1) + i], op_pr[i * op_stride], rnd);
        else
          for (size_t i = 0; i < length (&rop); i++)
            {
              mpfr_clear(&data[(rop.start - 1) + i]);
              mpfr_init_set_d(&data[(rop.start - 1) + i], op_pr[i * op_stride],
                              rnd);
            }
        break;
      }

      case 21: // char * mpfr_get_str (char *str, mpfr_exp_t *expptr, int base, size_t n, mpfr_t op, mpfr_rnd_t rnd)
      {
        // Note: char *str, mpfr_exp_t *expptr are return parameters.
        MEX_NARGINCHK(5);
        MEX_MPFR_T(3, op);
        size_t baseM = mxGetM (prhs[1]);
        size_t baseN = mxGetN (prhs[1]);
        if (! mxIsDouble (prhs[1])
            || (((baseM * baseN) != length (&op)) && ((baseM * baseN) != 1)))
          {
            MEX_FCN_ERR ("cmd[%d]:base must be a numeric vector "
                         "of length 1 or %d\n.", cmd_code, length (&op));
            break;
          }
        size_t nSigM = mxGetM (prhs[2]);
        size_t nSigN = mxGetN (prhs[2]);
        if (! mxIsDouble (prhs[2])
            || (((nSigM * nSigN) != length (&op)) && ((nSigM * nSigN) != 1)))
          {
            MEX_FCN_ERR ("cmd[%d]:n must be a numeric vector "
                         "of length 1 or %d\n.", cmd_code, length (&op));
            break;
          }
        MEX_MPFR_RND_T(4, rnd);
        DBG_PRINTF ("cmd[%d]: [%d:%d]\n", cmd_code, op.start, op.end);

        plhs[0] = mxCreateCellMatrix (length (&op), 1);
        plhs[1] = mxCreateNumericMatrix (length (&op), 1, mxDOUBLE_CLASS,
                                         mxREAL);
        double* exp_ptr = mxGetPr (plhs[1]);

        double* base_ptr = mxGetPr (prhs[1]);
        double* nSig_ptr = mxGetPr (prhs[2]);
        size_t base_stride = ((baseM * baseN) == 1) ? 0 : 1;
        size_t nSig_stride = ((nSigM * nSigN) == 1) ? 0 : 1;

        for (size_t i = 0; i < length (&op); i++)
          {
            mpfr_exp_t expptr = 0;
            char* str = mpfr_get_str (NULL, &expptr,
                                      (int)    base_ptr[i * base_stride],
                                      (size_t) nSig_ptr[i * nSig_stride],
                                      &data[(op.start - 1) + i], rnd);
            if (str != NULL)
              {
                char* significant = (char*) mxCalloc (strlen (str),
                                                      sizeof(char));
                strcpy (significant, str);
                mpfr_free_str(str);
                mxSetCell(plhs[0], i, mxCreateString (significant));
              }
            exp_ptr[i] = (double) expptr;
          }
        break;
      }

      case 23:  // int mpfr_fits_ulong_p (mpfr_t op, mpfr_rnd_t rnd)
      case 24:  // int mpfr_fits_slong_p (mpfr_t op, mpfr_rnd_t rnd)
      case 25:  // int mpfr_fits_uint_p (mpfr_t op, mpfr_rnd_t rnd)
      case 26:  // int mpfr_fits_sint_p (mpfr_t op, mpfr_rnd_t rnd)
      case 27:  // int mpfr_fits_ushort_p (mpfr_t op, mpfr_rnd_t rnd)
      case 28:  // int mpfr_fits_sshort_p (mpfr_t op, mpfr_rnd_t rnd)
      case 29:  // int mpfr_fits_uintmax_p (mpfr_t op, mpfr_rnd_t rnd)
      case 30:  // int mpfr_fits_intmax_p (mpfr_t op, mpfr_rnd_t rnd)
      {
        MEX_NARGINCHK(3);
        MEX_MPFR_T(1, op);
        MEX_MPFR_RND_T(2, rnd);
        DBG_PRINTF ("cmd[%d]: [%d:%d] (rnd = %d)\n", cmd_code, op.start,
                    op.end, (int) rnd);

        int (*fcn)(const mpfr_t, mpfr_rnd_t);
        if (cmd_code == 23)
          fcn = mpfr_fits_ulong_p;
        else if (cmd_code == 24)
          fcn = mpfr_fits_slong_p;
        else if (cmd_code == 25)
          fcn = mpfr_fits_uint_p;
        else if (cmd_code == 26)
          fcn = mpfr_fits_sint_p;
        else if (cmd_code == 27)
          fcn = mpfr_fits_ushort_p;
        else if (cmd_code == 28)
          fcn = mpfr_fits_sshort_p;
        else if (cmd_code == 29)
          fcn = mpfr_fits_uintmax_p;
        else if (cmd_code == 20)
          fcn = mpfr_fits_intmax_p;
        else
          {
            MEX_FCN_ERR ("cmd[%d]: Bad operator.\n", cmd_code);
            break;
          }

        plhs[0] = mxCreateNumericMatrix (length (&op), 1, mxDOUBLE_CLASS,
                                         mxREAL);
        double* ret_ptr = mxGetPr (plhs[0]);
        for (size_t i = 0; i < length (&op); i++)
          ret_ptr[i] = (double) fcn(&data[(op.start - 1) + i], rnd);
        break;
      }

      case 31:  // int mpfr_add (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_rnd_t rnd)
      case 33:  // int mpfr_sub (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_rnd_t rnd)
      case 36:  // int mpfr_mul (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_rnd_t rnd)
      case 39:  // int mpfr_div (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_rnd_t rnd)
      case 50:  // int mpfr_dim (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_rnd_t rnd)
      case 92:  // int mpfr_pow (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_rnd_t rnd)
      case 126:  // int mpfr_beta (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_rnd_t rnd)
      case 137:  // int mpfr_agm (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_rnd_t rnd)
      case 169:  // int mpfr_min (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_rnd_t rnd)
      case 170:  // int mpfr_max (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_rnd_t rnd)
      case 180:  // int mpfr_copysign (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_rnd_t rnd)
      {
        MEX_NARGINCHK(5);
        MEX_MPFR_T(1, rop);
        MEX_MPFR_T(2, op1);
        MEX_MPFR_T(3, op2);
        if ((length (&rop) != length (&op1)) && (length (&op1) != 1))
          {
            MEX_FCN_ERR ("cmd[%d]:op1 Invalid size.\n", cmd_code);
            break;
          }
        if ((length (&rop) != length (&op2)) && (length (&op2) != 1))
          {
            MEX_FCN_ERR ("cmd[%d]:op2 Invalid size.\n", cmd_code);
            break;
          }
        MEX_MPFR_RND_T(4, rnd);
        DBG_PRINTF ("cmd[%d]: [%d:%d] = [%d:%d] + [%d:%d] (rnd = %d)\n",
                    cmd_code,
                    rop.start, rop.end,
                    op1.start, op1.end,
                    op2.start, op2.end, (int) rnd);

        int (*operator) (mpfr_t, const mpfr_t, const mpfr_t, mpfr_rnd_t);
        if (cmd_code == 31)
          operator = mpfr_add;
        else if (cmd_code == 33)
          operator = mpfr_sub;
        else if (cmd_code == 36)
          operator = mpfr_mul;
        else if (cmd_code == 39)
          operator = mpfr_div;
        else if (cmd_code == 50)
          operator = mpfr_dim;
        else if (cmd_code == 92)
          operator = mpfr_pow;
        else if (cmd_code == 126)
          operator = mpfr_beta;
        else if (cmd_code == 137)
          operator = mpfr_agm;
        else if (cmd_code == 169)
          operator = mpfr_min;
        else if (cmd_code == 170)
          operator = mpfr_max;
        else if (cmd_code == 180)
          operator = mpfr_copysign;
        else
          {
            MEX_FCN_ERR ("cmd[%d]: Bad operator.\n", cmd_code);
            break;
          }

        plhs[0] = mxCreateNumericMatrix (nlhs ? length (&rop): 1, 1,
                                         mxDOUBLE_CLASS, mxREAL);
        double*  ret_ptr = mxGetPr (plhs[0]);
        mpfr_ptr rop_ptr = &data[rop.start - 1];
        mpfr_ptr op1_ptr = &data[op1.start - 1];
        mpfr_ptr op2_ptr = &data[op2.start - 1];
        size_t ret_stride = (nlhs) ? 1 : 0;
        size_t op1_stride = (length (&op1) == 1) ? 0 : 1;
        size_t op2_stride = (length (&op2) == 1) ? 0 : 1;
        for (size_t i = 0; i < length (&rop); i++)
          {
            ret_ptr[i * ret_stride] =
              (double) operator (rop_ptr + i,
                                 op1_ptr + (i * op1_stride),
                                 op2_ptr + (i * op2_stride), rnd);
          }
        break;
      }

      case 32:  // int mpfr_add_d (mpfr_t rop, mpfr_t op1, double op2, mpfr_rnd_t rnd)
      case 35:  // int mpfr_sub_d (mpfr_t rop, mpfr_t op1, double op2, mpfr_rnd_t rnd)
      case 37:  // int mpfr_mul_d (mpfr_t rop, mpfr_t op1, double op2, mpfr_rnd_t rnd)
      case 41:  // int mpfr_div_d (mpfr_t rop, mpfr_t op1, double op2, mpfr_rnd_t rnd)
      {
        MEX_NARGINCHK(5);
        MEX_MPFR_T(1, rop);
        MEX_MPFR_T(2, op1);
        if (length (&rop) != length (&op1))
          {
            MEX_FCN_ERR ("cmd[%d]:op1 Invalid size.\n", cmd_code);
            break;
          }
        size_t op2M = mxGetM (prhs[3]);
        size_t op2N = mxGetN (prhs[3]);
        if (! mxIsDouble (prhs[3])
            || (((op2M * op2N) != length (&rop)) && ((op2M * op2N) != 1)))
          {
            MEX_FCN_ERR ("cmd[%d]:op2 Invalid.\n", cmd_code);
            break;
          }
        MEX_MPFR_RND_T(4, rnd);
        DBG_PRINTF ("cmd[%d]: [%d:%d] = [%d:%d] + [%d:%d] (rnd = %d)\n",
                    cmd_code,
                    rop.start, rop.end,
                    op1.start, op1.end,
                    op2M, op2N, (int) rnd);

        int (*operator) (mpfr_t, const mpfr_t, const double, mpfr_rnd_t);
        if (cmd_code == 32)
          operator = mpfr_add_d;
        else if (cmd_code == 35)
          operator = mpfr_sub_d;
        else if (cmd_code == 37)
          operator = mpfr_mul_d;
        else if (cmd_code == 41)
          operator = mpfr_div_d;
        else
          {
            MEX_FCN_ERR ("cmd[%d]: Bad operator.\n", cmd_code);
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
              (double) operator (rop_ptr + i, op1_ptr + i,
                                 op2_ptr[i * op2_stride], rnd);
          }
        break;
      }

      case 34:  // int mpfr_d_sub (mpfr_t rop, double op1, mpfr_t op2, mpfr_rnd_t rnd)
      case 40:  // int mpfr_d_div (mpfr_t rop, double op1, mpfr_t op2, mpfr_rnd_t rnd)
      {
        MEX_NARGINCHK(5);
        MEX_MPFR_T(1, rop);
        size_t op1M = mxGetM (prhs[2]);
        size_t op1N = mxGetN (prhs[2]);
        if (! mxIsDouble (prhs[2])
            || (((op1M * op1N) != length (&rop)) && ((op1M * op1N) != 1)))
          {
            MEX_FCN_ERR ("cmd[%d]:op1 Invalid.\n", cmd_code);
            break;
          }
        MEX_MPFR_T(3, op2);
        if (length (&rop) != length (&op2))
          {
            MEX_FCN_ERR ("cmd[%d]:op2 Invalid size.\n", cmd_code);
            break;
          }
        MEX_MPFR_RND_T(4, rnd);
        DBG_PRINTF ("cmd[%d]: [%d:%d] = [%d:%d] + [%d:%d] (rnd = %d)\n",
                    cmd_code, rop.start, rop.end, op1M, op1N,
                    op2.start, op2.end, (int) rnd);

        int (*operator) (mpfr_t, const double, const mpfr_t, mpfr_rnd_t) =
          ((cmd_code == 34) ? mpfr_d_sub : mpfr_d_div);

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
              (double) operator (rop_ptr + i, op1_ptr[i * op1_stride],
                                 op2_ptr + i, rnd);
          }
        break;
      }

      case 5:    // int mpfr_set (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 38:   // int mpfr_sqr (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 42:   // int mpfr_sqrt (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 44:   // int mpfr_rec_sqrt (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 48:   // int mpfr_neg (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 49:   // int mpfr_abs (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 83:   // int mpfr_log (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 85:   // int mpfr_log2 (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 86:   // int mpfr_log10 (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 87:   // int mpfr_log1p (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 88:   // int mpfr_exp (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 89:   // int mpfr_exp2 (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 90:   // int mpfr_exp10 (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 91:   // int mpfr_expm1 (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 98:   // int mpfr_cos (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 99:   // int mpfr_sin (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 100:  // int mpfr_tan (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 102:  // int mpfr_sec (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 103:  // int mpfr_csc (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 104:  // int mpfr_cot (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 105:  // int mpfr_acos (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 106:  // int mpfr_asin (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 107:  // int mpfr_atan (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 109:  // int mpfr_cosh (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 110:  // int mpfr_sinh (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 111:  // int mpfr_tanh (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 113:  // int mpfr_sech (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 114:  // int mpfr_csch (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 115:  // int mpfr_coth (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 116:  // int mpfr_acosh (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 117:  // int mpfr_asinh (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 118:  // int mpfr_atanh (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 119:  // int mpfr_eint (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 120:  // int mpfr_li2 (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 121:  // int mpfr_gamma (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 123:  // int mpfr_lngamma (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 125:  // int mpfr_digamma (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 127:  // int mpfr_zeta (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 129:  // int mpfr_erf (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 130:  // int mpfr_erfc (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 131:  // int mpfr_j0 (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 132:  // int mpfr_j1 (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 134:  // int mpfr_y0 (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 135:  // int mpfr_y1 (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 143:  // int mpfr_rint (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 149:  // int mpfr_rint_ceil (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 150:  // int mpfr_rint_floor (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 151:  // int mpfr_rint_round (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 152:  // int mpfr_rint_roundeven (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 153:  // int mpfr_rint_trunc (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 154:  // int mpfr_frac (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      {
        MEX_NARGINCHK(4);
        MEX_MPFR_T(1, rop);
        MEX_MPFR_T(2, op);
        if (length (&rop) != length (&op))
          {
            MEX_FCN_ERR ("cmd[%d]:op Invalid size.\n", cmd_code);
            break;
          }
        MEX_MPFR_RND_T(3, rnd);
        DBG_PRINTF ("cmd[%d]: rop = [%d:%d], op = [%d:%d] (rnd = %d)\n",
                    cmd_code, rop.start, rop.end, op.start, op.end, (int) rnd);

        int (*operator) (mpfr_t, const mpfr_t, mpfr_rnd_t);
        if (cmd_code == 5)
          operator = mpfr_set;
        else if (cmd_code == 38)
          operator = mpfr_sqr;
        else if (cmd_code == 42)
          operator = mpfr_sqrt;
        else if (cmd_code == 44)
          operator = mpfr_rec_sqrt;
        else if (cmd_code == 48)
          operator = mpfr_neg;
        else if (cmd_code == 49)
          operator = mpfr_abs;
        else if (cmd_code == 83)
          operator = mpfr_log;
        else if (cmd_code == 85)
          operator = mpfr_log2;
        else if (cmd_code == 86)
          operator = mpfr_log10;
        else if (cmd_code == 87)
          operator = mpfr_log1p;
        else if (cmd_code == 88)
          operator = mpfr_exp;
        else if (cmd_code == 89)
          operator = mpfr_exp2;
        else if (cmd_code == 90)
          operator = mpfr_exp10;
        else if (cmd_code == 91)
          operator = mpfr_expm1;
        else if (cmd_code == 98)
          operator = mpfr_cos;
        else if (cmd_code == 99)
          operator = mpfr_sin;
        else if (cmd_code == 100)
          operator = mpfr_tan;
        else if (cmd_code == 102)
          operator = mpfr_sec;
        else if (cmd_code == 103)
          operator = mpfr_csc;
        else if (cmd_code == 104)
          operator = mpfr_cot;
        else if (cmd_code == 105)
          operator = mpfr_acos;
        else if (cmd_code == 106)
          operator = mpfr_asin;
        else if (cmd_code == 107)
          operator = mpfr_atan;
        else if (cmd_code == 109)
          operator = mpfr_cosh;
        else if (cmd_code == 110)
          operator = mpfr_sinh;
        else if (cmd_code == 111)
          operator = mpfr_tanh;
        else if (cmd_code == 113)
          operator = mpfr_sech;
        else if (cmd_code == 114)
          operator = mpfr_csch;
        else if (cmd_code == 115)
          operator = mpfr_coth;
        else if (cmd_code == 116)
          operator = mpfr_acosh;
        else if (cmd_code == 117)
          operator = mpfr_asinh;
        else if (cmd_code == 118)
          operator = mpfr_atanh;
        else if (cmd_code == 119)
          operator = mpfr_eint;
        else if (cmd_code == 120)
          operator = mpfr_li2;
        else if (cmd_code == 121)
          operator = mpfr_gamma;
        else if (cmd_code == 123)
          operator = mpfr_lngamma;
        else if (cmd_code == 125)
          operator = mpfr_digamma;
        else if (cmd_code == 127)
          operator = mpfr_zeta;
        else if (cmd_code == 129)
          operator = mpfr_erf;
        else if (cmd_code == 130)
          operator = mpfr_erfc;
        else if (cmd_code == 131)
          operator = mpfr_j0;
        else if (cmd_code == 132)
          operator = mpfr_j1;
        else if (cmd_code == 134)
          operator = mpfr_y0;
        else if (cmd_code == 135)
          operator = mpfr_y1;
        else if (cmd_code == 143)
          operator = mpfr_rint;
        else if (cmd_code == 149)
          operator = mpfr_rint_ceil;
        else if (cmd_code == 150)
          operator = mpfr_rint_floor;
        else if (cmd_code == 151)
          operator = mpfr_rint_round;
        else if (cmd_code == 152)
          operator = mpfr_rint_roundeven;
        else if (cmd_code == 153)
          operator = mpfr_rint_trunc;
        else if (cmd_code == 154)
          operator = mpfr_frac;
        else
          {
            MEX_FCN_ERR ("cmd[%d]: Bad operator.\n", cmd_code);
            break;
          }

        plhs[0] = mxCreateNumericMatrix (nlhs ? length (&rop): 1, 1,
                                         mxDOUBLE_CLASS, mxREAL);
        double*  ret_ptr = mxGetPr (plhs[0]);
        mpfr_ptr rop_ptr = &data[rop.start - 1];
        mpfr_ptr op_ptr = &data[op.start - 1];
        size_t ret_stride = (nlhs) ? 1 : 0;
        for (size_t i = 0; i < length (&rop); i++)
          ret_ptr[i * ret_stride] = (double) operator (rop_ptr + i,
                                                       op_ptr + i, rnd);
        break;
      }

      case 63:  // int mpfr_cmp (mpfr_t op1, mpfr_t op2)
      case 67:  // int mpfr_cmpabs (mpfr_t op1, mpfr_t op2)
      case 75:  // int mpfr_greater_p (mpfr_t op1, mpfr_t op2)
      case 76:  // int mpfr_greaterequal_p (mpfr_t op1, mpfr_t op2)
      case 77:  // int mpfr_less_p (mpfr_t op1, mpfr_t op2)
      case 78:  // int mpfr_lessequal_p (mpfr_t op1, mpfr_t op2)
      case 79:  // int mpfr_equal_p (mpfr_t op1, mpfr_t op2)
      case 80:  // int mpfr_lessgreater_p (mpfr_t op1, mpfr_t op2)
      case 81:  // int mpfr_unordered_p (mpfr_t op1, mpfr_t op2)
      case 82:  // int mpfr_total_order_p (mpfr_t x, mpfr_t y)
      {
        MEX_NARGINCHK(3);
        MEX_MPFR_T(1, op1);
        MEX_MPFR_T(2, op2);
        if (length (&op1) != length (&op2))
          {
            MEX_FCN_ERR ("cmd[%d]:op2 Invalid size.\n", cmd_code);
            break;
          }
        DBG_PRINTF ("cmd[%d]: op1 = [%d:%d], op2 = [%d:%d]\n",
                    cmd_code, op1.start, op1.end, op2.start, op2.end);

        int (*operator) (const mpfr_t, const mpfr_t);
        if (cmd_code == 63)
          operator = mpfr_cmp;
        else if (cmd_code == 67)
          operator = mpfr_cmpabs;
        else if (cmd_code == 75)
          operator = mpfr_greater_p;
        else if (cmd_code == 76)
          operator = mpfr_greaterequal_p;
        else if (cmd_code == 77)
          operator = mpfr_less_p;
        else if (cmd_code == 78)
          operator = mpfr_lessequal_p;
        else if (cmd_code == 79)
          operator = mpfr_equal_p;
        else if (cmd_code == 80)
          operator = mpfr_lessgreater_p;
        else if (cmd_code == 81)
          operator = mpfr_unordered_p;
        else if (cmd_code == 82)
          {
            #if (MPFR_VERSION < MPFR_VERSION_NUM(4,1,0))
              MEX_FCN_ERR ("cmd[%d]: Not supported in MPFR %s.\n",
                           cmd_code, MPFR_VERSION_STRING);
              break;
            #else
              operator = mpfr_total_order_p;
            #endif
          }
        else
          {
            MEX_FCN_ERR ("cmd[%d]: Bad operator.\n", cmd_code);
            break;
          }

        plhs[0] = mxCreateNumericMatrix (nlhs ? length (&op1): 1, 1,
                                         mxDOUBLE_CLASS, mxREAL);
        double*  ret_ptr = mxGetPr (plhs[0]);
        mpfr_ptr op1_ptr = &data[op1.start - 1];
        mpfr_ptr op2_ptr = &data[op2.start - 1];
        size_t ret_stride = (nlhs) ? 1 : 0;
        for (size_t i = 0; i < length (&op1); i++)
          ret_ptr[i * ret_stride] = (double) operator (op1_ptr + i,
                                                       op2_ptr + i);
        break;
      }

      case 166:  // void mpfr_nexttoward (mpfr_t x, mpfr_t y)
      {
        MEX_NARGINCHK(3);
        MEX_MPFR_T(1, op1);
        MEX_MPFR_T(2, op2);
        if (length (&op1) != length (&op2))
          {
            MEX_FCN_ERR ("cmd[%d]:op2 Invalid size.\n", cmd_code);
            break;
          }
        DBG_PRINTF ("cmd[%d]: op1 = [%d:%d], op2 = [%d:%d]\n",
                    cmd_code, op1.start, op1.end, op2.start, op2.end);

        for (size_t i = 0; i < length (&op1); i++)
          mpfr_nexttoward (&data[op1.start - 1 + i], &data[op2.start - 1 + i]);
        break;
      }

      case 22: // void mpfr_free_str (char *str)
      {
        // NOP
        break;
      }

      default:
      {
        MEX_FCN_ERR ("Unknown command code '%d'\n", cmd_code);
        break;
      }
    }

  if (throw_error)
    mexErrMsgIdAndTxt ("mp:mexFunction", "See previous error message.");
}

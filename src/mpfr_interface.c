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
        if (cmd_code == 7)
          {
            for (size_t i = 0; i < length (&rop); i++)
              {
                ret_ptr[i * ret_stride] = (double) mpfr_set_ui_2exp (
                  rop_ptr + i, (unsigned long) op_ptr[i *  op_stride],
                  (mpfr_exp_t) exp_ptr[i * exp_stride], rnd);
              }
          }
        else
          {
            for (size_t i = 0; i < length (&rop); i++)
              {
                ret_ptr[i * ret_stride] = (double) mpfr_set_si_2exp (
                  rop_ptr + i, (unsigned long) op_ptr[i *  op_stride],
                  (mpfr_exp_t) exp_ptr[i * exp_stride], rnd);
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
                ret_ptr[i * ret_stride] = (double) mpfr_strtofr (
                  &data[(idx.start - 1) + i], str, &endptr,
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
                ret_ptr[i * ret_stride] = (double) fcn (
                  &data[(idx.start - 1) + i], str,
                  (int) base_ptr[i * base_stride], rnd);
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
      case 166:  // void mpfr_nexttoward (mpfr_t x, mpfr_t y)
      {
        MEX_NARGINCHK(3);
        MEX_MPFR_T(1, x);
        MEX_MPFR_T(2, y);
        if (length (&x) != length (&y))
          {
            MEX_FCN_ERR ("cmd[%d]:y Invalid size.\n", cmd_code);
            break;
          }
        DBG_PRINTF ("cmd[%d]: x = [%d:%d], y = [%d:%d]\n",
                    cmd_code, x.start, x.end, y.start, y.end);

        if (cmd_code == 13)
          for (size_t i = 0; i < length (&x); i++)
            mpfr_swap (&data[x.start - 1 + i], &data[y.start - 1 + i]);
        else
          for (size_t i = 0; i < length (&x); i++)
            mpfr_nexttoward (&data[x.start - 1 + i], &data[y.start - 1 + i]);
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
            ret_ptr[i * ret_stride] = (double) mpfr_init_set (
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
          ret_ptr[i] = (double) mpfr_get_d (&data[(op.start - 1) + i], rnd);
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
            ret_ptr[i] = (double) mpfr_get_d_2exp (&exp,
                                                   &data[(op.start - 1) + i],
                                                   rnd);
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
            ret_ptr[i] = (double) mpfr_frexp (&exp, &data[(y.start - 1) + i],
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
          ret_ptr[i] = (double) mpfr_get_str_ndigits (
            (int) op1_ptr[i * op1_stride],
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

        plhs[0] = mxCreateNumericMatrix (nlhs ? length (&rop): 1, 1,
                                         mxDOUBLE_CLASS, mxREAL);
        double* ret_ptr = mxGetPr (plhs[0]);
        double* op_pr = mxGetPr (prhs[2]);
        size_t ret_stride = (nlhs) ? 1 : 0;
        size_t op_stride = ((opM * opN) == 1) ? 0 : 1;

        if (cmd_code == 6)
          for (size_t i = 0; i < length (&rop); i++)
            ret_ptr[i * ret_stride] = (double) mpfr_set_d (
              &data[(rop.start - 1) + i], op_pr[i * op_stride], rnd);
        else
          for (size_t i = 0; i < length (&rop); i++)
            {
              mpfr_clear(&data[(rop.start - 1) + i]);
              ret_ptr[i * ret_stride] = (double) mpfr_init_set_d (
                &data[(rop.start - 1) + i], op_pr[i * op_stride], rnd);
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
      case 60:  // int mpfr_hypot (mpfr_t rop, mpfr_t x, mpfr_t y, mpfr_rnd_t rnd)
      case 92:  // int mpfr_pow (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_rnd_t rnd)
      case 108:  // int mpfr_atan2 (mpfr_t rop, mpfr_t y, mpfr_t x, mpfr_rnd_t rnd)
      case 122:  // int mpfr_gamma_inc (mpfr_t rop, mpfr_t op, mpfr_t op2, mpfr_rnd_t rnd)
      case 126:  // int mpfr_beta (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_rnd_t rnd)
      case 137:  // int mpfr_agm (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_rnd_t rnd)
      case 156:  // int mpfr_fmod (mpfr_t r, mpfr_t x, mpfr_t y, mpfr_rnd_t rnd)
      case 158:  // int mpfr_remainder (mpfr_t r, mpfr_t x, mpfr_t y, mpfr_rnd_t rnd)
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

        int (*fcn) (mpfr_t, const mpfr_t, const mpfr_t, mpfr_rnd_t);
        if (cmd_code == 31)
          fcn = mpfr_add;
        else if (cmd_code == 33)
          fcn = mpfr_sub;
        else if (cmd_code == 36)
          fcn = mpfr_mul;
        else if (cmd_code == 39)
          fcn = mpfr_div;
        else if (cmd_code == 50)
          fcn = mpfr_dim;
        else if (cmd_code == 60)
          fcn = mpfr_hypot;
        else if (cmd_code == 92)
          fcn = mpfr_pow;
        else if (cmd_code == 108)
          fcn = mpfr_atan2;
        else if (cmd_code == 122)
          fcn = mpfr_gamma_inc;
        else if (cmd_code == 126)
          fcn = mpfr_beta;
        else if (cmd_code == 137)
          fcn = mpfr_agm;
        else if (cmd_code == 156)
          fcn = mpfr_fmod;
        else if (cmd_code == 158)
          fcn = mpfr_remainder;
        else if (cmd_code == 169)
          fcn = mpfr_min;
        else if (cmd_code == 170)
          fcn = mpfr_max;
        else if (cmd_code == 180)
          fcn = mpfr_copysign;
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
              (double) fcn (rop_ptr + i, op1_ptr + (i * op1_stride),
                            op2_ptr + (i * op2_stride), rnd);
          }
        break;
      }

      case 101:  // int mpfr_sin_cos (mpfr_t sop, mpfr_t cop, mpfr_t op, mpfr_rnd_t rnd)
      case 112:  // int mpfr_sinh_cosh (mpfr_t sop, mpfr_t cop, mpfr_t op, mpfr_rnd_t rnd)
      case 155:  // int mpfr_modf (mpfr_t iop, mpfr_t fop, mpfr_t op, mpfr_rnd_t rnd)
      {
        MEX_NARGINCHK(5);
        MEX_MPFR_T(1, sop);
        MEX_MPFR_T(2, cop);
        MEX_MPFR_T(3, op);
        if (length (&sop) != length (&cop))
          {
            MEX_FCN_ERR ("cmd[%d]:sop and cop must have the same size.\n",
                         cmd_code);
            break;
          }
        if (sop.start == cop.start)
          {
            MEX_FCN_ERR ("cmd[%d]:sop and cop must be different variables.\n",
                         cmd_code);
            break;
          }
        if ((length (&op) != length (&sop)) && (length (&op) != 1))
          {
            MEX_FCN_ERR ("cmd[%d]:op Invalid size.\n", cmd_code);
            break;
          }
        MEX_MPFR_RND_T(4, rnd);
        DBG_PRINTF ("cmd[%d]: sop = [%d:%d], cop = [%d:%d], op = [%d:%d] "
                    "(rnd = %d)\n", cmd_code, sop.start, sop.end,
                    cop.start, cop.end, op.start, op.end, (int) rnd);

        int (*fcn) (mpfr_t, mpfr_t, const mpfr_t, mpfr_rnd_t);
        if (cmd_code == 101)
          fcn = mpfr_sin_cos;
        else if (cmd_code == 112)
          fcn = mpfr_sinh_cosh;
        else if (cmd_code == 155)
          fcn = mpfr_modf;
        else
          {
            MEX_FCN_ERR ("cmd[%d]: Bad operator.\n", cmd_code);
            break;
          }

        plhs[0] = mxCreateNumericMatrix (nlhs ? length (&sop): 1, 1,
                                         mxDOUBLE_CLASS, mxREAL);
        double*  ret_ptr = mxGetPr (plhs[0]);
        mpfr_ptr sop_ptr = &data[sop.start - 1];
        mpfr_ptr cop_ptr = &data[cop.start - 1];
        mpfr_ptr  op_ptr = &data[op.start - 1];
        size_t ret_stride = (nlhs) ? 1 : 0;
        size_t  op_stride = (length (&op) == 1) ? 0 : 1;
        for (size_t i = 0; i < length (&sop); i++)
          {
            ret_ptr[i * ret_stride] = (double) fcn (sop_ptr + i, cop_ptr + i,
                                                    op_ptr + (i * op_stride),
                                                    rnd);
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

        int (*fcn) (mpfr_t, const mpfr_t, const double, mpfr_rnd_t);
        if (cmd_code == 32)
          fcn = mpfr_add_d;
        else if (cmd_code == 35)
          fcn = mpfr_sub_d;
        else if (cmd_code == 37)
          fcn = mpfr_mul_d;
        else if (cmd_code == 41)
          fcn = mpfr_div_d;
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
              (double) fcn (rop_ptr + i, op1_ptr + i, op2_ptr[i * op2_stride],
                            rnd);
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

        int (*fcn) (mpfr_t, const double, const mpfr_t, mpfr_rnd_t) =
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
              (double) fcn (rop_ptr + i, op1_ptr[i * op1_stride],
                            op2_ptr + i, rnd);
          }
        break;
      }

      case 5:    // int mpfr_set (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 38:   // int mpfr_sqr (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 42:   // int mpfr_sqrt (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 44:   // int mpfr_rec_sqrt (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 45:   // int mpfr_cbrt (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
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
      case 138:  // int mpfr_ai (mpfr_t rop, mpfr_t x, mpfr_rnd_t rnd)
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

        int (*fcn) (mpfr_t, const mpfr_t, mpfr_rnd_t);
        if (cmd_code == 5)
          fcn = mpfr_set;
        else if (cmd_code == 38)
          fcn = mpfr_sqr;
        else if (cmd_code == 42)
          fcn = mpfr_sqrt;
        else if (cmd_code == 44)
          fcn = mpfr_rec_sqrt;
        else if (cmd_code == 48)
          fcn = mpfr_neg;
        else if (cmd_code == 49)
          fcn = mpfr_abs;
        else if (cmd_code == 83)
          fcn = mpfr_log;
        else if (cmd_code == 85)
          fcn = mpfr_log2;
        else if (cmd_code == 86)
          fcn = mpfr_log10;
        else if (cmd_code == 87)
          fcn = mpfr_log1p;
        else if (cmd_code == 88)
          fcn = mpfr_exp;
        else if (cmd_code == 89)
          fcn = mpfr_exp2;
        else if (cmd_code == 90)
          fcn = mpfr_exp10;
        else if (cmd_code == 91)
          fcn = mpfr_expm1;
        else if (cmd_code == 98)
          fcn = mpfr_cos;
        else if (cmd_code == 99)
          fcn = mpfr_sin;
        else if (cmd_code == 100)
          fcn = mpfr_tan;
        else if (cmd_code == 102)
          fcn = mpfr_sec;
        else if (cmd_code == 103)
          fcn = mpfr_csc;
        else if (cmd_code == 104)
          fcn = mpfr_cot;
        else if (cmd_code == 105)
          fcn = mpfr_acos;
        else if (cmd_code == 106)
          fcn = mpfr_asin;
        else if (cmd_code == 107)
          fcn = mpfr_atan;
        else if (cmd_code == 109)
          fcn = mpfr_cosh;
        else if (cmd_code == 110)
          fcn = mpfr_sinh;
        else if (cmd_code == 111)
          fcn = mpfr_tanh;
        else if (cmd_code == 113)
          fcn = mpfr_sech;
        else if (cmd_code == 114)
          fcn = mpfr_csch;
        else if (cmd_code == 115)
          fcn = mpfr_coth;
        else if (cmd_code == 116)
          fcn = mpfr_acosh;
        else if (cmd_code == 117)
          fcn = mpfr_asinh;
        else if (cmd_code == 118)
          fcn = mpfr_atanh;
        else if (cmd_code == 119)
          fcn = mpfr_eint;
        else if (cmd_code == 120)
          fcn = mpfr_li2;
        else if (cmd_code == 121)
          fcn = mpfr_gamma;
        else if (cmd_code == 123)
          fcn = mpfr_lngamma;
        else if (cmd_code == 125)
          fcn = mpfr_digamma;
        else if (cmd_code == 127)
          fcn = mpfr_zeta;
        else if (cmd_code == 129)
          fcn = mpfr_erf;
        else if (cmd_code == 130)
          fcn = mpfr_erfc;
        else if (cmd_code == 131)
          fcn = mpfr_j0;
        else if (cmd_code == 132)
          fcn = mpfr_j1;
        else if (cmd_code == 134)
          fcn = mpfr_y0;
        else if (cmd_code == 135)
          fcn = mpfr_y1;
        else if (cmd_code == 138)
          fcn = mpfr_ai;
        else if (cmd_code == 143)
          fcn = mpfr_rint;
        else if (cmd_code == 149)
          fcn = mpfr_rint_ceil;
        else if (cmd_code == 150)
          fcn = mpfr_rint_floor;
        else if (cmd_code == 151)
          fcn = mpfr_rint_round;
        else if (cmd_code == 152)
          fcn = mpfr_rint_roundeven;
        else if (cmd_code == 153)
          fcn = mpfr_rint_trunc;
        else if (cmd_code == 154)
          fcn = mpfr_frac;
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
          ret_ptr[i * ret_stride] = (double) fcn (rop_ptr + i, op_ptr + i, rnd);
        break;
      }

      case 43:  // int mpfr_sqrt_ui (mpfr_t rop, unsigned long int op, mpfr_rnd_t rnd)
      case 55:  // int mpfr_fac_ui (mpfr_t rop, unsigned long int op, mpfr_rnd_t rnd)
      case 84:  // int mpfr_log_ui (mpfr_t rop, unsigned long op, mpfr_rnd_t rnd)
      case 128:  // int mpfr_zeta_ui (mpfr_t rop, unsigned long op, mpfr_rnd_t rnd)
      {
        MEX_NARGINCHK(4);
        MEX_MPFR_T(1, rop);
        size_t opM = mxGetM (prhs[2]);
        size_t opN = mxGetN (prhs[2]);
        if (! mxIsDouble (prhs[2])
            || (((opM * opN) != length (&rop)) && ((opM * opN) != 1)))
          {
            MEX_FCN_ERR ("cmd[%d]:op Invalid.\n", cmd_code);
            break;
          }
        MEX_MPFR_RND_T(3, rnd);
        DBG_PRINTF ("cmd[%d]: rop = [%d:%d], op = [%d:%d] (rnd = %d)\n",
                    cmd_code, rop.start, rop.end, opM, opN, (int) rnd);

        int (*fcn)(mpfr_t, unsigned long int, mpfr_rnd_t);
        if (cmd_code == 43)
          fcn = mpfr_sqrt_ui;
        else if (cmd_code == 55)
          fcn = mpfr_fac_ui;
        else if (cmd_code == 84)
          fcn = mpfr_log_ui;
        else if (cmd_code == 128)
          fcn = mpfr_zeta_ui;
        else
          {
            MEX_FCN_ERR ("cmd[%d]: Bad operator.\n", cmd_code);
            break;
          }

        plhs[0] = mxCreateNumericMatrix (nlhs ? length (&rop): 1, 1,
                                         mxDOUBLE_CLASS, mxREAL);
        double*  ret_ptr = mxGetPr (plhs[0]);
        mpfr_ptr rop_ptr = &data[rop.start - 1];
        double*   op_ptr = mxGetPr (prhs[2]);
        size_t ret_stride = (nlhs) ? 1 : 0;
        size_t  op_stride = ((opM * opN) == 1) ? 0 : 1;
        for (size_t i = 0; i < length (&rop); i++)
          ret_ptr[i * ret_stride] = (double) fcn (rop_ptr + i,
            (unsigned long int) op_ptr[i * op_stride], rnd);

        break;
      }

      case 46:  // int mpfr_rootn_ui (mpfr_t rop, mpfr_t op, unsigned long int n, mpfr_rnd_t rnd)
      case 51:  // int mpfr_mul_2ui (mpfr_t rop, mpfr_t op1, unsigned long int op2, mpfr_rnd_t rnd)
      case 53:  // int mpfr_div_2ui (mpfr_t rop, mpfr_t op1, unsigned long int op2, mpfr_rnd_t rnd)
      case 93:  // int mpfr_pow_ui (mpfr_t rop, mpfr_t op1, unsigned long int op2, mpfr_rnd_t rnd)
      {
        MEX_NARGINCHK(5);
        MEX_MPFR_T(1, rop);
        MEX_MPFR_T(2, op1);
        if ((length (&op1) != length (&rop)) && (length (&op1) != 1))
          {
            MEX_FCN_ERR ("cmd[%d]:op1 Invalid.\n", cmd_code);
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
        DBG_PRINTF ("cmd[%d]: rop = [%d:%d], op1 = [%d:%d] , op2 = [%d x %d] "
                    "(rnd = %d)\n", cmd_code, rop.start, rop.end,
                    op1.start, op1.end, op2M, op2N, (int) rnd);

        int (*fcn)(mpfr_t, const mpfr_t, unsigned long int, mpfr_rnd_t);
        if (cmd_code == 46)
          fcn = mpfr_rootn_ui;
        else if (cmd_code == 51)
          fcn = mpfr_mul_2ui;
        else if (cmd_code == 53)
          fcn = mpfr_div_2ui;
        else if (cmd_code == 93)
          fcn = mpfr_pow_ui;
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
        size_t op1_stride = (length (&op1) == 1) ? 0 : 1;
        size_t op2_stride = ((op2M * op2N) == 1) ? 0 : 1;
        for (size_t i = 0; i < length (&rop); i++)
          ret_ptr[i * ret_stride] = (double) fcn (rop_ptr + i,
            op1_ptr + (i * op1_stride),
            (unsigned long int) op2_ptr[i * op2_stride], rnd);

        break;
      }

      case 52:  // int mpfr_mul_2si (mpfr_t rop, mpfr_t op1, long int op2, mpfr_rnd_t rnd)
      case 54:  // int mpfr_div_2si (mpfr_t rop, mpfr_t op1, long int op2, mpfr_rnd_t rnd)
      case 94:  // int mpfr_pow_si (mpfr_t rop, mpfr_t op1, long int op2, mpfr_rnd_t rnd)
      {
        MEX_NARGINCHK(5);
        MEX_MPFR_T(1, rop);
        MEX_MPFR_T(2, op1);
        if ((length (&op1) != length (&rop)) && (length (&op1) != 1))
          {
            MEX_FCN_ERR ("cmd[%d]:op1 Invalid.\n", cmd_code);
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
        DBG_PRINTF ("cmd[%d]: rop = [%d:%d], op1 = [%d:%d] , op2 = [%d x %d] "
                    "(rnd = %d)\n", cmd_code, rop.start, rop.end,
                    op1.start, op1.end, op2M, op2N, (int) rnd);

        int (*fcn)(mpfr_t, const mpfr_t, long int, mpfr_rnd_t);
        if (cmd_code == 52)
          fcn = mpfr_mul_2si;
        else if (cmd_code == 54)
          fcn = mpfr_div_2si;
        else if (cmd_code == 94)
          fcn = mpfr_pow_si;
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
        size_t op1_stride = (length (&op1) == 1) ? 0 : 1;
        size_t op2_stride = ((op2M * op2N) == 1) ? 0 : 1;
        for (size_t i = 0; i < length (&rop); i++)
          ret_ptr[i * ret_stride] = (double) fcn (rop_ptr + i,
            op1_ptr + (i * op1_stride), (long int) op2_ptr[i * op2_stride],
            rnd);

        break;
      }

      case 47:  // int mpfr_root (mpfr_t rop, mpfr_t op, unsigned long int n, mpfr_rnd_t rnd)
      {
        //TODO: deprecated
        mexErrMsgIdAndTxt("mpfr_interface:mpfr_root",
                          "mpfr_root is deprecated and might be removed in a "
                          "future version of MPFR.", cmd_code);
        MEX_NARGINCHK(5);
        MEX_MPFR_T(1, rop);
        MEX_MPFR_T(2, op1);
        if ((length (&op1) != length (&rop)) && (length (&op1) != 1))
          {
            MEX_FCN_ERR ("cmd[%d]:op1 Invalid.\n", cmd_code);
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
        DBG_PRINTF ("cmd[%d]: rop = [%d:%d], op1 = [%d:%d] , op2 = [%d x %d] "
                    "(rnd = %d)\n", cmd_code, rop.start, rop.end,
                    op1.start, op1.end, op2M, op2N, (int) rnd);

        plhs[0] = mxCreateNumericMatrix (nlhs ? length (&rop): 1, 1,
                                         mxDOUBLE_CLASS, mxREAL);
        double*  ret_ptr = mxGetPr (plhs[0]);
        mpfr_ptr rop_ptr = &data[rop.start - 1];
        mpfr_ptr op1_ptr = &data[op1.start - 1];
        double*  op2_ptr = mxGetPr (prhs[3]);
        size_t ret_stride = (nlhs) ? 1 : 0;
        size_t op1_stride = (length (&op1) == 1) ? 0 : 1;
        size_t op2_stride = ((op2M * op2N) == 1) ? 0 : 1;
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wdeprecated-declarations"
        for (size_t i = 0; i < length (&rop); i++)
          ret_ptr[i * ret_stride] = (double) mpfr_root (rop_ptr + i,
            op1_ptr + (i * op1_stride),
            (unsigned long int) op2_ptr[i * op2_stride], rnd);
        #pragma GCC diagnostic pop
        break;
      }

      case 56:  // int mpfr_fma (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_t op3, mpfr_rnd_t rnd)
      case 57:  // int mpfr_fms (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_t op3, mpfr_rnd_t rnd)
      {
        MEX_NARGINCHK(6);
        MEX_MPFR_T(1, rop);
        MEX_MPFR_T(2, op1);
        if ((length (&op1) != length (&rop)) && (length (&op1) != 1))
          {
            MEX_FCN_ERR ("cmd[%d]:op1 Invalid.\n", cmd_code);
            break;
          }
        MEX_MPFR_T(3, op2);
        if ((length (&op2) != length (&rop)) && (length (&op2) != 1))
          {
            MEX_FCN_ERR ("cmd[%d]:op2 Invalid.\n", cmd_code);
            break;
          }
        MEX_MPFR_T(4, op3);
        if ((length (&op3) != length (&rop)) && (length (&op3) != 1))
          {
            MEX_FCN_ERR ("cmd[%d]:op3 Invalid.\n", cmd_code);
            break;
          }
        MEX_MPFR_RND_T(5, rnd);
        DBG_PRINTF ("cmd[%d]: rop = [%d:%d], op1 = [%d:%d], op2 = [%d:%d], "
                    "op3 = [%d:%d] (rnd = %d)\n", cmd_code, rop.start, rop.end,
                    op1.start, op1.end, op2.start, op2.end, op3.start, op3.end,
                    (int) rnd);

        int (*fcn)(mpfr_t, const mpfr_t, const mpfr_t,
                   const mpfr_t, mpfr_rnd_t) = ((cmd_code == 56) ? mpfr_fma
                                                                 : mpfr_fms);

        plhs[0] = mxCreateNumericMatrix (nlhs ? length (&rop): 1, 1,
                                         mxDOUBLE_CLASS, mxREAL);
        double*  ret_ptr = mxGetPr (plhs[0]);
        mpfr_ptr rop_ptr = &data[rop.start - 1];
        mpfr_ptr op1_ptr = &data[op1.start - 1];
        mpfr_ptr op2_ptr = &data[op2.start - 1];
        mpfr_ptr op3_ptr = &data[op3.start - 1];
        size_t ret_stride = (nlhs) ? 1 : 0;
        size_t op1_stride = (length (&op1) == 1) ? 0 : 1;
        size_t op2_stride = (length (&op2) == 1) ? 0 : 1;
        size_t op3_stride = (length (&op3) == 1) ? 0 : 1;
        for (size_t i = 0; i < length (&rop); i++)
          ret_ptr[i * ret_stride] = (double) fcn (rop_ptr + i,
            op1_ptr + (i * op1_stride), op2_ptr + (i * op2_stride),
            op3_ptr + (i * op3_stride), rnd);
        break;
      }

      case 58:  // int mpfr_fmma (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_t op3, mpfr_t op4, mpfr_rnd_t rnd)
      case 59:  // int mpfr_fmms (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_t op3, mpfr_t op4, mpfr_rnd_t rnd)
      {
        MEX_NARGINCHK(6);
        MEX_MPFR_T(1, rop);
        MEX_MPFR_T(2, op1);
        if ((length (&op1) != length (&rop)) && (length (&op1) != 1))
          {
            MEX_FCN_ERR ("cmd[%d]:op1 Invalid.\n", cmd_code);
            break;
          }
        MEX_MPFR_T(3, op2);
        if ((length (&op2) != length (&rop)) && (length (&op2) != 1))
          {
            MEX_FCN_ERR ("cmd[%d]:op2 Invalid.\n", cmd_code);
            break;
          }
        MEX_MPFR_T(4, op3);
        if ((length (&op3) != length (&rop)) && (length (&op3) != 1))
          {
            MEX_FCN_ERR ("cmd[%d]:op3 Invalid.\n", cmd_code);
            break;
          }
        MEX_MPFR_T(5, op4);
        if ((length (&op4) != length (&rop)) && (length (&op4) != 1))
          {
            MEX_FCN_ERR ("cmd[%d]:op4 Invalid.\n", cmd_code);
            break;
          }
        MEX_MPFR_RND_T(6, rnd);
        DBG_PRINTF ("cmd[%d]: rop = [%d:%d], op1 = [%d:%d], op2 = [%d:%d], "
                    "op3 = [%d:%d], op4 = [%d:%d] (rnd = %d)\n", cmd_code,
                    rop.start, rop.end, op1.start, op1.end, op2.start, op2.end,
                    op3.start, op3.end, op4.start, op4.end, (int) rnd);

        int (*fcn)(mpfr_t, const mpfr_t, const mpfr_t, const mpfr_t,
                   const mpfr_t, mpfr_rnd_t) = ((cmd_code == 58) ? mpfr_fmma
                                                                 : mpfr_fmms);

        plhs[0] = mxCreateNumericMatrix (nlhs ? length (&rop): 1, 1,
                                         mxDOUBLE_CLASS, mxREAL);
        double*  ret_ptr = mxGetPr (plhs[0]);
        mpfr_ptr rop_ptr = &data[rop.start - 1];
        mpfr_ptr op1_ptr = &data[op1.start - 1];
        mpfr_ptr op2_ptr = &data[op2.start - 1];
        mpfr_ptr op3_ptr = &data[op3.start - 1];
        mpfr_ptr op4_ptr = &data[op4.start - 1];
        size_t ret_stride = (nlhs) ? 1 : 0;
        size_t op1_stride = (length (&op1) == 1) ? 0 : 1;
        size_t op2_stride = (length (&op2) == 1) ? 0 : 1;
        size_t op3_stride = (length (&op3) == 1) ? 0 : 1;
        size_t op4_stride = (length (&op4) == 1) ? 0 : 1;
        for (size_t i = 0; i < length (&rop); i++)
          ret_ptr[i * ret_stride] = (double) fcn (rop_ptr + i,
            op1_ptr + (i * op1_stride), op2_ptr + (i * op2_stride),
            op3_ptr + (i * op3_stride), op4_ptr + (i * op4_stride), rnd);
        break;
      }

      case 61:  // int mpfr_sum (mpfr_t rop, const mpfr_ptr tab[], unsigned long int n, mpfr_rnd_t rnd)
      {
        MEX_NARGINCHK(5);
        MEX_MPFR_T(1, rop);
        if (length (&rop) != 1)
          {
            MEX_FCN_ERR ("cmd[%d]:rop must be a scalar MPFR variable.\n",
                         cmd_code);
            break;
          }
        MEX_MPFR_T(2, tab);
        uint64_t n = 0;
        if (! extract_ui (3, nrhs, prhs, &n))
          {
            MEX_FCN_ERR ("cmd[%d]:n must be a positive numeric scalar.\n",
                         cmd_code);
            break;
          }
        if (length (&tab) < n)
          {
            MEX_FCN_ERR ("cmd[%d]:tab must be a MPFR vector of at least ",
                         "length %d.\n", cmd_code, (int) n);
            break;
          }
        MEX_MPFR_RND_T(4, rnd);
        DBG_PRINTF ("cmd[%d]: rop = [%d:%d], tab = [%d:%d], n = %d "
                    "(rnd = %d)\n", cmd_code, rop.start, rop.end,
                    tab.start, tab.end, (int) n, (int) rnd);

        plhs[0] = mxCreateNumericMatrix (1, 1, mxDOUBLE_CLASS, mxREAL);
        double*  ret_ptr = mxGetPr (plhs[0]);
        mpfr_ptr rop_ptr = &data[rop.start - 1];

        mpfr_ptr* tab_ptr = (mpfr_ptr*) mxMalloc(n * sizeof (mpfr_ptr*));
        for (size_t i = 0; i < n; i++)
          tab_ptr[i] = data + tab.start - 1 + i;

        *ret_ptr = (double) mpfr_sum (rop_ptr, tab_ptr, n, rnd);
        mxFree(tab_ptr);
        break;
      }

      case 62:  // int mpfr_dot (mpfr_t rop, const mpfr_ptr a[], const mpfr_ptr b[], unsigned long int n, mpfr_rnd_t rnd)
      {
        #if (MPFR_VERSION < MPFR_VERSION_NUM(4,1,0))
        MEX_FCN_ERR ("cmd[%d]: Not supported in MPFR %s.\n",
                      cmd_code, MPFR_VERSION_STRING);
        #else
        MEX_NARGINCHK(6);
        MEX_MPFR_T(1, rop);
        if (length (&rop) != 1)
          {
            MEX_FCN_ERR ("cmd[%d]:rop must be a scalar MPFR variable.\n",
                         cmd_code);
            break;
          }
        MEX_MPFR_T(2, a);
        MEX_MPFR_T(3, b);
        uint64_t n = 0;
        if (! extract_ui (4, nrhs, prhs, &n))
          {
            MEX_FCN_ERR ("cmd[%d]:n must be a positive numeric scalar.\n",
                         cmd_code);
            break;
          }
        if ((length (&a) < n) || (length (&b) < n))
          {
            MEX_FCN_ERR ("cmd[%d]:a and b must be MPFR vectors of at least ",
                         "length %d.\n", cmd_code, (int) n);
            break;
          }
        MEX_MPFR_RND_T(5, rnd);
        DBG_PRINTF ("cmd[%d]: rop = [%d:%d], a = [%d:%d], b = [%d:%d], "
                    "n = %d (rnd = %d)\n", cmd_code, rop.start, rop.end,
                    a.start, a.end, b.start, b.end, (int) n, (int) rnd);

        plhs[0] = mxCreateNumericMatrix (1, 1, mxDOUBLE_CLASS, mxREAL);
        double*  ret_ptr = mxGetPr (plhs[0]);
        mpfr_ptr rop_ptr = &data[rop.start - 1];

        mpfr_ptr* a_ptr = (mpfr_ptr*) mxMalloc(n * sizeof (mpfr_ptr*));
        mpfr_ptr* b_ptr = (mpfr_ptr*) mxMalloc(n * sizeof (mpfr_ptr*));
        for (size_t i = 0; i < n; i++)
          {
            a_ptr[i] = data + a.start - 1 + i;
            b_ptr[i] = data + b.start - 1 + i;
          }

        *ret_ptr = (double) mpfr_dot (rop_ptr, a_ptr, b_ptr, n, rnd);
        mxFree(a_ptr);
        mxFree(b_ptr);
        #endif
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

        int (*fcn) (const mpfr_t, const mpfr_t);
        if (cmd_code == 63)
          fcn = mpfr_cmp;
        else if (cmd_code == 67)
          fcn = mpfr_cmpabs;
        else if (cmd_code == 75)
          fcn = mpfr_greater_p;
        else if (cmd_code == 76)
          fcn = mpfr_greaterequal_p;
        else if (cmd_code == 77)
          fcn = mpfr_less_p;
        else if (cmd_code == 78)
          fcn = mpfr_lessequal_p;
        else if (cmd_code == 79)
          fcn = mpfr_equal_p;
        else if (cmd_code == 80)
          fcn = mpfr_lessgreater_p;
        else if (cmd_code == 81)
          fcn = mpfr_unordered_p;
        else if (cmd_code == 82)
          {
            #if (MPFR_VERSION < MPFR_VERSION_NUM(4,1,0))
              MEX_FCN_ERR ("cmd[%d]: Not supported in MPFR %s.\n",
                           cmd_code, MPFR_VERSION_STRING);
              break;
            #else
              fcn = mpfr_total_order_p;
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
          ret_ptr[i * ret_stride] = (double) fcn (op1_ptr + i, op2_ptr + i);
        break;
      }

      case 64:  // int mpfr_cmp_d (mpfr_t op1, double op2)
      case 68:  // int mpfr_cmpabs_ui (mpfr_t op1, unsigned long op2)
      {
        MEX_NARGINCHK(3);
        MEX_MPFR_T(1, op1);
        size_t op2M = mxGetM (prhs[2]);
        size_t op2N = mxGetN (prhs[2]);
        if (! mxIsDouble (prhs[2])
            || (((op2M * op2N) != length (&op1)) && ((op2M * op2N) != 1)))
          {
            MEX_FCN_ERR ("cmd[%d]:op2 Invalid.\n", cmd_code);
            break;
          }
        DBG_PRINTF ("cmd[%d]: op1 = [%d:%d] , op2 = [%d x %d]\n", cmd_code,
                    op1.start, op1.end, op2M, op2N);

        plhs[0] = mxCreateNumericMatrix (
          (nlhs ? MAX(length (&op1), (op2M * op2N)) : 1), 1, mxDOUBLE_CLASS,
          mxREAL);
        double*  ret_ptr = mxGetPr (plhs[0]);
        mpfr_ptr op1_ptr = &data[op1.start - 1];
        double*  op2_ptr = mxGetPr (prhs[2]);
        size_t ret_stride = (nlhs) ? 1 : 0;
        size_t op1_stride = (length (&op1) == 1) ? 0 : 1;
        size_t op2_stride = ((op2M * op2N) == 1) ? 0 : 1;
        if (cmd_code == 64)
          for (size_t i = 0; i < MAX(length (&op1), (op2M * op2N)); i++)
            ret_ptr[i * ret_stride] = (double) mpfr_cmp_d (
              op1_ptr + (i * op1_stride), op2_ptr[i * op2_stride]);
        else
          {
          #if (MPFR_VERSION < MPFR_VERSION_NUM(4,1,0))
            mxFree(plhs[0]);
            MEX_FCN_ERR ("cmd[%d]: Not supported in MPFR %s.\n",
                         cmd_code, MPFR_VERSION_STRING);
            break;
          #else
            for (size_t i = 0; i < MAX(length (&op1), (op2M * op2N)); i++)
              ret_ptr[i * ret_stride] = (double) mpfr_cmpabs_ui (
                op1_ptr + (i * op1_stride),
                (unsigned long) op2_ptr[i * op2_stride]);
          #endif
          }
        break;
      }

      case 65:  // int mpfr_cmp_ui_2exp (mpfr_t op1, unsigned long int op2, mpfr_exp_t e)
      case 66:  // int mpfr_cmp_si_2exp (mpfr_t op1, long int op2, mpfr_exp_t e)
      {
        MEX_NARGINCHK(4);
        MEX_MPFR_T(1, op1);
        size_t op2M = mxGetM (prhs[2]);
        size_t op2N = mxGetN (prhs[2]);
        if (! mxIsDouble (prhs[2])
            || (((op2M * op2N) != length (&op1)) && ((op2M * op2N) != 1)))
          {
            MEX_FCN_ERR ("cmd[%d]:op2 Invalid.\n", cmd_code);
            break;
          }
        MEX_MPFR_EXP_T(3, e);
        DBG_PRINTF ("cmd[%d]: op1 = [%d:%d] , op2 = [%d x %d]\n", cmd_code,
                    op1.start, op1.end, op2M, op2N);

        plhs[0] = mxCreateNumericMatrix (
          (nlhs ? MAX(length (&op1), (op2M * op2N)) : 1), 1, mxDOUBLE_CLASS,
          mxREAL);
        double*  ret_ptr = mxGetPr (plhs[0]);
        mpfr_ptr op1_ptr = &data[op1.start - 1];
        double*  op2_ptr = mxGetPr (prhs[2]);
        size_t ret_stride = (nlhs) ? 1 : 0;
        size_t op1_stride = (length (&op1) == 1) ? 0 : 1;
        size_t op2_stride = ((op2M * op2N) == 1) ? 0 : 1;
        if (cmd_code == 65)
          for (size_t i = 0; i < MAX(length (&op1), (op2M * op2N)); i++)
            ret_ptr[i * ret_stride] = (double) mpfr_cmp_ui_2exp (
              op1_ptr + (i * op1_stride),
              (unsigned long int) op2_ptr[i * op2_stride], e);
        else
          for (size_t i = 0; i < MAX(length (&op1), (op2M * op2N)); i++)
            ret_ptr[i * ret_stride] = (double) mpfr_cmp_si_2exp (
              op1_ptr + (i * op1_stride), (long int) op2_ptr[i * op2_stride],
              e);
        break;
      }

      case 139:  // int mpfr_const_log2 (mpfr_t rop, mpfr_rnd_t rnd)
      case 140:  // int mpfr_const_pi (mpfr_t rop, mpfr_rnd_t rnd)
      case 141:  // int mpfr_const_euler (mpfr_t rop, mpfr_rnd_t rnd)
      case 142:  // int mpfr_const_catalan (mpfr_t rop, mpfr_rnd_t rnd)
      {
        MEX_NARGINCHK(3);
        MEX_MPFR_T(1, rop);
        MEX_MPFR_RND_T(2, rnd);
        DBG_PRINTF ("cmd[%d]: rop = [%d:%d] (rnd = %d)\n",
                    cmd_code, rop.start, rop.end, (int) rnd);

        int (*fcn)(mpfr_t, mpfr_rnd_t);
        if (cmd_code == 139)
          fcn = mpfr_const_log2;
        else if (cmd_code == 140)
          fcn = mpfr_const_pi;
        else if (cmd_code == 141)
          fcn = mpfr_const_euler;
        else if (cmd_code == 142)
          fcn = mpfr_const_catalan;
        else
          {
            MEX_FCN_ERR ("cmd[%d]: Bad operator.\n", cmd_code);
            break;
          }

        plhs[0] = mxCreateNumericMatrix (nlhs ? length (&rop): 1, 1,
                                         mxDOUBLE_CLASS, mxREAL);
        double* ret_ptr = mxGetPr (plhs[0]);
        size_t ret_stride = (nlhs) ? 1 : 0;

        for (size_t i = 0; i < length (&rop); i++)
          ret_ptr[i * ret_stride] = (double) fcn (&data[rop.start - 1 + i],
                                                  rnd);
        break;
      }

      case 144:  // int mpfr_ceil (mpfr_t rop, mpfr_t op)
      case 145:  // int mpfr_floor (mpfr_t rop, mpfr_t op)
      case 146:  // int mpfr_round (mpfr_t rop, mpfr_t op)
      case 147:  // int mpfr_roundeven (mpfr_t rop, mpfr_t op)
      case 148:  // int mpfr_trunc (mpfr_t rop, mpfr_t op)
      {
        MEX_NARGINCHK(3);
        MEX_MPFR_T(1, rop);
        MEX_MPFR_T(2, op);
        if (length (&rop) != length (&op))
          {
            MEX_FCN_ERR ("cmd[%d]:op Invalid size.\n", cmd_code);
            break;
          }
        DBG_PRINTF ("cmd[%d]: rop = [%d:%d], op = [%d:%d]\n",
                    cmd_code, rop.start, rop.end, op.start, op.end);

        int (*fcn)(mpfr_t, const mpfr_t);
        if (cmd_code == 144)
          fcn = mpfr_ceil;
        else if (cmd_code == 145)
          fcn = mpfr_floor;
        else if (cmd_code == 146)
          fcn = mpfr_round;
        else if (cmd_code == 147)
          fcn = mpfr_roundeven;
        else if (cmd_code == 148)
          fcn = mpfr_trunc;
        else
          {
            MEX_FCN_ERR ("cmd[%d]: Bad operator.\n", cmd_code);
            break;
          }

        plhs[0] = mxCreateNumericMatrix (nlhs ? length (&rop): 1, 1,
                                         mxDOUBLE_CLASS, mxREAL);
        double* ret_ptr = mxGetPr (plhs[0]);
        size_t ret_stride = (nlhs) ? 1 : 0;

        for (size_t i = 0; i < length (&rop); i++)
          ret_ptr[i * ret_stride] = (double) fcn (&data[rop.start - 1 + i],
                                                  &data[op.start - 1 + i]);
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

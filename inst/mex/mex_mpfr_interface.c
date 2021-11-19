#include "mex_mpfr_interface.h"

#define MAX(a, b)                                  \
  ({ __typeof__(a)_a = (a); __typeof__(b)_b = (b); \
     _a > _b ? _a : _b; })


/**
 * Octave/Matlab MEX interface for MPFR.
 *
 * @param nlhs MEX parameter.
 * @param plhs MEX parameter.
 * @param nrhs MEX parameter.
 * @param prhs MEX parameter.
 * @param cmd_code code of command to execute (1000 - 1999).
 */

void
mex_mpfr_interface (int nlhs, mxArray *plhs[],
                    int nrhs, const mxArray *prhs[],
                    uint64_t cmd_code)
{
  #if (! defined(MPFR_VERSION) || (MPFR_VERSION < MPFR_VERSION_NUM (4, 0, 0)))
  # error "Oldest supported MPFR version is 4.0.0."
  #endif

  switch (cmd_code)
    {
      /**
       * Extra MPFR interface memory management functions.
       */

      case 1900: // size_t mpfr_t.get_data_capacity (void)
      {
        MEX_NARGINCHK (1);
        plhs[0] = mxCreateDoubleScalar ((double) mpfr_data_capacity);
        return;
      }

      case 1901: // size_t mpfr_t.get_data_size (void)
      {
        MEX_NARGINCHK (1);
        plhs[0] = mxCreateDoubleScalar ((double) mpfr_data_size);
        return;
      }

      case 1902: // idx_t mpfr_t.allocate (size_t count)
      {
        MEX_NARGINCHK (2);
        uint64_t count = 0;
        if (! extract_ui (1, nrhs, prhs, &count) || (count == 0))
          MEX_FCN_ERR ("cmd[%s]: Count must be a positive numeric scalar.\n",
                       "mpfr_t.allocate");

        DBG_PRINTF ("allocate '%d' new MPFR variables\n", (int) count);
        idx_t idx;
        if (! mex_mpfr_allocate ((size_t) count, &idx))
          MEX_FCN_ERR ("%s\n", "Memory allocation failed.");
        // Return start and end indices (1-based).
        plhs[0] = mxCreateNumericMatrix (2, 1, mxDOUBLE_CLASS, mxREAL);
        double *ptr = mxGetPr (plhs[0]);
        ptr[0] = (double) idx.start;
        ptr[1] = (double) idx.end;
        return;
      }

      case 1903: // void mpfr_t.mark_free (mpfr_t idx)
      {
        // Check if MPFR memory is (already) cleared.
        if ((mpfr_data_capacity <= 0) && (mpfr_data_size <= 0))
          return;

        MEX_NARGINCHK (2);
        MEX_MPFR_T (1, idx);
        DBG_PRINTF ("cmd[mpfr_t.mark_free]: [%d:%d] will be marked as free\n",
                    idx.start, idx.end);
        mex_mpfr_mark_free (&idx);
        return;
      }


      /**
       * Genuine MPFR interface functions.
       */

      case 1000:  // void mpfr_init2 (mpfr_t x, mpfr_prec_t prec)
      {
        MEX_NARGINCHK (3);
        MEX_MPFR_T (1, idx);
        MEX_MPFR_PREC_T (2, prec);
        DBG_PRINTF ("cmd[mpfr_init2]: [%d:%d] (prec = %d)\n",
                    idx.start, idx.end, (int) prec);
        for (size_t i = 0; i < length (&idx); i++)
          {
            mpfr_clear (&mpfr_data[(idx.start - 1) + i]);
            mpfr_init2 (&mpfr_data[(idx.start - 1) + i], prec);
          }
        return;
      }

      case 1218:  // void mpfr_init (mpfr_t x)
      {
        MEX_NARGINCHK (2);
        MEX_MPFR_T (1, idx);
        DBG_PRINTF ("cmd[mpfr_init]: [%d:%d]\n", idx.start, idx.end);
        for (size_t i = 0; i < length (&idx); i++)
          {
            mpfr_clear (&mpfr_data[(idx.start - 1) + i]);
            mpfr_init (&mpfr_data[(idx.start - 1) + i]);
          }
        return;
      }

      case 1001:  // void mpfr_set_default_prec (mpfr_prec_t prec)
      {
        MEX_NARGINCHK (2);
        MEX_MPFR_PREC_T (1, prec);
        mpfr_set_default_prec (prec);
        return;
      }

      case 1002:  // mpfr_prec_t mpfr_get_default_prec (void)
      {
        MEX_NARGINCHK (1);
        plhs[0] = mxCreateDoubleScalar ((double) mpfr_get_default_prec ());
        return;
      }

      case 1003:  // void mpfr_set_prec (mpfr_t x, mpfr_prec_t prec)
      {
        MEX_NARGINCHK (3);
        MEX_MPFR_T (1, idx);
        MEX_MPFR_PREC_T (2, prec);
        DBG_PRINTF ("cmd[mpfr_set_prec]: [%d:%d] (prec = %d)\n",
                    idx.start, idx.end, (int) prec);
        for (size_t i = 0; i < length (&idx); i++)
          mpfr_set_prec (&mpfr_data[(idx.start - 1) + i], prec);
        return;
      }

      case 1161:  // void mpfr_set_default_rounding_mode (mpfr_rnd_t rnd)
      {
        MEX_NARGINCHK (2);
        MEX_MPFR_RND_T (1, rnd);
        mpfr_set_default_rounding_mode (rnd);
        return;
      }

      case 1162:  // mpfr_rnd_t mpfr_get_default_rounding_mode (void)
      {
        MEX_NARGINCHK (1);
        plhs[0] = mxCreateDoubleScalar ((double)
                                        mpfr_get_default_rounding_mode ());
        return;
      }

      case 1181:  // const char * mpfr_get_version (void)
      {
        MEX_NARGINCHK (1);
        char *output_buf = (char *) mxCalloc (strlen (mpfr_get_version ()) + 1,
                                              sizeof(char));
        strcpy (output_buf, mpfr_get_version ());
        plhs[0] = mxCreateString (output_buf);
        return;
      }

      case 1182:  // int mpfr_buildopt_tls_p (void)
      {
        MEX_NARGINCHK (1);
        plhs[0] = mxCreateDoubleScalar ((double) mpfr_buildopt_tls_p ());
        return;
      }

      case 1183:  // int mpfr_buildopt_float128_p (void)
      {
        MEX_NARGINCHK (1);
        plhs[0] = mxCreateDoubleScalar ((double) mpfr_buildopt_float128_p ());
        return;
      }

      case 1184:  // int mpfr_buildopt_decimal_p (void)
      {
        MEX_NARGINCHK (1);
        plhs[0] = mxCreateDoubleScalar ((double) mpfr_buildopt_decimal_p ());
        return;
      }

      case 1185:  // int mpfr_buildopt_gmpinternals_p (void)
      {
        MEX_NARGINCHK (1);
        plhs[0] = mxCreateDoubleScalar ((double)
                                        mpfr_buildopt_gmpinternals_p ());
        return;
      }

      case 1186:  // int mpfr_buildopt_sharedcache_p (void)
      {
        MEX_NARGINCHK (1);
        plhs[0] = mxCreateDoubleScalar ((double)
                                        mpfr_buildopt_sharedcache_p ());
        return;
      }

      case 1187:  // const char * mpfr_buildopt_tune_case (void)
      {
        MEX_NARGINCHK (1);
        char *output_buf = (char *) mxCalloc (
          strlen (mpfr_buildopt_tune_case ()) + 1, sizeof(char));
        strcpy (output_buf, mpfr_buildopt_tune_case ());
        plhs[0] = mxCreateString (output_buf);
        return;
      }

      case 1188:  // mpfr_exp_t mpfr_get_emin (void)
      {
        MEX_NARGINCHK (1);
        plhs[0] = mxCreateDoubleScalar ((double) mpfr_get_emin ());
        return;
      }

      case 1189:  // mpfr_exp_t mpfr_get_emax (void)
      {
        MEX_NARGINCHK (1);
        plhs[0] = mxCreateDoubleScalar ((double) mpfr_get_emax ());
        return;
      }

      case 1192:  // mpfr_exp_t mpfr_get_emin_min
      {
        MEX_NARGINCHK (1);
        plhs[0] = mxCreateDoubleScalar ((double) mpfr_get_emin_min ());
        return;
      }

      case 1193:  // mpfr_exp_t mpfr_get_emin_max
      {
        MEX_NARGINCHK (1);
        plhs[0] = mxCreateDoubleScalar ((double) mpfr_get_emin_max ());
        return;
      }

      case 1194:  // mpfr_exp_t mpfr_get_emax_min
      {
        MEX_NARGINCHK (1);
        plhs[0] = mxCreateDoubleScalar ((double) mpfr_get_emax_min ());
        return;
      }

      case 1195:  // mpfr_exp_t mpfr_get_emax_max
      {
        MEX_NARGINCHK (1);
        plhs[0] = mxCreateDoubleScalar ((double) mpfr_get_emax_max ());
        return;
      }

      case 1198:  // void mpfr_clear_underflow (void)
      {
        MEX_NARGINCHK (1);
        mpfr_clear_underflow ();
        return;
      }

      case 1199:  // void mpfr_clear_overflow (void)
      {
        MEX_NARGINCHK (1);
        mpfr_clear_overflow ();
        return;
      }

      case 1200:  // void mpfr_clear_divby0 (void)
      {
        MEX_NARGINCHK (1);
        mpfr_clear_divby0 ();
        return;
      }

      case 1201:  // void mpfr_clear_nanflag (void)
      {
        MEX_NARGINCHK (1);
        mpfr_clear_nanflag ();
        return;
      }

      case 1202:  // void mpfr_clear_inexflag (void)
      {
        MEX_NARGINCHK (1);
        mpfr_clear_inexflag ();
        return;
      }

      case 1203:  // void mpfr_clear_erangeflag (void)
      {
        MEX_NARGINCHK (1);
        mpfr_clear_erangeflag ();
        return;
      }

      case 1204:  // void mpfr_clear_flags (void)
      {
        MEX_NARGINCHK (1);
        mpfr_clear_flags ();
        return;
      }

      case 1205:  // void mpfr_set_underflow (void)
      {
        MEX_NARGINCHK (1);
        mpfr_set_underflow ();
        return;
      }

      case 1206:  // void mpfr_set_overflow (void)
      {
        MEX_NARGINCHK (1);
        mpfr_set_overflow ();
        return;
      }

      case 1207:  // void mpfr_set_divby0 (void)
      {
        MEX_NARGINCHK (1);
        mpfr_set_divby0 ();
        return;
      }

      case 1208:  // void mpfr_set_nanflag (void)
      {
        MEX_NARGINCHK (1);
        mpfr_set_nanflag ();
        return;
      }

      case 1209:  // void mpfr_set_inexflag (void)
      {
        MEX_NARGINCHK (1);
        mpfr_set_inexflag ();
        return;
      }

      case 1210:  // void mpfr_set_erangeflag (void)
      {
        MEX_NARGINCHK (1);
        mpfr_set_erangeflag ();
        return;
      }

      case 1211:  // int mpfr_underflow_p (void)
      {
        MEX_NARGINCHK (1);
        plhs[0] = mxCreateDoubleScalar ((double) mpfr_underflow_p ());
        return;
      }

      case 1212:  // int mpfr_overflow_p (void)
      {
        MEX_NARGINCHK (1);
        plhs[0] = mxCreateDoubleScalar ((double) mpfr_overflow_p ());
        return;
      }

      case 1213:  // int mpfr_divby0_p (void)
      {
        MEX_NARGINCHK (1);
        plhs[0] = mxCreateDoubleScalar ((double) mpfr_divby0_p ());
        return;
      }

      case 1214:  // int mpfr_nanflag_p (void)
      {
        MEX_NARGINCHK (1);
        plhs[0] = mxCreateDoubleScalar ((double) mpfr_nanflag_p ());
        return;
      }

      case 1215:  // int mpfr_inexflag_p (void)
      {
        MEX_NARGINCHK (1);
        plhs[0] = mxCreateDoubleScalar ((double) mpfr_inexflag_p ());
        return;
      }

      case 1216:  // int mpfr_erangeflag_p (void)
      {
        MEX_NARGINCHK (1);
        plhs[0] = mxCreateDoubleScalar ((double) mpfr_erangeflag_p ());
        return;
      }

      case 1004:  // mpfr_prec_t mpfr_get_prec (mpfr_t x)
      case 1165:  // mpfr_prec_t mpfr_min_prec (mpfr_t x)
      {
        MEX_NARGINCHK (2);
        MEX_MPFR_T (1, idx);
        DBG_PRINTF ("cmd[%d]: [%d:%d]\n", cmd_code, idx.start, idx.end);
        plhs[0] = mxCreateNumericMatrix (length (&idx), 1, mxDOUBLE_CLASS,
                                         mxREAL);
        double *plhs_0_pr = mxGetPr (plhs[0]);

        mpfr_prec_t (*fcn)(const mpfr_t) = ((cmd_code == 1004) ? mpfr_get_prec
                                            : mpfr_min_prec);

        for (size_t i = 0; i < length (&idx); i++)
          plhs_0_pr[i] = (double) fcn (&mpfr_data[(idx.start - 1) + i]);
        return;
      }

      case 1069:  // int mpfr_nan_p (mpfr_t op)
      case 1070:  // int mpfr_inf_p (mpfr_t op)
      case 1071:  // int mpfr_number_p (mpfr_t op)
      case 1072:  // int mpfr_zero_p (mpfr_t op)
      case 1073:  // int mpfr_regular_p (mpfr_t op)
      case 1074:  // int mpfr_sgn (mpfr_t op)
      case 1160:  // int mpfr_integer_p (mpfr_t op)
      case 1178:  // int mpfr_signbit (mpfr_t op)
      {
        MEX_NARGINCHK (2);
        MEX_MPFR_T (1, idx);
        DBG_PRINTF ("cmd[%d]: [%d:%d]\n", cmd_code, idx.start, idx.end);
        plhs[0] = mxCreateNumericMatrix (length (&idx), 1, mxDOUBLE_CLASS,
                                         mxREAL);
        double *plhs_0_pr = mxGetPr (plhs[0]);

        int (*fcn)(const mpfr_t);
        if (cmd_code == 1069)
          fcn = mpfr_nan_p;
        else if (cmd_code == 1070)
          fcn = mpfr_inf_p;
        else if (cmd_code == 1071)
          fcn = mpfr_number_p;
        else if (cmd_code == 1072)
          fcn = mpfr_zero_p;
        else if (cmd_code == 1073)
          fcn = mpfr_regular_p;
        else if (cmd_code == 1074)
          fcn = mpfr_sgn;
        else if (cmd_code == 1160)
          fcn = mpfr_integer_p;
        else if (cmd_code == 1178)
          fcn = mpfr_signbit;
        else
          MEX_FCN_ERR ("cmd[%d]: Bad operator.\n", cmd_code);

        for (size_t i = 0; i < length (&idx); i++)
          plhs_0_pr[i] = (double) fcn (&mpfr_data[(idx.start - 1) + i]);
        return;
      }

      case 1010:  // void mpfr_set_nan (mpfr_t x)
      case 1167:  // void mpfr_nextabove (mpfr_t x)
      case 1168:  // void mpfr_nextbelow (mpfr_t x)
      {
        MEX_NARGINCHK (2);
        MEX_MPFR_T (1, idx);
        DBG_PRINTF ("cmd[%d]: [%d:%d]\n", cmd_code, idx.start, idx.end);

        void (*fcn)(mpfr_t);
        if (cmd_code == 1010)
          fcn = mpfr_set_nan;
        else if (cmd_code == 1167)
          fcn = mpfr_nextabove;
        else if (cmd_code == 1168)
          fcn = mpfr_nextbelow;
        else
          MEX_FCN_ERR ("cmd[%d]: Bad operator.\n", cmd_code);

        for (size_t i = 0; i < length (&idx); i++)
          fcn (&mpfr_data[(idx.start - 1) + i]);
        return;
      }

      case 1007:  // int mpfr_set_ui_2exp (mpfr_t rop, unsigned long int op, mpfr_exp_t e, mpfr_rnd_t rnd)
      case 1008:  // int mpfr_set_si_2exp (mpfr_t rop, long int op, mpfr_exp_t e, mpfr_rnd_t rnd)
      {
        MEX_NARGINCHK (5);
        MEX_MPFR_T (1, rop);
        size_t opM = mxGetM (prhs[2]);
        size_t opN = mxGetN (prhs[2]);
        if (! mxIsDouble (prhs[2])
            || (((opM * opN) != length (&rop)) && ((opM * opN) != 1)))
          MEX_FCN_ERR ("cmd[%d]:op must be a numerical vector of length 1"
                       "or %d.\n", cmd_code, length (&rop));
        size_t expM = mxGetM (prhs[3]);
        size_t expN = mxGetN (prhs[3]);
        if (! mxIsDouble (prhs[3])
            || (((expM * expN) != length (&rop)) && ((expM * expN) != 1)))
          MEX_FCN_ERR ("cmd[%d]:e must be a numerical vector of length 1"
                       "or %d.\n", cmd_code, length (&rop));
        MEX_MPFR_RND_T (4, rnd);
        DBG_PRINTF ("cmd[%d]: [%d:%d] (rnd: %d)\n", cmd_code, rop.start,
                    rop.end, (int) rnd);

        plhs[0] = mxCreateNumericMatrix (nlhs ? length (&rop) : 1, 1,
                                         mxDOUBLE_CLASS, mxREAL);
        double * ret_ptr    = mxGetPr (plhs[0]);
        mpfr_ptr rop_ptr    = &mpfr_data[rop.start - 1];
        double * op_ptr     = mxGetPr (prhs[2]);
        double * exp_ptr    = mxGetPr (prhs[3]);
        size_t   ret_stride = (nlhs) ? 1 : 0;
        size_t   op_stride  = ((opM * opN) == 1) ? 0 : 1;
        size_t   exp_stride = ((expM * expN) == 1) ? 0 : 1;
        if (cmd_code == 1007)
          for (size_t i = 0; i < length (&rop); i++)
            ret_ptr[i * ret_stride] = (double) mpfr_set_ui_2exp (
              rop_ptr + i, (unsigned long) op_ptr[i * op_stride],
              (mpfr_exp_t) exp_ptr[i * exp_stride], rnd);

        else
          for (size_t i = 0; i < length (&rop); i++)
            ret_ptr[i * ret_stride] = (double) mpfr_set_si_2exp (
              rop_ptr + i, (unsigned long) op_ptr[i * op_stride],
              (mpfr_exp_t) exp_ptr[i * exp_stride], rnd);

        return;
      }

      case 1009:  // int mpfr_set_str (mpfr_t rop, const char *s, int base, mpfr_rnd_t rnd)
      case 1016:  // int mpfr_init_set_str (mpfr_t x, const char *s, int base, mpfr_rnd_t rnd)
      case 1217:  // int mpfr_strtofr (mpfr_t rop, const char *nptr, char **endptr, int base, mpfr_rnd_t rnd)
      {
        // Note: In `mpfr_strtofr`, the parameter `char **endptr` is treated
        // as output variable and activated, if two output variables are
        // requested.
        MEX_NARGINCHK (5);
        MEX_MPFR_T (1, idx);
        size_t strM = mxGetM (prhs[2]);
        size_t strN = mxGetN (prhs[2]);
        if (! mxIsCell (prhs[2])
            || (((strM * strN) != length (&idx)) && ((strM * strN) != 1)))
          MEX_FCN_ERR ("cmd[%d]:s must be a cell vector of strings "
                       "of length 1 or %d\n.", cmd_code, length (&idx));
        size_t baseM = mxGetM (prhs[3]);
        size_t baseN = mxGetN (prhs[3]);
        if (! mxIsDouble (prhs[3])
            || (((baseM * baseN) != length (&idx)) && ((baseM * baseN) != 1)))
          MEX_FCN_ERR ("cmd[%d]:base must be a numeric vector "
                       "of length 1 or %d\n.", cmd_code, length (&idx));
        MEX_MPFR_RND_T (4, rnd);
        DBG_PRINTF ("cmd[%d]: [%d:%d] (rnd: %d)\n", cmd_code, idx.start,
                    idx.end, (int) rnd);

        plhs[0] = mxCreateNumericMatrix (nlhs ? length (&idx) : 1, 1,
                                         mxDOUBLE_CLASS, mxREAL);
        double *ret_ptr     = mxGetPr (plhs[0]);
        double *base_ptr    = mxGetPr (prhs[3]);
        size_t  ret_stride  = (nlhs) ? 1 : 0;
        size_t  str_stride  = ((strM * strN) == 1) ? 0 : 1;
        size_t  base_stride = ((baseM * baseN) == 1) ? 0 : 1;

        char *str = mxArrayToString (mxGetCell (prhs[2], 0));
        if (cmd_code == 1217)  // mpfr_strtofr
          {
            plhs[1] = mxCreateNumericMatrix ((nlhs == 2) ? length (&idx) : 1, 1,
                                             mxDOUBLE_CLASS, mxREAL);
            double *end_ptr        = mxGetPr (plhs[1]);
            size_t  end_ptr_stride = (nlhs == 2) ? 1 : 0;
            for (size_t i = 0; i < length (&idx); i++)
              {
                if (str_stride && (i > 0))
                  {
                    mxFree (str);
                    str = mxArrayToString (mxGetCell (prhs[2], i * str_stride));
                  }
                char *endptr = NULL;
                ret_ptr[i * ret_stride] = (double) mpfr_strtofr (
                  &mpfr_data[(idx.start - 1) + i], str, &endptr,
                  (int) base_ptr[i * base_stride], rnd);
                end_ptr[i * end_ptr_stride] = (endptr == NULL)
                                              ? -1.0
                                              : (double) (endptr - str + 1);
              }
          }
        else  // mpfr_set_str OR mpfr_init_set_str
          {
            int (*fcn)(mpfr_t, const char *, int, mpfr_rnd_t) =
              (cmd_code == 1009) ? mpfr_set_str : mpfr_init_set_str;

            for (size_t i = 0; i < length (&idx); i++)
              {
                if (str_stride && (i > 0))
                  {
                    mxFree (str);
                    str = mxArrayToString (mxGetCell (prhs[2], i * str_stride));
                  }
                if (cmd_code == 1009)
                  mpfr_clear (&mpfr_data[(idx.start - 1) + i]);
                ret_ptr[i * ret_stride] = (double) fcn (
                  &mpfr_data[(idx.start - 1) + i], str,
                  (int) base_ptr[i * base_stride], rnd);
              }
          }
        mxFree (str);
        return;
      }

      case 1011:  // void mpfr_set_inf (mpfr_t x, int sign)
      case 1012:  // void mpfr_set_zero (mpfr_t x, int sign)
      {
        MEX_NARGINCHK (3);
        MEX_MPFR_T (1, idx);
        size_t signM = mxGetM (prhs[2]);
        size_t signN = mxGetN (prhs[2]);
        if (! mxIsDouble (prhs[2])
            || (((signM * signN) != length (&idx)) && ((signM * signN) != 1)))
          MEX_FCN_ERR ("cmd[%d]:sign must be a numeric vector "
                       "of length 1 or %d\n.", cmd_code, length (&idx));
        DBG_PRINTF ("cmd[%d]: [%d:%d]\n", cmd_code, idx.start, idx.end);

        void (*fcn)(mpfr_t, int) = ((cmd_code == 1011) ? mpfr_set_inf
                                    : mpfr_set_zero);

        double *sign_ptr    = mxGetPr (prhs[2]);
        size_t  sign_stride = ((signM * signN) == 1) ? 0 : 1;

        for (size_t i = 0; i < length (&idx); i++)
          fcn (&mpfr_data[(idx.start - 1) + i],
               (int) sign_ptr[i * sign_stride]);
        return;
      }

      case 1013:  // void mpfr_swap (mpfr_t x, mpfr_t y)
      case 1166:  // void mpfr_nexttoward (mpfr_t x, mpfr_t y)
      {
        MEX_NARGINCHK (3);
        MEX_MPFR_T (1, x);
        MEX_MPFR_T (2, y);
        if (length (&x) != length (&y))
          MEX_FCN_ERR ("cmd[%d]:y Invalid size.\n", cmd_code);
        DBG_PRINTF ("cmd[%d]: x = [%d:%d], y = [%d:%d]\n",
                    cmd_code, x.start, x.end, y.start, y.end);

        if (cmd_code == 1013)
          for (size_t i = 0; i < length (&x); i++)
            mpfr_swap (&mpfr_data[x.start - 1 + i],
                       &mpfr_data[y.start - 1 + i]);
        else
          for (size_t i = 0; i < length (&x); i++)
            mpfr_nexttoward (&mpfr_data[x.start - 1 + i],
                             &mpfr_data[y.start - 1 + i]);
        return;
      }

      case 1014:  // int mpfr_init_set (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      {
        MEX_NARGINCHK (4);
        MEX_MPFR_T (1, rop);
        MEX_MPFR_T (2, op);
        if (length (&rop) != length (&op))
          MEX_FCN_ERR ("cmd[%d]:op Invalid size.\n", cmd_code);
        MEX_MPFR_RND_T (3, rnd);
        DBG_PRINTF ("cmd[%d]: rop = [%d:%d], op = [%d:%d] (rnd = %d)\n",
                    cmd_code, rop.start, rop.end, op.start, op.end, (int) rnd);

        plhs[0] = mxCreateNumericMatrix (nlhs ? length (&rop) : 1, 1,
                                         mxDOUBLE_CLASS, mxREAL);
        double *ret_ptr    = mxGetPr (plhs[0]);
        size_t  ret_stride = (nlhs) ? 1 : 0;

        for (size_t i = 0; i < length (&rop); i++)
          {
            mpfr_clear (&mpfr_data[(rop.start - 1) + i]);
            ret_ptr[i * ret_stride] = (double) mpfr_init_set (
              &mpfr_data[(rop.start - 1) + i], &mpfr_data[(op.start - 1) + i],
              rnd);
          }
        return;
      }

      case 1017:  // double mpfr_get_d (mpfr_t op, mpfr_rnd_t rnd)
      {
        MEX_NARGINCHK (3);
        MEX_MPFR_T (1, op);
        MEX_MPFR_RND_T (2, rnd);
        DBG_PRINTF ("cmd[mpfr_get_d]: [%d:%d] (rnd = %d)\n", op.start,
                    op.end, (int) rnd);

        plhs[0] = mxCreateNumericMatrix (length (&op), 1, mxDOUBLE_CLASS,
                                         mxREAL);
        double *ret_ptr = mxGetPr (plhs[0]);
        for (size_t i = 0; i < length (&op); i++)
          ret_ptr[i] =
            (double) mpfr_get_d (&mpfr_data[(op.start - 1) + i], rnd);
        return;
      }

      case 1018:  // double mpfr_get_d_2exp (long *exp, mpfr_t op, mpfr_rnd_t rnd)
      {
        // Note `long *exp` is return parameter.
        MEX_NARGINCHK (3);
        MEX_MPFR_T (1, op);
        MEX_MPFR_RND_T (2, rnd);
        DBG_PRINTF ("cmd[mpfr_get_d_2exp]: [%d:%d] (rnd = %d)\n", op.start,
                    op.end, (int) rnd);

        plhs[0] = mxCreateNumericMatrix (length (&op), 1, mxDOUBLE_CLASS,
                                         mxREAL);
        plhs[1] = mxCreateNumericMatrix (length (&op), 1, mxDOUBLE_CLASS,
                                         mxREAL);
        double *ret_ptr = mxGetPr (plhs[0]);
        double *exp_ptr = mxGetPr (plhs[1]);
        for (size_t i = 0; i < length (&op); i++)
          {
            long exp = 0;
            ret_ptr[i] = (double) mpfr_get_d_2exp (&exp,
                                                   &mpfr_data[(op.start - 1) + i],
                                                   rnd);
            exp_ptr[i] = (double) exp;
          }
        return;
      }

      case 1019:  // int mpfr_frexp (mpfr_exp_t *exp, mpfr_t y, mpfr_t x, mpfr_rnd_t rnd)
      {
        // Note `mpfr_exp_t *exp` is return parameter.
        MEX_NARGINCHK (4);
        MEX_MPFR_T (1, y);
        MEX_MPFR_T (2, x);
        if (length (&y) != length (&x))
          MEX_FCN_ERR ("cmd[%s]:x and y must have the same size.\n",
                       "mpfr_frexp");
        MEX_MPFR_RND_T (3, rnd);
        DBG_PRINTF ("cmd[mpfr_frexp]: [%d:%d] [%d:%d] (rnd = %d)\n",
                    y.start, y.end, x.start, x.end, (int) rnd);

        plhs[0] = mxCreateNumericMatrix (length (&y), 1, mxDOUBLE_CLASS,
                                         mxREAL);
        plhs[1] = mxCreateNumericMatrix (length (&y), 1, mxDOUBLE_CLASS,
                                         mxREAL);
        double *ret_ptr = mxGetPr (plhs[0]);
        double *exp_ptr = mxGetPr (plhs[1]);
        for (size_t i = 0; i < length (&y); i++)
          {
            mpfr_exp_t exp = 0;
            ret_ptr[i] = (double) mpfr_frexp (&exp,
                                              &mpfr_data[(y.start - 1) + i],
                                              &mpfr_data[(x.start - 1) + i],
                                              rnd);
            exp_ptr[i] = (double) exp;
          }
        return;
      }

      case 1020:  // size_t mpfr_get_str_ndigits (int b, mpfr_prec_t p)
      {
      #if (MPFR_VERSION < MPFR_VERSION_NUM (4, 1, 0))
        MEX_FCN_ERR ("cmd[mpfr_get_str_ndigits]: Not supported in MPFR %s.\n",
                     MPFR_VERSION_STRING);
      #else
        MEX_NARGINCHK (3);
        if (! mxIsDouble (prhs[1]) || ! mxIsDouble (prhs[2]))
          MEX_FCN_ERR ("cmd[%s]:b and p must be a numeric vectors\n.",
                       "mpfr_get_str_ndigits");
        size_t op1Dim = mxGetM (prhs[1]) * mxGetN (prhs[1]);
        size_t op2Dim = mxGetM (prhs[2]) * mxGetN (prhs[2]);
        if ((op1Dim != 1) && (op2Dim != 1) && (op1Dim != op2Dim))
          MEX_FCN_ERR ("cmd[%s]:b and p must be scalar or their dimensions "
                       "must agree\n.", "mpfr_get_str_ndigits");
        DBG_PRINTF ("cmd[mpfr_get_str_ndigits]: dim(b) = %d, dim(p) = %d\n",
                    op1Dim, op2Dim);

        plhs[0] = mxCreateNumericMatrix (MAX (op1Dim, op2Dim), 1,
                                         mxDOUBLE_CLASS, mxREAL);
        double *ret_ptr    = mxGetPr (plhs[0]);
        double *op1_ptr    = mxGetPr (prhs[1]);
        double *op2_ptr    = mxGetPr (prhs[2]);
        size_t  op1_stride = (op1Dim == 1) ? 0 : 1;
        size_t  op2_stride = (op2Dim == 1) ? 0 : 1;

        for (size_t i = 0; i < MAX (op1Dim, op2Dim); i++)
          ret_ptr[i] = (double) mpfr_get_str_ndigits (
            (int) op1_ptr[i * op1_stride],
            (mpfr_prec_t) op2_ptr[i * op2_stride]);
      #endif
        return;
      }

      case 1006:  // int mpfr_set_d (mpfr_t rop, double op, mpfr_rnd_t rnd)
      case 1015:  // int mpfr_init_set_d (mpfr_t rop, double op, mpfr_rnd_t rnd)
      {
        MEX_NARGINCHK (4);
        MEX_MPFR_T (1, rop);
        size_t opM = mxGetM (prhs[2]);
        size_t opN = mxGetN (prhs[2]);
        if (! mxIsDouble (prhs[2])
            || (((opM * opN) != length (&rop)) && ((opM * opN) != 1)))
          MEX_FCN_ERR ("cmd[%d]:op must be a numeric vector "
                       "of length 1 or %d\n.", cmd_code, length (&rop));
        MEX_MPFR_RND_T (3, rnd);
        DBG_PRINTF ("cmd[%d]: [%d:%d]\n", cmd_code, rop.start, rop.end);

        plhs[0] = mxCreateNumericMatrix (nlhs ? length (&rop) : 1, 1,
                                         mxDOUBLE_CLASS, mxREAL);
        double *ret_ptr    = mxGetPr (plhs[0]);
        double *op_pr      = mxGetPr (prhs[2]);
        size_t  ret_stride = (nlhs) ? 1 : 0;
        size_t  op_stride  = ((opM * opN) == 1) ? 0 : 1;

        if (cmd_code == 1006)
          for (size_t i = 0; i < length (&rop); i++)
            ret_ptr[i * ret_stride] = (double) mpfr_set_d (
              &mpfr_data[(rop.start - 1) + i], op_pr[i * op_stride], rnd);
        else
          for (size_t i = 0; i < length (&rop); i++)
            {
              mpfr_clear (&mpfr_data[(rop.start - 1) + i]);
              ret_ptr[i * ret_stride] = (double) mpfr_init_set_d (
                &mpfr_data[(rop.start - 1) + i], op_pr[i * op_stride], rnd);
            }
        return;
      }

      case 1021: // char * mpfr_get_str (char *str, mpfr_exp_t *expptr, int base, size_t n, mpfr_t op, mpfr_rnd_t rnd)
      {
        // Note: char *str, mpfr_exp_t *expptr are return parameters.
        MEX_NARGINCHK (5);
        MEX_MPFR_T (3, op);
        size_t baseM = mxGetM (prhs[1]);
        size_t baseN = mxGetN (prhs[1]);
        if (! mxIsDouble (prhs[1])
            || (((baseM * baseN) != length (&op)) && ((baseM * baseN) != 1)))
          MEX_FCN_ERR ("cmd[mpfr_get_str]:base must be a numeric vector "
                       "of length 1 or %d\n.", length (&op));
        size_t nSigM = mxGetM (prhs[2]);
        size_t nSigN = mxGetN (prhs[2]);
        if (! mxIsDouble (prhs[2])
            || (((nSigM * nSigN) != length (&op)) && ((nSigM * nSigN) != 1)))
          MEX_FCN_ERR ("cmd[mpfr_get_str]:n must be a numeric vector "
                       "of length 1 or %d\n.", length (&op));
        MEX_MPFR_RND_T (4, rnd);
        DBG_PRINTF ("cmd[mpfr_get_str]: [%d:%d]\n", op.start, op.end);

        plhs[0] = mxCreateCellMatrix (length (&op), 1);
        plhs[1] = mxCreateNumericMatrix (length (&op), 1, mxDOUBLE_CLASS,
                                         mxREAL);
        double *exp_ptr = mxGetPr (plhs[1]);

        double *base_ptr    = mxGetPr (prhs[1]);
        double *nSig_ptr    = mxGetPr (prhs[2]);
        size_t  base_stride = ((baseM * baseN) == 1) ? 0 : 1;
        size_t  nSig_stride = ((nSigM * nSigN) == 1) ? 0 : 1;

        for (size_t i = 0; i < length (&op); i++)
          {
            mpfr_exp_t expptr = 0;
            char *     str    = mpfr_get_str (NULL, &expptr,
                                              (int) base_ptr[i * base_stride],
                                              (size_t) nSig_ptr[i * nSig_stride],
                                              &mpfr_data[(op.start - 1) + i],
                                              rnd);
            if (str != NULL)
              {
                char *significant = (char *) mxCalloc (strlen (str) + 1,
                                                       sizeof(char));
                if (significant != NULL)
                  {
                    strcpy (significant, str);
                    mxSetCell (plhs[0], i, mxCreateString (significant));
                  }
                mpfr_free_str (str);
              }
            exp_ptr[i] = (double) expptr;
          }
        return;
      }

      case 1023:  // int mpfr_fits_ulong_p (mpfr_t op, mpfr_rnd_t rnd)
      case 1024:  // int mpfr_fits_slong_p (mpfr_t op, mpfr_rnd_t rnd)
      case 1025:  // int mpfr_fits_uint_p (mpfr_t op, mpfr_rnd_t rnd)
      case 1026:  // int mpfr_fits_sint_p (mpfr_t op, mpfr_rnd_t rnd)
      case 1027:  // int mpfr_fits_ushort_p (mpfr_t op, mpfr_rnd_t rnd)
      case 1028:  // int mpfr_fits_sshort_p (mpfr_t op, mpfr_rnd_t rnd)
      case 1029:  // int mpfr_fits_uintmax_p (mpfr_t op, mpfr_rnd_t rnd)
      case 1030:  // int mpfr_fits_intmax_p (mpfr_t op, mpfr_rnd_t rnd)
      {
        MEX_NARGINCHK (3);
        MEX_MPFR_T (1, op);
        MEX_MPFR_RND_T (2, rnd);
        DBG_PRINTF ("cmd[%d]: [%d:%d] (rnd = %d)\n", cmd_code, op.start,
                    op.end, (int) rnd);

        int (*fcn)(const mpfr_t, mpfr_rnd_t);
        if (cmd_code == 1023)
          fcn = mpfr_fits_ulong_p;
        else if (cmd_code == 1024)
          fcn = mpfr_fits_slong_p;
        else if (cmd_code == 1025)
          fcn = mpfr_fits_uint_p;
        else if (cmd_code == 1026)
          fcn = mpfr_fits_sint_p;
        else if (cmd_code == 1027)
          fcn = mpfr_fits_ushort_p;
        else if (cmd_code == 1028)
          fcn = mpfr_fits_sshort_p;
        else if (cmd_code == 1029)
          fcn = mpfr_fits_uintmax_p;
        else if (cmd_code == 1020)
          fcn = mpfr_fits_intmax_p;
        else
          MEX_FCN_ERR ("cmd[%d]: Bad operator.\n", cmd_code);

        plhs[0] = mxCreateNumericMatrix (length (&op), 1, mxDOUBLE_CLASS,
                                         mxREAL);
        double *ret_ptr = mxGetPr (plhs[0]);
        for (size_t i = 0; i < length (&op); i++)
          ret_ptr[i] = (double) fcn (&mpfr_data[(op.start - 1) + i], rnd);
        return;
      }

      case 1031:  // int mpfr_add (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_rnd_t rnd)
      case 1033:  // int mpfr_sub (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_rnd_t rnd)
      case 1036:  // int mpfr_mul (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_rnd_t rnd)
      case 1039:  // int mpfr_div (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_rnd_t rnd)
      case 1050:  // int mpfr_dim (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_rnd_t rnd)
      case 1060:  // int mpfr_hypot (mpfr_t rop, mpfr_t x, mpfr_t y, mpfr_rnd_t rnd)
      case 1092:  // int mpfr_pow (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_rnd_t rnd)
      case 1108:  // int mpfr_atan2 (mpfr_t rop, mpfr_t y, mpfr_t x, mpfr_rnd_t rnd)
      case 1122:  // int mpfr_gamma_inc (mpfr_t rop, mpfr_t op, mpfr_t op2, mpfr_rnd_t rnd)
      case 1126:  // int mpfr_beta (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_rnd_t rnd)
      case 1137:  // int mpfr_agm (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_rnd_t rnd)
      case 1156:  // int mpfr_fmod (mpfr_t r, mpfr_t x, mpfr_t y, mpfr_rnd_t rnd)
      case 1158:  // int mpfr_remainder (mpfr_t r, mpfr_t x, mpfr_t y, mpfr_rnd_t rnd)
      case 1169:  // int mpfr_min (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_rnd_t rnd)
      case 1170:  // int mpfr_max (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_rnd_t rnd)
      case 1180:  // int mpfr_copysign (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_rnd_t rnd)
      {
        MEX_NARGINCHK (5);
        MEX_MPFR_T (1, rop);
        MEX_MPFR_T (2, op1);
        MEX_MPFR_T (3, op2);
        if ((length (&rop) != length (&op1)) && (length (&op1) != 1))
          MEX_FCN_ERR ("cmd[%d]:op1 Invalid size.\n", cmd_code);
        if ((length (&rop) != length (&op2)) && (length (&op2) != 1))
          MEX_FCN_ERR ("cmd[%d]:op2 Invalid size.\n", cmd_code);
        MEX_MPFR_RND_T (4, rnd);
        DBG_PRINTF ("cmd[%d]: [%d:%d] = [%d:%d] + [%d:%d] (rnd = %d)\n",
                    cmd_code,
                    rop.start, rop.end,
                    op1.start, op1.end,
                    op2.start, op2.end, (int) rnd);

        int (*fcn) (mpfr_t, const mpfr_t, const mpfr_t, mpfr_rnd_t);
        if (cmd_code == 1031)
          fcn = mpfr_add;
        else if (cmd_code == 1033)
          fcn = mpfr_sub;
        else if (cmd_code == 1036)
          fcn = mpfr_mul;
        else if (cmd_code == 1039)
          fcn = mpfr_div;
        else if (cmd_code == 1050)
          fcn = mpfr_dim;
        else if (cmd_code == 1060)
          fcn = mpfr_hypot;
        else if (cmd_code == 1092)
          fcn = mpfr_pow;
        else if (cmd_code == 1108)
          fcn = mpfr_atan2;
        else if (cmd_code == 1122)
          fcn = mpfr_gamma_inc;
        else if (cmd_code == 1126)
          fcn = mpfr_beta;
        else if (cmd_code == 1137)
          fcn = mpfr_agm;
        else if (cmd_code == 1156)
          fcn = mpfr_fmod;
        else if (cmd_code == 1158)
          fcn = mpfr_remainder;
        else if (cmd_code == 1169)
          fcn = mpfr_min;
        else if (cmd_code == 1170)
          fcn = mpfr_max;
        else if (cmd_code == 1180)
          fcn = mpfr_copysign;
        else
          MEX_FCN_ERR ("cmd[%d]: Bad operator.\n", cmd_code);

        plhs[0] = mxCreateNumericMatrix (nlhs ? length (&rop) : 1, 1,
                                         mxDOUBLE_CLASS, mxREAL);
        double * ret_ptr    = mxGetPr (plhs[0]);
        mpfr_ptr rop_ptr    = &mpfr_data[rop.start - 1];
        mpfr_ptr op1_ptr    = &mpfr_data[op1.start - 1];
        mpfr_ptr op2_ptr    = &mpfr_data[op2.start - 1];
        size_t   ret_stride = (nlhs) ? 1 : 0;
        size_t   op1_stride = (length (&op1) == 1) ? 0 : 1;
        size_t   op2_stride = (length (&op2) == 1) ? 0 : 1;
        for (size_t i = 0; i < length (&rop); i++)
          ret_ptr[i * ret_stride] =
            (double) fcn (rop_ptr + i, op1_ptr + (i * op1_stride),
                          op2_ptr + (i * op2_stride), rnd);
        return;
      }

      case 1101:  // int mpfr_sin_cos (mpfr_t sop, mpfr_t cop, mpfr_t op, mpfr_rnd_t rnd)
      case 1112:  // int mpfr_sinh_cosh (mpfr_t sop, mpfr_t cop, mpfr_t op, mpfr_rnd_t rnd)
      case 1155:  // int mpfr_modf (mpfr_t iop, mpfr_t fop, mpfr_t op, mpfr_rnd_t rnd)
      {
        MEX_NARGINCHK (5);
        MEX_MPFR_T (1, sop);
        MEX_MPFR_T (2, cop);
        MEX_MPFR_T (3, op);
        if (length (&sop) != length (&cop))
          MEX_FCN_ERR ("cmd[%d]:sop and cop must have the same size.\n",
                       cmd_code);
        if (sop.start == cop.start)
          MEX_FCN_ERR ("cmd[%d]:sop and cop must be different variables.\n",
                       cmd_code);
        if ((length (&op) != length (&sop)) && (length (&op) != 1))
          MEX_FCN_ERR ("cmd[%d]:op Invalid size.\n", cmd_code);
        MEX_MPFR_RND_T (4, rnd);
        DBG_PRINTF ("cmd[%d]: sop = [%d:%d], cop = [%d:%d], op = [%d:%d] "
                    "(rnd = %d)\n", cmd_code, sop.start, sop.end,
                    cop.start, cop.end, op.start, op.end, (int) rnd);

        int (*fcn) (mpfr_t, mpfr_t, const mpfr_t, mpfr_rnd_t);
        if (cmd_code == 1101)
          fcn = mpfr_sin_cos;
        else if (cmd_code == 1112)
          fcn = mpfr_sinh_cosh;
        else if (cmd_code == 1155)
          fcn = mpfr_modf;
        else
          MEX_FCN_ERR ("cmd[%d]: Bad operator.\n", cmd_code);

        plhs[0] = mxCreateNumericMatrix (nlhs ? length (&sop) : 1, 1,
                                         mxDOUBLE_CLASS, mxREAL);
        double * ret_ptr    = mxGetPr (plhs[0]);
        mpfr_ptr sop_ptr    = &mpfr_data[sop.start - 1];
        mpfr_ptr cop_ptr    = &mpfr_data[cop.start - 1];
        mpfr_ptr op_ptr     = &mpfr_data[op.start - 1];
        size_t   ret_stride = (nlhs) ? 1 : 0;
        size_t   op_stride  = (length (&op) == 1) ? 0 : 1;
        for (size_t i = 0; i < length (&sop); i++)
          ret_ptr[i * ret_stride] = (double) fcn (sop_ptr + i, cop_ptr + i,
                                                  op_ptr + (i * op_stride),
                                                  rnd);
        return;
      }

      case 1032:  // int mpfr_add_d (mpfr_t rop, mpfr_t op1, double op2, mpfr_rnd_t rnd)
      case 1035:  // int mpfr_sub_d (mpfr_t rop, mpfr_t op1, double op2, mpfr_rnd_t rnd)
      case 1037:  // int mpfr_mul_d (mpfr_t rop, mpfr_t op1, double op2, mpfr_rnd_t rnd)
      case 1041:  // int mpfr_div_d (mpfr_t rop, mpfr_t op1, double op2, mpfr_rnd_t rnd)
      {
        MEX_NARGINCHK (5);
        MEX_MPFR_T (1, rop);
        MEX_MPFR_T (2, op1);
        if (length (&rop) != length (&op1))
          MEX_FCN_ERR ("cmd[%d]:op1 Invalid size.\n", cmd_code);
        size_t op2M = mxGetM (prhs[3]);
        size_t op2N = mxGetN (prhs[3]);
        if (! mxIsDouble (prhs[3])
            || (((op2M * op2N) != length (&rop)) && ((op2M * op2N) != 1)))
          MEX_FCN_ERR ("cmd[%d]:op2 Invalid.\n", cmd_code);
        MEX_MPFR_RND_T (4, rnd);
        DBG_PRINTF ("cmd[%d]: [%d:%d] = [%d:%d] + [%d:%d] (rnd = %d)\n",
                    cmd_code,
                    rop.start, rop.end,
                    op1.start, op1.end,
                    op2M, op2N, (int) rnd);

        int (*fcn) (mpfr_t, const mpfr_t, const double, mpfr_rnd_t);
        if (cmd_code == 1032)
          fcn = mpfr_add_d;
        else if (cmd_code == 1035)
          fcn = mpfr_sub_d;
        else if (cmd_code == 1037)
          fcn = mpfr_mul_d;
        else if (cmd_code == 1041)
          fcn = mpfr_div_d;
        else
          MEX_FCN_ERR ("cmd[%d]: Bad operator.\n", cmd_code);

        plhs[0] = mxCreateNumericMatrix (nlhs ? length (&rop) : 1, 1,
                                         mxDOUBLE_CLASS, mxREAL);
        double * ret_ptr    = mxGetPr (plhs[0]);
        mpfr_ptr rop_ptr    = &mpfr_data[rop.start - 1];
        mpfr_ptr op1_ptr    = &mpfr_data[op1.start - 1];
        double * op2_ptr    = mxGetPr (prhs[3]);
        size_t   ret_stride = (nlhs) ? 1 : 0;
        size_t   op2_stride = ((op2M * op2N) == 1) ? 0 : 1;
        for (size_t i = 0; i < length (&rop); i++)
          ret_ptr[i * ret_stride] =
            (double) fcn (rop_ptr + i, op1_ptr + i, op2_ptr[i * op2_stride],
                          rnd);
        return;
      }

      case 1034:  // int mpfr_d_sub (mpfr_t rop, double op1, mpfr_t op2, mpfr_rnd_t rnd)
      case 1040:  // int mpfr_d_div (mpfr_t rop, double op1, mpfr_t op2, mpfr_rnd_t rnd)
      case 1097:  // int mpfr_ui_pow (mpfr_t rop, unsigned long int op1, mpfr_t op2, mpfr_rnd_t rnd)
      case 1133:  // int mpfr_jn (mpfr_t rop, long n, mpfr_t op, mpfr_rnd_t rnd)
      case 1136:  // int mpfr_yn (mpfr_t rop, long n, mpfr_t op, mpfr_rnd_t rnd)
      {
        MEX_NARGINCHK (5);
        MEX_MPFR_T (1, rop);
        size_t op1M = mxGetM (prhs[2]);
        size_t op1N = mxGetN (prhs[2]);
        if (! mxIsDouble (prhs[2])
            || (((op1M * op1N) != length (&rop)) && ((op1M * op1N) != 1)))
          MEX_FCN_ERR ("cmd[%d]:op1 Invalid.\n", cmd_code);
        MEX_MPFR_T (3, op2);
        if ((length (&op2) != length (&rop)) && (length (&op2) != 1))
          MEX_FCN_ERR ("cmd[%d]:op2 Invalid size.\n", cmd_code);
        MEX_MPFR_RND_T (4, rnd);
        DBG_PRINTF ("cmd[%d]: rop = [%d:%d], op1 = [%d x %d], op2 = [%d:%d] "
                    "(rnd = %d)\n", cmd_code, rop.start, rop.end, op1M, op1N,
                    op2.start, op2.end, (int) rnd);

        plhs[0] = mxCreateNumericMatrix (nlhs ? length (&rop) : 1, 1,
                                         mxDOUBLE_CLASS, mxREAL);
        double * ret_ptr    = mxGetPr (plhs[0]);
        mpfr_ptr rop_ptr    = &mpfr_data[rop.start - 1];
        double * op1_ptr    = mxGetPr (prhs[2]);
        mpfr_ptr op2_ptr    = &mpfr_data[op2.start - 1];
        size_t   ret_stride = (nlhs) ? 1 : 0;
        size_t   op1_stride = ((op1M * op1N) == 1) ? 0 : 1;
        size_t   op2_stride = (length (&op2) == 1) ? 0 : 1;
        if (cmd_code == 1034)
          for (size_t i = 0; i < length (&rop); i++)
            ret_ptr[i * ret_stride] = (double) mpfr_d_sub (rop_ptr + i,
                                                           op1_ptr[i *
                                                                   op1_stride],
                                                           op2_ptr +
                                                           (i * op2_stride),
                                                           rnd);
        else if (cmd_code == 1040)
          for (size_t i = 0; i < length (&rop); i++)
            ret_ptr[i * ret_stride] = (double) mpfr_d_div (rop_ptr + i,
                                                           op1_ptr[i *
                                                                   op1_stride],
                                                           op2_ptr +
                                                           (i * op2_stride),
                                                           rnd);
        else if (cmd_code == 1097)
          for (size_t i = 0; i < length (&rop); i++)
            ret_ptr[i * ret_stride] = (double) mpfr_ui_pow (rop_ptr + i,
                                                            (unsigned long int) op1_ptr[
                                                              i * op1_stride],
                                                            op2_ptr +
                                                            (i * op2_stride),
                                                            rnd);
        else if (cmd_code == 1133)
          for (size_t i = 0; i < length (&rop); i++)
            ret_ptr[i * ret_stride] = (double) mpfr_jn (rop_ptr + i,
                                                        (long) op1_ptr[i *
                                                                       op1_stride],
                                                        op2_ptr +
                                                        (i * op2_stride), rnd);
        else if (cmd_code == 1136)
          for (size_t i = 0; i < length (&rop); i++)
            ret_ptr[i * ret_stride] = (double) mpfr_yn (rop_ptr + i,
                                                        (long) op1_ptr[i *
                                                                       op1_stride],
                                                        op2_ptr +
                                                        (i * op2_stride), rnd);
        else
          {
            mxFree (plhs[0]);
            MEX_FCN_ERR ("cmd[%d]: Bad operator.\n", cmd_code);
          }
        return;
      }

      case 1096:  // int mpfr_ui_pow_ui (mpfr_t rop, unsigned long int op1, unsigned long int op2, mpfr_rnd_t rnd)
      {
        MEX_NARGINCHK (5);
        MEX_MPFR_T (1, rop);
        size_t op1M = mxGetM (prhs[2]);
        size_t op1N = mxGetN (prhs[2]);
        if (! mxIsDouble (prhs[2])
            || (((op1M * op1N) != length (&rop)) && ((op1M * op1N) != 1)))
          MEX_FCN_ERR ("cmd[%d]:op1 Invalid.\n", cmd_code);
        size_t op2M = mxGetM (prhs[3]);
        size_t op2N = mxGetN (prhs[3]);
        if (! mxIsDouble (prhs[3])
            || (((op2M * op2N) != length (&rop)) && ((op2M * op2N) != 1)))
          MEX_FCN_ERR ("cmd[%d]:op2 Invalid.\n", cmd_code);
        MEX_MPFR_RND_T (4, rnd);
        DBG_PRINTF ("cmd[%d]: rop = [%d:%d], op1 = [%d x %d], op2 = [%d x %d] "
                    "(rnd = %d)\n", cmd_code, rop.start, rop.end, op1M, op1N,
                    op2M, op2N, (int) rnd);

        plhs[0] = mxCreateNumericMatrix (nlhs ? length (&rop) : 1, 1,
                                         mxDOUBLE_CLASS, mxREAL);
        double * ret_ptr    = mxGetPr (plhs[0]);
        mpfr_ptr rop_ptr    = &mpfr_data[rop.start - 1];
        double * op1_ptr    = mxGetPr (prhs[2]);
        double * op2_ptr    = mxGetPr (prhs[3]);
        size_t   ret_stride = (nlhs) ? 1 : 0;
        size_t   op1_stride = ((op1M * op1N) == 1) ? 0 : 1;
        size_t   op2_stride = ((op2M * op2N) == 1) ? 0 : 1;
        for (size_t i = 0; i < length (&rop); i++)
          ret_ptr[i * ret_stride] = (double) mpfr_ui_pow_ui (rop_ptr + i,
                                                             (unsigned long int) op1_ptr[
                                                               i * op1_stride],
                                                             (unsigned long int) op2_ptr[
                                                               i * op2_stride],
                                                             rnd);
        return;
      }

      case 1005:  // int mpfr_set (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 1038:  // int mpfr_sqr (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 1042:  // int mpfr_sqrt (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 1044:  // int mpfr_rec_sqrt (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 1045:  // int mpfr_cbrt (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 1048:  // int mpfr_neg (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 1049:  // int mpfr_abs (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 1083:  // int mpfr_log (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 1085:  // int mpfr_log2 (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 1086:  // int mpfr_log10 (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 1087:  // int mpfr_log1p (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 1088:  // int mpfr_exp (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 1089:  // int mpfr_exp2 (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 1090:  // int mpfr_exp10 (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 1091:  // int mpfr_expm1 (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 1098:  // int mpfr_cos (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 1099:  // int mpfr_sin (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 1100:  // int mpfr_tan (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 1102:  // int mpfr_sec (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 1103:  // int mpfr_csc (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 1104:  // int mpfr_cot (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 1105:  // int mpfr_acos (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 1106:  // int mpfr_asin (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 1107:  // int mpfr_atan (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 1109:  // int mpfr_cosh (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 1110:  // int mpfr_sinh (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 1111:  // int mpfr_tanh (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 1113:  // int mpfr_sech (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 1114:  // int mpfr_csch (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 1115:  // int mpfr_coth (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 1116:  // int mpfr_acosh (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 1117:  // int mpfr_asinh (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 1118:  // int mpfr_atanh (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 1119:  // int mpfr_eint (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 1120:  // int mpfr_li2 (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 1121:  // int mpfr_gamma (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 1123:  // int mpfr_lngamma (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 1125:  // int mpfr_digamma (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 1127:  // int mpfr_zeta (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 1129:  // int mpfr_erf (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 1130:  // int mpfr_erfc (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 1131:  // int mpfr_j0 (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 1132:  // int mpfr_j1 (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 1134:  // int mpfr_y0 (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 1135:  // int mpfr_y1 (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 1138:  // int mpfr_ai (mpfr_t rop, mpfr_t x, mpfr_rnd_t rnd)
      case 1143:  // int mpfr_rint (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 1149:  // int mpfr_rint_ceil (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 1150:  // int mpfr_rint_floor (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 1151:  // int mpfr_rint_round (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 1152:  // int mpfr_rint_roundeven (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 1153:  // int mpfr_rint_trunc (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      case 1154:  // int mpfr_frac (mpfr_t rop, mpfr_t op, mpfr_rnd_t rnd)
      {
        MEX_NARGINCHK (4);
        MEX_MPFR_T (1, rop);
        MEX_MPFR_T (2, op);
        if (length (&rop) != length (&op))
          MEX_FCN_ERR ("cmd[%d]:op Invalid size.\n", cmd_code);
        MEX_MPFR_RND_T (3, rnd);
        DBG_PRINTF ("cmd[%d]: rop = [%d:%d], op = [%d:%d] (rnd = %d)\n",
                    cmd_code, rop.start, rop.end, op.start, op.end, (int) rnd);

        int (*fcn) (mpfr_t, const mpfr_t, mpfr_rnd_t);
        if (cmd_code == 1005)
          fcn = mpfr_set;
        else if (cmd_code == 1038)
          fcn = mpfr_sqr;
        else if (cmd_code == 1042)
          fcn = mpfr_sqrt;
        else if (cmd_code == 1044)
          fcn = mpfr_rec_sqrt;
        else if (cmd_code == 1048)
          fcn = mpfr_neg;
        else if (cmd_code == 1049)
          fcn = mpfr_abs;
        else if (cmd_code == 1083)
          fcn = mpfr_log;
        else if (cmd_code == 1085)
          fcn = mpfr_log2;
        else if (cmd_code == 1086)
          fcn = mpfr_log10;
        else if (cmd_code == 1087)
          fcn = mpfr_log1p;
        else if (cmd_code == 1088)
          fcn = mpfr_exp;
        else if (cmd_code == 1089)
          fcn = mpfr_exp2;
        else if (cmd_code == 1090)
          fcn = mpfr_exp10;
        else if (cmd_code == 1091)
          fcn = mpfr_expm1;
        else if (cmd_code == 1098)
          fcn = mpfr_cos;
        else if (cmd_code == 1099)
          fcn = mpfr_sin;
        else if (cmd_code == 1100)
          fcn = mpfr_tan;
        else if (cmd_code == 1102)
          fcn = mpfr_sec;
        else if (cmd_code == 1103)
          fcn = mpfr_csc;
        else if (cmd_code == 1104)
          fcn = mpfr_cot;
        else if (cmd_code == 1105)
          fcn = mpfr_acos;
        else if (cmd_code == 1106)
          fcn = mpfr_asin;
        else if (cmd_code == 1107)
          fcn = mpfr_atan;
        else if (cmd_code == 1109)
          fcn = mpfr_cosh;
        else if (cmd_code == 1110)
          fcn = mpfr_sinh;
        else if (cmd_code == 1111)
          fcn = mpfr_tanh;
        else if (cmd_code == 1113)
          fcn = mpfr_sech;
        else if (cmd_code == 1114)
          fcn = mpfr_csch;
        else if (cmd_code == 1115)
          fcn = mpfr_coth;
        else if (cmd_code == 1116)
          fcn = mpfr_acosh;
        else if (cmd_code == 1117)
          fcn = mpfr_asinh;
        else if (cmd_code == 1118)
          fcn = mpfr_atanh;
        else if (cmd_code == 1119)
          fcn = mpfr_eint;
        else if (cmd_code == 1120)
          fcn = mpfr_li2;
        else if (cmd_code == 1121)
          fcn = mpfr_gamma;
        else if (cmd_code == 1123)
          fcn = mpfr_lngamma;
        else if (cmd_code == 1125)
          fcn = mpfr_digamma;
        else if (cmd_code == 1127)
          fcn = mpfr_zeta;
        else if (cmd_code == 1129)
          fcn = mpfr_erf;
        else if (cmd_code == 1130)
          fcn = mpfr_erfc;
        else if (cmd_code == 1131)
          fcn = mpfr_j0;
        else if (cmd_code == 1132)
          fcn = mpfr_j1;
        else if (cmd_code == 1134)
          fcn = mpfr_y0;
        else if (cmd_code == 1135)
          fcn = mpfr_y1;
        else if (cmd_code == 1138)
          fcn = mpfr_ai;
        else if (cmd_code == 1143)
          fcn = mpfr_rint;
        else if (cmd_code == 1149)
          fcn = mpfr_rint_ceil;
        else if (cmd_code == 1150)
          fcn = mpfr_rint_floor;
        else if (cmd_code == 1151)
          fcn = mpfr_rint_round;
        else if (cmd_code == 1152)
          fcn = mpfr_rint_roundeven;
        else if (cmd_code == 1153)
          fcn = mpfr_rint_trunc;
        else if (cmd_code == 1154)
          fcn = mpfr_frac;
        else
          MEX_FCN_ERR ("cmd[%d]: Bad operator.\n", cmd_code);

        plhs[0] = mxCreateNumericMatrix (nlhs ? length (&rop) : 1, 1,
                                         mxDOUBLE_CLASS, mxREAL);
        double * ret_ptr    = mxGetPr (plhs[0]);
        mpfr_ptr rop_ptr    = &mpfr_data[rop.start - 1];
        mpfr_ptr op_ptr     = &mpfr_data[op.start - 1];
        size_t   ret_stride = (nlhs) ? 1 : 0;
        for (size_t i = 0; i < length (&rop); i++)
          ret_ptr[i * ret_stride] = (double) fcn (rop_ptr + i, op_ptr + i, rnd);
        return;
      }

      case 1043:  // int mpfr_sqrt_ui (mpfr_t rop, unsigned long int op, mpfr_rnd_t rnd)
      case 1055:  // int mpfr_fac_ui (mpfr_t rop, unsigned long int op, mpfr_rnd_t rnd)
      case 1084:  // int mpfr_log_ui (mpfr_t rop, unsigned long op, mpfr_rnd_t rnd)
      case 1128:  // int mpfr_zeta_ui (mpfr_t rop, unsigned long op, mpfr_rnd_t rnd)
      {
        MEX_NARGINCHK (4);
        MEX_MPFR_T (1, rop);
        size_t opM = mxGetM (prhs[2]);
        size_t opN = mxGetN (prhs[2]);
        if (! mxIsDouble (prhs[2])
            || (((opM * opN) != length (&rop)) && ((opM * opN) != 1)))
          MEX_FCN_ERR ("cmd[%d]:op Invalid.\n", cmd_code);
        MEX_MPFR_RND_T (3, rnd);
        DBG_PRINTF ("cmd[%d]: rop = [%d:%d], op = [%d:%d] (rnd = %d)\n",
                    cmd_code, rop.start, rop.end, opM, opN, (int) rnd);

        int (*fcn)(mpfr_t, unsigned long int, mpfr_rnd_t);
        if (cmd_code == 1043)
          fcn = mpfr_sqrt_ui;
        else if (cmd_code == 1055)
          fcn = mpfr_fac_ui;
        else if (cmd_code == 1084)
          fcn = mpfr_log_ui;
        else if (cmd_code == 1128)
          fcn = mpfr_zeta_ui;
        else
          MEX_FCN_ERR ("cmd[%d]: Bad operator.\n", cmd_code);

        plhs[0] = mxCreateNumericMatrix (nlhs ? length (&rop) : 1, 1,
                                         mxDOUBLE_CLASS, mxREAL);
        double * ret_ptr    = mxGetPr (plhs[0]);
        mpfr_ptr rop_ptr    = &mpfr_data[rop.start - 1];
        double * op_ptr     = mxGetPr (prhs[2]);
        size_t   ret_stride = (nlhs) ? 1 : 0;
        size_t   op_stride  = ((opM * opN) == 1) ? 0 : 1;
        for (size_t i = 0; i < length (&rop); i++)
          ret_ptr[i * ret_stride] = (double) fcn (rop_ptr + i,
                                                  (unsigned long int) op_ptr[i *
                                                                             op_stride],
                                                  rnd);

        return;
      }

      case 1046:  // int mpfr_rootn_ui (mpfr_t rop, mpfr_t op, unsigned long int n, mpfr_rnd_t rnd)
      case 1051:  // int mpfr_mul_2ui (mpfr_t rop, mpfr_t op1, unsigned long int op2, mpfr_rnd_t rnd)
      case 1053:  // int mpfr_div_2ui (mpfr_t rop, mpfr_t op1, unsigned long int op2, mpfr_rnd_t rnd)
      case 1093:  // int mpfr_pow_ui (mpfr_t rop, mpfr_t op1, unsigned long int op2, mpfr_rnd_t rnd)
      {
        MEX_NARGINCHK (5);
        MEX_MPFR_T (1, rop);
        MEX_MPFR_T (2, op1);
        if ((length (&op1) != length (&rop)) && (length (&op1) != 1))
          MEX_FCN_ERR ("cmd[%d]:op1 Invalid.\n", cmd_code);
        size_t op2M = mxGetM (prhs[3]);
        size_t op2N = mxGetN (prhs[3]);
        if (! mxIsDouble (prhs[3])
            || (((op2M * op2N) != length (&rop)) && ((op2M * op2N) != 1)))
          MEX_FCN_ERR ("cmd[%d]:op2 Invalid.\n", cmd_code);
        MEX_MPFR_RND_T (4, rnd);
        DBG_PRINTF ("cmd[%d]: rop = [%d:%d], op1 = [%d:%d] , op2 = [%d x %d] "
                    "(rnd = %d)\n", cmd_code, rop.start, rop.end,
                    op1.start, op1.end, op2M, op2N, (int) rnd);

        int (*fcn)(mpfr_t, const mpfr_t, unsigned long int, mpfr_rnd_t);
        if (cmd_code == 1046)
          fcn = mpfr_rootn_ui;
        else if (cmd_code == 1051)
          fcn = mpfr_mul_2ui;
        else if (cmd_code == 1053)
          fcn = mpfr_div_2ui;
        else if (cmd_code == 1093)
          fcn = mpfr_pow_ui;
        else
          MEX_FCN_ERR ("cmd[%d]: Bad operator.\n", cmd_code);

        plhs[0] = mxCreateNumericMatrix (nlhs ? length (&rop) : 1, 1,
                                         mxDOUBLE_CLASS, mxREAL);
        double * ret_ptr    = mxGetPr (plhs[0]);
        mpfr_ptr rop_ptr    = &mpfr_data[rop.start - 1];
        mpfr_ptr op1_ptr    = &mpfr_data[op1.start - 1];
        double * op2_ptr    = mxGetPr (prhs[3]);
        size_t   ret_stride = (nlhs) ? 1 : 0;
        size_t   op1_stride = (length (&op1) == 1) ? 0 : 1;
        size_t   op2_stride = ((op2M * op2N) == 1) ? 0 : 1;
        for (size_t i = 0; i < length (&rop); i++)
          ret_ptr[i * ret_stride] = (double) fcn (rop_ptr + i,
                                                  op1_ptr + (i * op1_stride),
                                                  (unsigned long int) op2_ptr[i
                                                                              *
                                                                              op2_stride],
                                                  rnd);

        return;
      }

      case 1052:  // int mpfr_mul_2si (mpfr_t rop, mpfr_t op1, long int op2, mpfr_rnd_t rnd)
      case 1054:  // int mpfr_div_2si (mpfr_t rop, mpfr_t op1, long int op2, mpfr_rnd_t rnd)
      case 1094:  // int mpfr_pow_si (mpfr_t rop, mpfr_t op1, long int op2, mpfr_rnd_t rnd)
      {
        MEX_NARGINCHK (5);
        MEX_MPFR_T (1, rop);
        MEX_MPFR_T (2, op1);
        if ((length (&op1) != length (&rop)) && (length (&op1) != 1))
          MEX_FCN_ERR ("cmd[%d]:op1 Invalid.\n", cmd_code);
        size_t op2M = mxGetM (prhs[3]);
        size_t op2N = mxGetN (prhs[3]);
        if (! mxIsDouble (prhs[3])
            || (((op2M * op2N) != length (&rop)) && ((op2M * op2N) != 1)))
          MEX_FCN_ERR ("cmd[%d]:op2 Invalid.\n", cmd_code);
        MEX_MPFR_RND_T (4, rnd);
        DBG_PRINTF ("cmd[%d]: rop = [%d:%d], op1 = [%d:%d] , op2 = [%d x %d] "
                    "(rnd = %d)\n", cmd_code, rop.start, rop.end,
                    op1.start, op1.end, op2M, op2N, (int) rnd);

        int (*fcn)(mpfr_t, const mpfr_t, long int, mpfr_rnd_t);
        if (cmd_code == 1052)
          fcn = mpfr_mul_2si;
        else if (cmd_code == 1054)
          fcn = mpfr_div_2si;
        else if (cmd_code == 1094)
          fcn = mpfr_pow_si;
        else
          MEX_FCN_ERR ("cmd[%d]: Bad operator.\n", cmd_code);

        plhs[0] = mxCreateNumericMatrix (nlhs ? length (&rop) : 1, 1,
                                         mxDOUBLE_CLASS, mxREAL);
        double * ret_ptr    = mxGetPr (plhs[0]);
        mpfr_ptr rop_ptr    = &mpfr_data[rop.start - 1];
        mpfr_ptr op1_ptr    = &mpfr_data[op1.start - 1];
        double * op2_ptr    = mxGetPr (prhs[3]);
        size_t   ret_stride = (nlhs) ? 1 : 0;
        size_t   op1_stride = (length (&op1) == 1) ? 0 : 1;
        size_t   op2_stride = ((op2M * op2N) == 1) ? 0 : 1;
        for (size_t i = 0; i < length (&rop); i++)
          ret_ptr[i * ret_stride] = (double) fcn (rop_ptr + i,
                                                  op1_ptr + (i * op1_stride),
                                                  (long int) op2_ptr[i *
                                                                     op2_stride],
                                                  rnd);

        return;
      }

      case 1047:  // int mpfr_root (mpfr_t rop, mpfr_t op, unsigned long int n, mpfr_rnd_t rnd)
      {
        //TODO: deprecated
        mexErrMsgIdAndTxt ("mpfr_interface:mpfr_root",
                           "mpfr_root is deprecated and might be removed in a "
                           "future version of MPFR.", cmd_code);
        MEX_NARGINCHK (5);
        MEX_MPFR_T (1, rop);
        MEX_MPFR_T (2, op1);
        if ((length (&op1) != length (&rop)) && (length (&op1) != 1))
          MEX_FCN_ERR ("cmd[%d]:op1 Invalid.\n", cmd_code);
        size_t op2M = mxGetM (prhs[3]);
        size_t op2N = mxGetN (prhs[3]);
        if (! mxIsDouble (prhs[3])
            || (((op2M * op2N) != length (&rop)) && ((op2M * op2N) != 1)))
          MEX_FCN_ERR ("cmd[%d]:op2 Invalid.\n", cmd_code);
        MEX_MPFR_RND_T (4, rnd);
        DBG_PRINTF ("cmd[%d]: rop = [%d:%d], op1 = [%d:%d] , op2 = [%d x %d] "
                    "(rnd = %d)\n", cmd_code, rop.start, rop.end,
                    op1.start, op1.end, op2M, op2N, (int) rnd);

        plhs[0] = mxCreateNumericMatrix (nlhs ? length (&rop) : 1, 1,
                                         mxDOUBLE_CLASS, mxREAL);
        double * ret_ptr    = mxGetPr (plhs[0]);
        mpfr_ptr rop_ptr    = &mpfr_data[rop.start - 1];
        mpfr_ptr op1_ptr    = &mpfr_data[op1.start - 1];
        double * op2_ptr    = mxGetPr (prhs[3]);
        size_t   ret_stride = (nlhs) ? 1 : 0;
        size_t   op1_stride = (length (&op1) == 1) ? 0 : 1;
        size_t   op2_stride = ((op2M * op2N) == 1) ? 0 : 1;
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wdeprecated-declarations"
        for (size_t i = 0; i < length (&rop); i++)
          ret_ptr[i * ret_stride] = (double) mpfr_root (rop_ptr + i,
                                                        op1_ptr +
                                                        (i * op1_stride),
                                                        (unsigned long int) op2_ptr[
                                                          i * op2_stride],
                                                        rnd);
        #pragma GCC diagnostic pop
        return;
      }

      case 1056:  // int mpfr_fma (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_t op3, mpfr_rnd_t rnd)
      case 1057:  // int mpfr_fms (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_t op3, mpfr_rnd_t rnd)
      {
        MEX_NARGINCHK (6);
        MEX_MPFR_T (1, rop);
        MEX_MPFR_T (2, op1);
        if ((length (&op1) != length (&rop)) && (length (&op1) != 1))
          MEX_FCN_ERR ("cmd[%d]:op1 Invalid.\n", cmd_code);
        MEX_MPFR_T (3, op2);
        if ((length (&op2) != length (&rop)) && (length (&op2) != 1))
          MEX_FCN_ERR ("cmd[%d]:op2 Invalid.\n", cmd_code);
        MEX_MPFR_T (4, op3);
        if ((length (&op3) != length (&rop)) && (length (&op3) != 1))
          MEX_FCN_ERR ("cmd[%d]:op3 Invalid.\n", cmd_code);
        MEX_MPFR_RND_T (5, rnd);
        DBG_PRINTF ("cmd[%d]: rop = [%d:%d], op1 = [%d:%d], op2 = [%d:%d], "
                    "op3 = [%d:%d] (rnd = %d)\n", cmd_code, rop.start, rop.end,
                    op1.start, op1.end, op2.start, op2.end, op3.start, op3.end,
                    (int) rnd);

        int (*fcn)(mpfr_t, const mpfr_t, const mpfr_t,
                   const mpfr_t, mpfr_rnd_t) = ((cmd_code == 1056) ? mpfr_fma
                                                : mpfr_fms);

        plhs[0] = mxCreateNumericMatrix (nlhs ? length (&rop) : 1, 1,
                                         mxDOUBLE_CLASS, mxREAL);
        double * ret_ptr    = mxGetPr (plhs[0]);
        mpfr_ptr rop_ptr    = &mpfr_data[rop.start - 1];
        mpfr_ptr op1_ptr    = &mpfr_data[op1.start - 1];
        mpfr_ptr op2_ptr    = &mpfr_data[op2.start - 1];
        mpfr_ptr op3_ptr    = &mpfr_data[op3.start - 1];
        size_t   ret_stride = (nlhs) ? 1 : 0;
        size_t   op1_stride = (length (&op1) == 1) ? 0 : 1;
        size_t   op2_stride = (length (&op2) == 1) ? 0 : 1;
        size_t   op3_stride = (length (&op3) == 1) ? 0 : 1;
        for (size_t i = 0; i < length (&rop); i++)
          ret_ptr[i * ret_stride] = (double) fcn (rop_ptr + i,
                                                  op1_ptr + (i * op1_stride),
                                                  op2_ptr + (i * op2_stride),
                                                  op3_ptr + (i * op3_stride),
                                                  rnd);
        return;
      }

      case 1058:  // int mpfr_fmma (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_t op3, mpfr_t op4, mpfr_rnd_t rnd)
      case 1059:  // int mpfr_fmms (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_t op3, mpfr_t op4, mpfr_rnd_t rnd)
      {
        MEX_NARGINCHK (6);
        MEX_MPFR_T (1, rop);
        MEX_MPFR_T (2, op1);
        if ((length (&op1) != length (&rop)) && (length (&op1) != 1))
          MEX_FCN_ERR ("cmd[%d]:op1 Invalid.\n", cmd_code);
        MEX_MPFR_T (3, op2);
        if ((length (&op2) != length (&rop)) && (length (&op2) != 1))
          MEX_FCN_ERR ("cmd[%d]:op2 Invalid.\n", cmd_code);
        MEX_MPFR_T (4, op3);
        if ((length (&op3) != length (&rop)) && (length (&op3) != 1))
          MEX_FCN_ERR ("cmd[%d]:op3 Invalid.\n", cmd_code);
        MEX_MPFR_T (5, op4);
        if ((length (&op4) != length (&rop)) && (length (&op4) != 1))
          MEX_FCN_ERR ("cmd[%d]:op4 Invalid.\n", cmd_code);
        MEX_MPFR_RND_T (6, rnd);
        DBG_PRINTF ("cmd[%d]: rop = [%d:%d], op1 = [%d:%d], op2 = [%d:%d], "
                    "op3 = [%d:%d], op4 = [%d:%d] (rnd = %d)\n", cmd_code,
                    rop.start, rop.end, op1.start, op1.end, op2.start, op2.end,
                    op3.start, op3.end, op4.start, op4.end, (int) rnd);

        int (*fcn)(mpfr_t, const mpfr_t, const mpfr_t, const mpfr_t,
                   const mpfr_t, mpfr_rnd_t) = ((cmd_code == 1058) ? mpfr_fmma
                                                : mpfr_fmms);

        plhs[0] = mxCreateNumericMatrix (nlhs ? length (&rop) : 1, 1,
                                         mxDOUBLE_CLASS, mxREAL);
        double * ret_ptr    = mxGetPr (plhs[0]);
        mpfr_ptr rop_ptr    = &mpfr_data[rop.start - 1];
        mpfr_ptr op1_ptr    = &mpfr_data[op1.start - 1];
        mpfr_ptr op2_ptr    = &mpfr_data[op2.start - 1];
        mpfr_ptr op3_ptr    = &mpfr_data[op3.start - 1];
        mpfr_ptr op4_ptr    = &mpfr_data[op4.start - 1];
        size_t   ret_stride = (nlhs) ? 1 : 0;
        size_t   op1_stride = (length (&op1) == 1) ? 0 : 1;
        size_t   op2_stride = (length (&op2) == 1) ? 0 : 1;
        size_t   op3_stride = (length (&op3) == 1) ? 0 : 1;
        size_t   op4_stride = (length (&op4) == 1) ? 0 : 1;
        for (size_t i = 0; i < length (&rop); i++)
          ret_ptr[i * ret_stride] = (double) fcn (rop_ptr + i,
                                                  op1_ptr + (i * op1_stride),
                                                  op2_ptr + (i * op2_stride),
                                                  op3_ptr + (i * op3_stride),
                                                  op4_ptr + (i * op4_stride),
                                                  rnd);
        return;
      }

      case 1061:  // int mpfr_sum (mpfr_t rop, const mpfr_ptr tab[], unsigned long int n, mpfr_rnd_t rnd)
      {
        MEX_NARGINCHK (5);
        MEX_MPFR_T (1, rop);
        if (length (&rop) != 1)
          MEX_FCN_ERR ("cmd[%d]:rop must be a scalar MPFR variable.\n",
                       cmd_code);
        MEX_MPFR_T (2, tab);
        uint64_t n = 0;
        if (! extract_ui (3, nrhs, prhs, &n))
          MEX_FCN_ERR ("cmd[%d]:n must be a non-negative numeric scalar.\n",
                       cmd_code);
        if (length (&tab) < n)
          MEX_FCN_ERR ("cmd[%d]:tab must be a MPFR vector of at least ",
                       "length %d.\n", cmd_code, (int) n);
        MEX_MPFR_RND_T (4, rnd);
        DBG_PRINTF ("cmd[%d]: rop = [%d:%d], tab = [%d:%d], n = %d "
                    "(rnd = %d)\n", cmd_code, rop.start, rop.end,
                    tab.start, tab.end, (int) n, (int) rnd);

        plhs[0] = mxCreateNumericMatrix (1, 1, mxDOUBLE_CLASS, mxREAL);
        double * ret_ptr = mxGetPr (plhs[0]);
        mpfr_ptr rop_ptr = &mpfr_data[rop.start - 1];

        mpfr_ptr *tab_ptr = (mpfr_ptr *) mxMalloc (n * sizeof(mpfr_ptr *));
        for (size_t i = 0; i < n; i++)
          tab_ptr[i] = mpfr_data + tab.start - 1 + i;

        *ret_ptr = (double) mpfr_sum (rop_ptr, tab_ptr, n, rnd);
        mxFree (tab_ptr);
        return;
      }

      case 1062:  // int mpfr_dot (mpfr_t rop, const mpfr_ptr a[], const mpfr_ptr b[], unsigned long int n, mpfr_rnd_t rnd)
      {
        #if (MPFR_VERSION < MPFR_VERSION_NUM (4, 1, 0))
        MEX_FCN_ERR ("cmd[%d]: Not supported in MPFR %s.\n",
                     cmd_code, MPFR_VERSION_STRING);
        #else
        MEX_NARGINCHK (6);
        MEX_MPFR_T (1, rop);
        if (length (&rop) != 1)
          MEX_FCN_ERR ("cmd[%d]:rop must be a scalar MPFR variable.\n",
                       cmd_code);
        MEX_MPFR_T (2, a);
        MEX_MPFR_T (3, b);
        uint64_t n = 0;
        if (! extract_ui (4, nrhs, prhs, &n))
          MEX_FCN_ERR ("cmd[%d]:n must be a non-negative numeric scalar.\n",
                       cmd_code);
        if ((length (&a) < n) || (length (&b) < n))
          MEX_FCN_ERR ("cmd[%d]:a and b must be MPFR vectors of at least ",
                       "length %d.\n", cmd_code, (int) n);
        MEX_MPFR_RND_T (5, rnd);
        DBG_PRINTF ("cmd[%d]: rop = [%d:%d], a = [%d:%d], b = [%d:%d], "
                    "n = %d (rnd = %d)\n", cmd_code, rop.start, rop.end,
                    a.start, a.end, b.start, b.end, (int) n, (int) rnd);

        plhs[0] = mxCreateNumericMatrix (1, 1, mxDOUBLE_CLASS, mxREAL);
        double * ret_ptr = mxGetPr (plhs[0]);
        mpfr_ptr rop_ptr = &mpfr_data[rop.start - 1];

        mpfr_ptr *a_ptr = (mpfr_ptr *) mxMalloc (n * sizeof(mpfr_ptr *));
        mpfr_ptr *b_ptr = (mpfr_ptr *) mxMalloc (n * sizeof(mpfr_ptr *));
        for (size_t i = 0; i < n; i++)
          {
            a_ptr[i] = mpfr_data + a.start - 1 + i;
            b_ptr[i] = mpfr_data + b.start - 1 + i;
          }

        *ret_ptr = (double) mpfr_dot (rop_ptr, a_ptr, b_ptr, n, rnd);
        mxFree (a_ptr);
        mxFree (b_ptr);
        #endif
        return;
      }

      case 1063:  // int mpfr_cmp (mpfr_t op1, mpfr_t op2)
      case 1067:  // int mpfr_cmpabs (mpfr_t op1, mpfr_t op2)
      case 1075:  // int mpfr_greater_p (mpfr_t op1, mpfr_t op2)
      case 1076:  // int mpfr_greaterequal_p (mpfr_t op1, mpfr_t op2)
      case 1077:  // int mpfr_less_p (mpfr_t op1, mpfr_t op2)
      case 1078:  // int mpfr_lessequal_p (mpfr_t op1, mpfr_t op2)
      case 1079:  // int mpfr_equal_p (mpfr_t op1, mpfr_t op2)
      case 1080:  // int mpfr_lessgreater_p (mpfr_t op1, mpfr_t op2)
      case 1081:  // int mpfr_unordered_p (mpfr_t op1, mpfr_t op2)
      case 1082:  // int mpfr_total_order_p (mpfr_t x, mpfr_t y)
      {
        MEX_NARGINCHK (3);
        MEX_MPFR_T (1, op1);
        MEX_MPFR_T (2, op2);
        if ((length (&op1) != length (&op2)) && (length (&op1) != 1)
            && (length (&op2) != 1))
          MEX_FCN_ERR ("cmd[%d]:op2 Invalid size.\n", cmd_code);
        DBG_PRINTF ("cmd[%d]: op1 = [%d:%d], op2 = [%d:%d]\n",
                    cmd_code, op1.start, op1.end, op2.start, op2.end);

        int (*fcn) (const mpfr_t, const mpfr_t);
        if (cmd_code == 1063)
          fcn = mpfr_cmp;
        else if (cmd_code == 1067)
          fcn = mpfr_cmpabs;
        else if (cmd_code == 1075)
          fcn = mpfr_greater_p;
        else if (cmd_code == 1076)
          fcn = mpfr_greaterequal_p;
        else if (cmd_code == 1077)
          fcn = mpfr_less_p;
        else if (cmd_code == 1078)
          fcn = mpfr_lessequal_p;
        else if (cmd_code == 1079)
          fcn = mpfr_equal_p;
        else if (cmd_code == 1080)
          fcn = mpfr_lessgreater_p;
        else if (cmd_code == 1081)
          fcn = mpfr_unordered_p;
        else if (cmd_code == 1082)
          {
            #if (MPFR_VERSION < MPFR_VERSION_NUM (4, 1, 0))
            MEX_FCN_ERR ("cmd[%d]: Not supported in MPFR %s.\n",
                         cmd_code, MPFR_VERSION_STRING);
            #else
            fcn = mpfr_total_order_p;
            #endif
          }
        else
          MEX_FCN_ERR ("cmd[%d]: Bad operator.\n", cmd_code);

        plhs[0] = mxCreateNumericMatrix (nlhs
                                         ? MAX (length (&op1), length (&op2))
                                         : 1,
                                         1, mxDOUBLE_CLASS, mxREAL);
        double * ret_ptr    = mxGetPr (plhs[0]);
        mpfr_ptr op1_ptr    = &mpfr_data[op1.start - 1];
        mpfr_ptr op2_ptr    = &mpfr_data[op2.start - 1];
        size_t   ret_stride = (nlhs) ? 1 : 0;
        size_t   op1_stride = (length (&op1) == 1) ? 0 : 1;
        size_t   op2_stride = (length (&op2) == 1) ? 0 : 1;
        for (size_t i = 0; i < MAX (length (&op1), length (&op2)); i++)
          ret_ptr[i * ret_stride] = (double) fcn (op1_ptr + (i * op1_stride),
                                                  op2_ptr + (i * op2_stride));
        return;
      }

      case 1064:  // int mpfr_cmp_d (mpfr_t op1, double op2)
      case 1068:  // int mpfr_cmpabs_ui (mpfr_t op1, unsigned long op2)
      {
        MEX_NARGINCHK (3);
        MEX_MPFR_T (1, op1);
        size_t op2M = mxGetM (prhs[2]);
        size_t op2N = mxGetN (prhs[2]);
        if (! mxIsDouble (prhs[2])
            || (((op2M * op2N) != length (&op1)) && ((op2M * op2N) != 1)))
          MEX_FCN_ERR ("cmd[%d]:op2 Invalid.\n", cmd_code);
        DBG_PRINTF ("cmd[%d]: op1 = [%d:%d] , op2 = [%d x %d]\n", cmd_code,
                    op1.start, op1.end, op2M, op2N);

        plhs[0] = mxCreateNumericMatrix (
          (nlhs ? MAX (length (&op1), (op2M * op2N)) : 1), 1, mxDOUBLE_CLASS,
          mxREAL);
        double * ret_ptr    = mxGetPr (plhs[0]);
        mpfr_ptr op1_ptr    = &mpfr_data[op1.start - 1];
        double * op2_ptr    = mxGetPr (prhs[2]);
        size_t   ret_stride = (nlhs) ? 1 : 0;
        size_t   op1_stride = (length (&op1) == 1) ? 0 : 1;
        size_t   op2_stride = ((op2M * op2N) == 1) ? 0 : 1;
        if (cmd_code == 1064)
          for (size_t i = 0; i < MAX (length (&op1), (op2M * op2N)); i++)
            ret_ptr[i * ret_stride] = (double) mpfr_cmp_d (
              op1_ptr + (i * op1_stride), op2_ptr[i * op2_stride]);
        else
          {
          #if (MPFR_VERSION < MPFR_VERSION_NUM (4, 1, 0))
            mxFree (plhs[0]);
            MEX_FCN_ERR ("cmd[%d]: Not supported in MPFR %s.\n",
                         cmd_code, MPFR_VERSION_STRING);
          #else
            for (size_t i = 0; i < MAX (length (&op1), (op2M * op2N)); i++)
              ret_ptr[i * ret_stride] = (double) mpfr_cmpabs_ui (
                op1_ptr + (i * op1_stride),
                (unsigned long) op2_ptr[i * op2_stride]);
          #endif
          }
        return;
      }

      case 1065:  // int mpfr_cmp_ui_2exp (mpfr_t op1, unsigned long int op2, mpfr_exp_t e)
      case 1066:  // int mpfr_cmp_si_2exp (mpfr_t op1, long int op2, mpfr_exp_t e)
      {
        MEX_NARGINCHK (4);
        MEX_MPFR_T (1, op1);
        size_t op2M = mxGetM (prhs[2]);
        size_t op2N = mxGetN (prhs[2]);
        if (! mxIsDouble (prhs[2])
            || (((op2M * op2N) != length (&op1)) && ((op2M * op2N) != 1)))
          MEX_FCN_ERR ("cmd[%d]:op2 Invalid.\n", cmd_code);
        MEX_MPFR_EXP_T (3, e);
        DBG_PRINTF ("cmd[%d]: op1 = [%d:%d] , op2 = [%d x %d]\n", cmd_code,
                    op1.start, op1.end, op2M, op2N);

        plhs[0] = mxCreateNumericMatrix (
          (nlhs ? MAX (length (&op1), (op2M * op2N)) : 1), 1, mxDOUBLE_CLASS,
          mxREAL);
        double * ret_ptr    = mxGetPr (plhs[0]);
        mpfr_ptr op1_ptr    = &mpfr_data[op1.start - 1];
        double * op2_ptr    = mxGetPr (prhs[2]);
        size_t   ret_stride = (nlhs) ? 1 : 0;
        size_t   op1_stride = (length (&op1) == 1) ? 0 : 1;
        size_t   op2_stride = ((op2M * op2N) == 1) ? 0 : 1;
        if (cmd_code == 1065)
          for (size_t i = 0; i < MAX (length (&op1), (op2M * op2N)); i++)
            ret_ptr[i * ret_stride] = (double) mpfr_cmp_ui_2exp (
              op1_ptr + (i * op1_stride),
              (unsigned long int) op2_ptr[i * op2_stride], e);
        else
          for (size_t i = 0; i < MAX (length (&op1), (op2M * op2N)); i++)
            ret_ptr[i * ret_stride] = (double) mpfr_cmp_si_2exp (
              op1_ptr + (i * op1_stride), (long int) op2_ptr[i * op2_stride],
              e);
        return;
      }

      case 1124:  // int mpfr_lgamma (mpfr_t rop, int *signp, mpfr_t op, mpfr_rnd_t rnd)
      {
        MEX_NARGINCHK (4);
        MEX_MPFR_T (1, rop);
        MEX_MPFR_T (2, op);
        if ((length (&op) != length (&rop)) && (length (&op) != 1))
          MEX_FCN_ERR ("cmd[%d]:op Invalid.\n", cmd_code);
        MEX_MPFR_RND_T (3, rnd);
        DBG_PRINTF ("cmd[%d]: rop = [%d:%d], op = [%d:%d] (rnd = %d)\n",
                    cmd_code, rop.start, rop.end, op.start, op.end, (int) rnd);

        plhs[0] = mxCreateNumericMatrix (length (&rop), 1, mxDOUBLE_CLASS,
                                         mxREAL);
        plhs[1] = mxCreateNumericMatrix (length (&rop), 1, mxDOUBLE_CLASS,
                                         mxREAL);
        double * ret_ptr   = mxGetPr (plhs[0]);
        double * signp_ptr = mxGetPr (plhs[1]);
        mpfr_ptr rop_ptr   = &mpfr_data[rop.start - 1];
        mpfr_ptr op_ptr    = &mpfr_data[op.start - 1];
        size_t   op_stride = (length (&op) == 1) ? 0 : 1;
        for (size_t i = 0; i < length (&rop); i++)
          {
            int signp = 0;
            ret_ptr[i] = (double) mpfr_lgamma (rop_ptr + i, &signp,
                                               op_ptr + (i * op_stride), rnd);
            signp_ptr[i] = (double) signp;
          }
        return;
      }

      case 1139:  // int mpfr_const_log2 (mpfr_t rop, mpfr_rnd_t rnd)
      case 1140:  // int mpfr_const_pi (mpfr_t rop, mpfr_rnd_t rnd)
      case 1141:  // int mpfr_const_euler (mpfr_t rop, mpfr_rnd_t rnd)
      case 1142:  // int mpfr_const_catalan (mpfr_t rop, mpfr_rnd_t rnd)
      {
        MEX_NARGINCHK (3);
        MEX_MPFR_T (1, rop);
        MEX_MPFR_RND_T (2, rnd);
        DBG_PRINTF ("cmd[%d]: rop = [%d:%d] (rnd = %d)\n",
                    cmd_code, rop.start, rop.end, (int) rnd);

        int (*fcn)(mpfr_t, mpfr_rnd_t);
        if (cmd_code == 1139)
          fcn = mpfr_const_log2;
        else if (cmd_code == 1140)
          fcn = mpfr_const_pi;
        else if (cmd_code == 1141)
          fcn = mpfr_const_euler;
        else if (cmd_code == 1142)
          fcn = mpfr_const_catalan;
        else
          MEX_FCN_ERR ("cmd[%d]: Bad operator.\n", cmd_code);

        plhs[0] = mxCreateNumericMatrix (nlhs ? length (&rop) : 1, 1,
                                         mxDOUBLE_CLASS, mxREAL);
        double *ret_ptr    = mxGetPr (plhs[0]);
        size_t  ret_stride = (nlhs) ? 1 : 0;

        for (size_t i = 0; i < length (&rop); i++)
          ret_ptr[i * ret_stride] = (double) fcn (&mpfr_data[rop.start - 1 + i],
                                                  rnd);
        return;
      }

      case 1144:  // int mpfr_ceil (mpfr_t rop, mpfr_t op)
      case 1145:  // int mpfr_floor (mpfr_t rop, mpfr_t op)
      case 1146:  // int mpfr_round (mpfr_t rop, mpfr_t op)
      case 1147:  // int mpfr_roundeven (mpfr_t rop, mpfr_t op)
      case 1148:  // int mpfr_trunc (mpfr_t rop, mpfr_t op)
      {
        MEX_NARGINCHK (3);
        MEX_MPFR_T (1, rop);
        MEX_MPFR_T (2, op);
        if (length (&rop) != length (&op))
          MEX_FCN_ERR ("cmd[%d]:op Invalid size.\n", cmd_code);
        DBG_PRINTF ("cmd[%d]: rop = [%d:%d], op = [%d:%d]\n",
                    cmd_code, rop.start, rop.end, op.start, op.end);

        int (*fcn)(mpfr_t, const mpfr_t);
        if (cmd_code == 1144)
          fcn = mpfr_ceil;
        else if (cmd_code == 1145)
          fcn = mpfr_floor;
        else if (cmd_code == 1146)
          fcn = mpfr_round;
        else if (cmd_code == 1147)
          fcn = mpfr_roundeven;
        else if (cmd_code == 1148)
          fcn = mpfr_trunc;
        else
          MEX_FCN_ERR ("cmd[%d]: Bad operator.\n", cmd_code);

        plhs[0] = mxCreateNumericMatrix (nlhs ? length (&rop) : 1, 1,
                                         mxDOUBLE_CLASS, mxREAL);
        double *ret_ptr    = mxGetPr (plhs[0]);
        size_t  ret_stride = (nlhs) ? 1 : 0;

        for (size_t i = 0; i < length (&rop); i++)
          ret_ptr[i * ret_stride] = (double) fcn (&mpfr_data[rop.start - 1 + i],
                                                  &mpfr_data[op.start - 1 + i]);
        return;
      }

      case 1157:  // int mpfr_fmodquo (mpfr_t r, long* q, mpfr_t x, mpfr_t y, mpfr_rnd_t rnd)
      case 1159:  // int mpfr_remquo (mpfr_t r, long* q, mpfr_t x, mpfr_t y, mpfr_rnd_t rnd)
      {
        MEX_NARGINCHK (5);
        MEX_MPFR_T (1, r);
        MEX_MPFR_T (2, x);
        if ((length (&x) != length (&r)) && (length (&x) != 1))
          MEX_FCN_ERR ("cmd[%d]:x Invalid size.\n", cmd_code);
        MEX_MPFR_T (3, y);
        if ((length (&y) != length (&r)) && (length (&y) != 1))
          MEX_FCN_ERR ("cmd[%d]:y Invalid size.\n", cmd_code);
        MEX_MPFR_RND_T (4, rnd);
        DBG_PRINTF ("cmd[%d]: r = [%d:%d], x = [%d:%d], y = [%d:%d] "
                    "(rnd = %d)\n", cmd_code, r.start, r.end, x.start, x.end,
                    y.start, y.end, (int) rnd);

        int (*fcn)(mpfr_t r, long *, const mpfr_t, const mpfr_t, mpfr_rnd_t);
        if (cmd_code == 1157)
          fcn = mpfr_fmodquo;
        else if (cmd_code == 1159)
          fcn = mpfr_remquo;
        else
          MEX_FCN_ERR ("cmd[%d]: Bad operator.\n", cmd_code);

        plhs[0] = mxCreateNumericMatrix (nlhs ? length (&r) : 1, 1,
                                         mxDOUBLE_CLASS, mxREAL);
        plhs[1] = mxCreateNumericMatrix (nlhs ? length (&r) : 1, 1,
                                         mxDOUBLE_CLASS, mxREAL);
        double * ret_ptr    = mxGetPr (plhs[0]);
        double * q_ptr      = mxGetPr (plhs[1]);
        mpfr_ptr r_ptr      = &mpfr_data[r.start - 1];
        mpfr_ptr x_ptr      = &mpfr_data[x.start - 1];
        mpfr_ptr y_ptr      = &mpfr_data[y.start - 1];
        size_t   ret_stride = (nlhs) ? 1 : 0;
        size_t   x_stride   = (length (&x) == 1) ? 0 : 1;
        size_t   y_stride   = (length (&y) == 1) ? 0 : 1;
        for (size_t i = 0; i < length (&x); i++)
          {
            long q;
            ret_ptr[i * ret_stride] = (double) fcn (r_ptr + i, &q,
                                                    x_ptr + (i * x_stride),
                                                    y_ptr + (i * y_stride),
                                                    rnd);
            q_ptr[i * ret_stride] = (double) q;
          }
        return;
      }

      case 1163:  // int mpfr_prec_round (mpfr_t x, mpfr_prec_t prec, mpfr_rnd_t rnd)
      {
        MEX_NARGINCHK (4);
        MEX_MPFR_T (1, x);
        MEX_MPFR_PREC_T (2, prec);
        MEX_MPFR_RND_T (3, rnd);
        DBG_PRINTF ("cmd[%d]: x = [%d:%d], prec = %d, rnd = %d\n", cmd_code,
                    x.start, x.end, (int) prec, (int) rnd);

        plhs[0] = mxCreateNumericMatrix (nlhs ? length (&x) : 1, 1,
                                         mxDOUBLE_CLASS, mxREAL);
        double * ret_ptr    = mxGetPr (plhs[0]);
        mpfr_ptr x_ptr      = &mpfr_data[x.start - 1];
        size_t   ret_stride = (nlhs) ? 1 : 0;

        for (size_t i = 0; i < length (&x); i++)
          ret_ptr[i * ret_stride] = (double) mpfr_prec_round (x_ptr + i, prec,
                                                              rnd);
        return;
      }

      case 1164:  // int mpfr_can_round (mpfr_t b, mpfr_exp_t err, mpfr_rnd_t rnd1, mpfr_rnd_t rnd2, mpfr_prec_t prec)
      {
        MEX_NARGINCHK (6);
        MEX_MPFR_T (1, b);
        MEX_MPFR_EXP_T (2, err)
        MEX_MPFR_RND_T (3, rnd1);
        MEX_MPFR_RND_T (4, rnd2);
        MEX_MPFR_PREC_T (5, prec);
        DBG_PRINTF ("cmd[%d]: b = [%d:%d], rnd1 = %d, rnd2 = %d, prec = %d\n",
                    cmd_code, b.start, b.end, (int) rnd1, (int) rnd2,
                    (int) prec);

        plhs[0] = mxCreateNumericMatrix (nlhs ? length (&b) : 1, 1,
                                         mxDOUBLE_CLASS, mxREAL);
        double * ret_ptr    = mxGetPr (plhs[0]);
        mpfr_ptr b_ptr      = &mpfr_data[b.start - 1];
        size_t   ret_stride = (nlhs) ? 1 : 0;

        for (size_t i = 0; i < length (&b); i++)
          ret_ptr[i * ret_stride] = (double) mpfr_can_round (b_ptr + i, err,
                                                             rnd1, rnd2, prec);
        return;
      }

      case 1176:  // mpfr_exp_t mpfr_get_exp (mpfr_t x)
      {
        MEX_NARGINCHK (2);
        MEX_MPFR_T (1, x);
        DBG_PRINTF ("cmd[%d]: [%d:%d]\n", cmd_code, x.start, x.end);

        plhs[0] = mxCreateNumericMatrix (length (&x), 1, mxDOUBLE_CLASS,
                                         mxREAL);
        double * ret_ptr = mxGetPr (plhs[0]);
        mpfr_ptr x_ptr   = &mpfr_data[x.start - 1];

        for (size_t i = 0; i < length (&x); i++)
          ret_ptr[i] = (double) mpfr_get_exp (x_ptr + i);
        return;
      }

      case 1177:  // int mpfr_set_exp (mpfr_t x, mpfr_exp_t e)
      {
        MEX_NARGINCHK (3);
        MEX_MPFR_T (1, x);
        MEX_MPFR_EXP_T (2, e);
        DBG_PRINTF ("cmd[%d]: [%d:%d] (exp = %d)\n", cmd_code, x.start, x.end,
                    (int) e);

        plhs[0] = mxCreateNumericMatrix (nlhs ? length (&x) : 1, 1,
                                         mxDOUBLE_CLASS, mxREAL);
        double * ret_ptr    = mxGetPr (plhs[0]);
        mpfr_ptr x_ptr      = &mpfr_data[x.start - 1];
        size_t   ret_stride = (nlhs) ? 1 : 0;

        for (size_t i = 0; i < length (&x); i++)
          ret_ptr[i * ret_stride] = (double) mpfr_set_exp (x_ptr + i, e);
        return;
      }

      case 1179:  // int mpfr_setsign (mpfr_t rop, mpfr_t op, int s, mpfr_rnd_t rnd)
      {
        MEX_NARGINCHK (5);
        MEX_MPFR_T (1, rop);
        MEX_MPFR_T (2, op);
        if ((length (&op) != length (&rop)) && (length (&op) != 1))
          MEX_FCN_ERR ("cmd[%d]:op Invalid.\n", cmd_code);
        size_t sM = mxGetM (prhs[3]);
        size_t sN = mxGetN (prhs[3]);
        if (! mxIsDouble (prhs[3])
            || (((sM * sN) != length (&rop)) && ((sM * sN) != 1)))
          MEX_FCN_ERR ("cmd[%d]:s Invalid.\n", cmd_code);
        MEX_MPFR_RND_T (4, rnd);
        DBG_PRINTF ("cmd[%d]: rop = [%d:%d], op = [%d:%d] , s = [%d x %d] "
                    "(rnd = %d)\n", cmd_code, rop.start, rop.end,
                    op.start, op.end, sM, sN, (int) rnd);

        plhs[0] = mxCreateNumericMatrix (nlhs ? length (&rop) : 1, 1,
                                         mxDOUBLE_CLASS, mxREAL);
        double * ret_ptr    = mxGetPr (plhs[0]);
        mpfr_ptr rop_ptr    = &mpfr_data[rop.start - 1];
        mpfr_ptr op_ptr     = &mpfr_data[op.start - 1];
        double * s_ptr      = mxGetPr (prhs[3]);
        size_t   ret_stride = (nlhs) ? 1 : 0;
        size_t   op_stride  = (length (&op) == 1) ? 0 : 1;
        size_t   s_stride   = ((sM * sN) == 1) ? 0 : 1;
        for (size_t i = 0; i < length (&rop); i++)
          ret_ptr[i * ret_stride] = (double) mpfr_setsign (rop_ptr + i,
                                                           op_ptr +
                                                           (i * op_stride),
                                                           (int) s_ptr[i *
                                                                       s_stride],
                                                           rnd);
        return;
      }

      case 1190:  // int mpfr_set_emin (mpfr_exp_t exp)
      case 1191:  // int mpfr_set_emax (mpfr_exp_t exp)
      {
        MEX_NARGINCHK (2);
        MEX_MPFR_EXP_T (1, exp);
        DBG_PRINTF ("cmd[%d]: exp = %d\n", cmd_code, (int) exp);

        plhs[0] = mxCreateNumericMatrix (1, 1, mxDOUBLE_CLASS, mxREAL);
        double *ret_ptr = mxGetPr (plhs[0]);
        if (cmd_code == 1190)
          *ret_ptr = (double) mpfr_set_emin (exp);
        else
          *ret_ptr = (double) mpfr_set_emax (exp);
        return;
      }

      case 1196:  // int mpfr_check_range (mpfr_t x, int t, mpfr_rnd_t rnd)
      case 1197:  // int mpfr_subnormalize (mpfr_t x, int t, mpfr_rnd_t rnd)
      {
        MEX_NARGINCHK (4);
        MEX_MPFR_T (1, x);
        size_t tM = mxGetM (prhs[2]);
        size_t tN = mxGetN (prhs[2]);
        if (! mxIsDouble (prhs[2])
            || (((tM * tN) != length (&x)) && ((tM * tN) != 1)))
          MEX_FCN_ERR ("cmd[%d]:t Invalid.\n", cmd_code);
        MEX_MPFR_RND_T (3, rnd);
        DBG_PRINTF ("cmd[%d]: x = [%d:%d], t = [%d:%d] (rnd = %d)\n",
                    cmd_code, x.start, x.end, tM, tN, (int) rnd);

        int (*fcn)(mpfr_t, int, mpfr_rnd_t);
        if (cmd_code == 1196)
          fcn = mpfr_check_range;
        else if (cmd_code == 1197)
          fcn = mpfr_subnormalize;
        else
          MEX_FCN_ERR ("cmd[%d]: Bad operator.\n", cmd_code);

        plhs[0] = mxCreateNumericMatrix (nlhs ? length (&x) : 1, 1,
                                         mxDOUBLE_CLASS, mxREAL);
        double * ret_ptr    = mxGetPr (plhs[0]);
        mpfr_ptr x_ptr      = &mpfr_data[x.start - 1];
        double * t_ptr      = mxGetPr (prhs[2]);
        size_t   ret_stride = (nlhs) ? 1 : 0;
        size_t   t_stride   = ((tM * tN) == 1) ? 0 : 1;
        for (size_t i = 0; i < length (&x); i++)
          ret_ptr[i * ret_stride] = (double) fcn (x_ptr + i,
                                                  (int) t_ptr[i * t_stride],
                                                  rnd);
        return;
      }

      case 1022: // void mpfr_free_str (char *str)
        return;  // NOP

      case 1300: // int MPFR_RNDN (void)
      {
        // round to nearest, with ties to even
        MEX_NARGINCHK (1);
        plhs[0] = mxCreateDoubleScalar ((double) MPFR_RNDN);
        return;
      }

      case 1301: // int MPFR_RNDZ (void)
      {
        // round toward zero
        MEX_NARGINCHK (1);
        plhs[0] = mxCreateDoubleScalar ((double) MPFR_RNDZ);
        return;
      }

      case 1302: // int MPFR_RNDU (void)
      {
        // round toward +Inf
        MEX_NARGINCHK (1);
        plhs[0] = mxCreateDoubleScalar ((double) MPFR_RNDU);
        return;
      }

      case 1303: // int MPFR_RNDD (void)
      {
        // round toward -Inf
        MEX_NARGINCHK (1);
        plhs[0] = mxCreateDoubleScalar ((double) MPFR_RNDD);
        return;
      }

      case 1304: // int MPFR_RNDA (void)
      {
        // round away from zero
        MEX_NARGINCHK (1);
        plhs[0] = mxCreateDoubleScalar ((double) MPFR_RNDA);
        return;
      }

      default:
        MEX_FCN_ERR ("Unknown command code '%d'\n", cmd_code);
    }
}


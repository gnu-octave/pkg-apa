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

      case 17:  // double mpfr_get_d (mpfr_t op, mpfr_rnd_t rnd)
      {
        MEX_NARGINCHK(3);
        MEX_MPFR_T(1, idx);
        MEX_MPFR_RND_T(2, rnd);
        DBG_PRINTF ("cmd[%d]: [%d:%d]\n", cmd_code, idx.start, idx.end);
        plhs[0] = mxCreateNumericMatrix (length (&idx), 1, mxDOUBLE_CLASS,
                                         mxREAL);
        double* plhs_0_pr = mxGetPr (plhs[0]);
        for (size_t i = 0; i < length (&idx); i++)
          plhs_0_pr[i] = mpfr_get_d (&data[(idx.start - 1) + i], rnd);
        break;
      }

      case 6:  // int mpfr_set_d (mpfr_t rop, double op, mpfr_rnd_t rnd)
      {
        MEX_NARGINCHK(4);
        MEX_MPFR_T(1, idx);
        if (! mxIsDouble (prhs[2])
            || ((mxGetM (prhs[2]) * mxGetN (prhs[2])) != length (&idx)))
          {
            MEX_FCN_ERR ("cmd[%d]: Invalid number of double values.\n",
                         cmd_code);
            break;
          }
        double* op_pr = mxGetPr (prhs[2]);
        MEX_MPFR_RND_T(3, rnd);
        DBG_PRINTF ("cmd[%d]: [%d:%d]\n", cmd_code, idx.start, idx.end);

        for (size_t i = 0; i < length (&idx); i++)
          mpfr_set_d (&data[(idx.start - 1) + i], op_pr[i], rnd);
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

      default:
      {
        MEX_FCN_ERR ("Unknown command code '%d'\n", cmd_code);
        break;
      }
    }

  if (throw_error)
    mexErrMsgIdAndTxt ("mp:mexFunction", "See previous error message.");
}

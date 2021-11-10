#include "mex_mpfr_interface.h"

// MPFR memory management
// ======================
//
// Similar to C++ std::vector:
// - xxx_capacity: number of elements that `xxx` has currently allocated
//                 space for.
// - xxx_size:     number of elements in `xxx`.

mpfr_ptr mpfr_data          = NULL;
size_t   mpfr_data_capacity = 0;
size_t   mpfr_data_size     = 0;

idx_t *mpfr_free_list          = NULL;
size_t mpfr_free_list_capacity = 0;
size_t mpfr_free_list_size     = 0;


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
          && ((*idx).end <= mpfr_data_size));
}


/**
 * Function called at exit of the mex file to tidy up all memory.
 * After calling this function the initial state is restored.
 */

static void
mpfr_tidy_up (void)
{
  DBG_PRINTF ("%s\n", "Call");
  for (size_t i = 0; i < mpfr_data_size; i++)
    mpfr_clear (&mpfr_data[i]);
  mxFree (mpfr_data);
  mpfr_data          = NULL;
  mpfr_data_capacity = 0;
  mpfr_data_size     = 0;
  mxFree (mpfr_free_list);
  mpfr_free_list          = NULL;
  mpfr_free_list_capacity = 0;
  mpfr_free_list_size     = 0;
}


/**
 * Remove the i-th entry from the pool of free MPFR variables.
 *
 * @param i index (0-based) to entry to remove from the pool.
 */

void
mpfr_free_list_remove (size_t i)
{
  for (; i < mpfr_free_list_size; i++)
    {
      mpfr_free_list[i].start = mpfr_free_list[i + 1].start;
      mpfr_free_list[i].end   = mpfr_free_list[i + 1].end;
    }
  mpfr_free_list_size--;
}


/**
 * Try to compress the pool of free MPFR variables.
 */

void
mpfr_free_list_compress ()
{
  int have_merge = 0;

  do
    {
      have_merge = 0;
      for (size_t i = 0; i < mpfr_free_list_size; i++)
        {
          // Rule 1: If end index of entry matches `mpfr_data_size` decrease.
          if (mpfr_free_list[i].end == mpfr_data_size)
            {
              have_merge = 1;
              DBG_PRINTF ("mmgr: Rule 1 for [%d:%d].\n",
                          mpfr_free_list[i].start, mpfr_free_list[i].end);
              mpfr_data_size = mpfr_free_list[i].start - 1;
              mpfr_free_list_remove (i);
              break;
            }
          // Rule 2: Merge neighboring entries.
          int do_break = 0;
          for (size_t j = i + 1; j < mpfr_free_list_size; j++)
            if ((mpfr_free_list[i].end + 1 == mpfr_free_list[j].start)
                || (mpfr_free_list[j].end + 1 == mpfr_free_list[i].start))
              {
                have_merge = 1;
                do_break   = 1;
                DBG_PRINTF ("mmgr: Rule 2 for [%d:%d] + [%d:%d].\n",
                            mpfr_free_list[i].start, mpfr_free_list[i].end,
                            mpfr_free_list[j].start, mpfr_free_list[j].end);
                mpfr_free_list[i].start =
                  (mpfr_free_list[i].start <= mpfr_free_list[j].start
                   ? mpfr_free_list[i].start
                   : mpfr_free_list[j].start);
                mpfr_free_list[i].end =
                  (mpfr_free_list[i].end >= mpfr_free_list[j].end
                   ? mpfr_free_list[i].end
                   : mpfr_free_list[j].end);
                mpfr_free_list_remove (j);
                break;
              }
          if (do_break)
            break;
        }
    } while (have_merge);

  /* Very verbose debug output.
   * for (size_t i = 0; i < mpfr_free_list_size; i++)
   * DBG_PRINTF ("mmgr: mpfr_free_list[%d] = [%d:%d].\n", i,
   *            mpfr_free_list[i].start, mpfr_free_list[i].end);
   */
}


/**
 * Mark MPFR variable as no longer used.
 *
 * @param[in] idx Pointer to index (1-based, idx_t) of MPFR variable to be no
 *                longer used.
 *
 * @returns success of MPFR variables creation.
 */

void
mex_mpfr_mark_free (idx_t *idx)
{
  // Check for impossible start and end index.
  if (! is_valid (idx))
    {
      DBG_PRINTF ("%s\n", "Bad indices");
      return;
    }

  // Extend space if necessary.
  if (mpfr_free_list_size == mpfr_free_list_capacity)
    {
      size_t new_capacity = mpfr_free_list_capacity + DATA_CHUNK_SIZE;
      DBG_PRINTF ("Increase capacity to '%d'.\n", new_capacity);
      if (mpfr_free_list == NULL)
        {
          mpfr_free_list = (idx_t *) mxMalloc (new_capacity * sizeof(idx_t));
          mexAtExit (mpfr_tidy_up);
        }
      else
        mpfr_free_list = (idx_t *) mxRealloc (mpfr_free_list,
                                              new_capacity * sizeof(idx_t));
      if (mpfr_free_list == NULL)
        return;  // Memory allocation failed.

      mexMakeMemoryPersistent (mpfr_free_list);
      mpfr_free_list_capacity = new_capacity;
    }

  // Reinitialize MPFR variables (free significant memory).
  for (size_t i = idx->start - 1; i < idx->end; i++)
    {
      mpfr_clear (mpfr_data + i);
      mpfr_init (mpfr_data + i);
    }

  // Append reinitialized variables to pool.
  mpfr_free_list[mpfr_free_list_size].start = idx->start;
  mpfr_free_list[mpfr_free_list_size].end   = idx->end;
  mpfr_free_list_size++;

  mpfr_free_list_compress ();
}


/**
 * Constructor for new MPFR variables.
 *
 * @param[in] count Number of MPFR variables to create.
 * @param[out] idx If function returns `1`, pointer to index (1-based, idx_t)
 *                 of MPFR variables, otherwise the value of `idx` remains
 *                 unchanged.
 *
 * @returns success of MPFR variable creation.
 */

int
mex_mpfr_allocate (size_t count, idx_t *idx)
{
  // Check for trivial case, failure as indices do not make sense.
  if (count == 0)
    return (0);

  // Try to reuse a free marked variable from the pool.
  for (size_t i = 0; i < mpfr_free_list_size; i++)
    if (count <= length (mpfr_free_list + i))
      {
        idx->start = mpfr_free_list[i].start;
        idx->end   = mpfr_free_list[i].start + count - 1;
        DBG_PRINTF ("New MPFR variable [%d:%d] reused.\n",
                    idx->start, idx->end);
        if (count < length (mpfr_free_list + i))
          mpfr_free_list[i].start += count;
        else
          mpfr_free_list_remove (i);
        return (is_valid (idx));
      }

  // Check if there is enough space to create new MPFR variables.
  if ((mpfr_data_size + count) > mpfr_data_capacity)
    {
      // Determine new capacity.
      size_t new_capacity = mpfr_data_capacity;
      while ((mpfr_data_size + count) > new_capacity)
        new_capacity += DATA_CHUNK_SIZE;

      DBG_PRINTF ("Increase capacity to '%d'.\n", new_capacity);
      // Reallocate memory.
      if (mpfr_data == NULL)
        {
          mpfr_data = (mpfr_ptr) mxMalloc (new_capacity * sizeof(mpfr_t));
          mexAtExit (mpfr_tidy_up);
        }
      else
        mpfr_data =
          (mpfr_ptr) mxRealloc (mpfr_data, new_capacity * sizeof(mpfr_t));
      if (mpfr_data == NULL)
        return (0); // Memory allocation failed.

      mexMakeMemoryPersistent (mpfr_data);

      // Initialize new MPFR variables.
      for (size_t i = mpfr_data_capacity; i < new_capacity; i++)
        mpfr_init (mpfr_data + i);

      mpfr_data_capacity = new_capacity;
    }

  idx->start      = mpfr_data_size + 1;
  mpfr_data_size += count;
  idx->end        = mpfr_data_size;
  DBG_PRINTF ("New MPFR variable [%d:%d] allocated.\n", idx->start, idx->end);
  return (is_valid (idx));
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
extract_idx (int idx, int nrhs, const mxArray *prhs[], idx_t *idx_vec)
{
  uint64_t *ui = NULL;

  if (extract_ui_vector (idx, nrhs, prhs, &ui, 2))
    {
      (*idx_vec).start = ui[0];
      (*idx_vec).end   = ui[1];
      mxFree (ui);
      if (is_valid (idx_vec))
        return (1);

      DBG_PRINTF ("Invalid index [%d:%d].\n", (*idx_vec).start, (*idx_vec).end);
    }
  DBG_PRINTF ("%s\n", "Failed.");
  return (0);
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
  return (d);
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
extract_rounding_mode (int idx, int nrhs, const mxArray *prhs[],
                       mpfr_rnd_t *rnd)
{
  int64_t si = 0;

  if (extract_si (idx, nrhs, prhs, &si))
    switch (si)
      {
        case -1:
          *rnd = MPFR_RNDD;
          return (1);

        case 0:
          *rnd = MPFR_RNDN;
          return (1);

        case 1:
          *rnd = MPFR_RNDU;
          return (1);

        case 2:
          *rnd = MPFR_RNDZ;
          return (1);

        case 3:
          *rnd = MPFR_RNDA;
          return (1);

        default:
          break;
      }
  return (0);
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
      return (1);
    }
  return (0);
}


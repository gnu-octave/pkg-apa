/*
 * This file is part of APA.
 *
 *  APA is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  APA is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with APA.  If not, see <https://www.gnu.org/licenses/>.
 */

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


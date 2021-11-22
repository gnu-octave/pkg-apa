/**
 * Simple example computing the following sequence with MPFR:
 * u[0] = 2; u[1] = −4; u[n] = 111 − 1130/u[n−1] + 3000/(u[n−1]*u[n−2]).
 *
 * Example from page 549 (C listing 14.1):
 *
 *   J.-M. Muller et al., Handbook of Floating-Point Arithmetic. Springer
 *   International Publishing, 2018.
 *   doi: https://doi.org/10.1007/978-3-319-76526-6.
 *
 * Convergence to "100.0" for "(k = 2; k <= 300; k++)", see above.
 */

#include <mpfr.h>

static void display (mpfr_t v, int k)
{
  mpfr_printf ("u[%d] = %.20Rg\n", k, v);
}

int main (void)
{
  mpfr_t u, v, x, y, z;
  int k;

  mpfr_inits2 (600, u, v, x, y, z, (mpfr_ptr) 0);

  mpfr_set_si (u, 2, MPFR_RNDN);    // u[0] = 2
  mpfr_set_si (v, -4, MPFR_RNDN);   // u[1] = -4

  display (u, 0);
  display (v, 1);

  for (k = 2; k < 200; k++)
    {
      // u = u[n-2], v = u[n-1]
      mpfr_ui_div (x, 1130, v, MPFR_RNDN);  // x = 1130 / v
      mpfr_ui_sub (y, 111, x, MPFR_RNDN);   // y = 111 - x
      mpfr_mul (z, v, u, MPFR_RNDN);        // z = v * u
      mpfr_ui_div (x, 3000, z, MPFR_RNDN);  // x = 3000 / z
      mpfr_set (u, v, MPFR_RNDN);           // u = v
      mpfr_add (v, y, x, MPFR_RNDN);        // v = y + x
      display (v, k);
    }

  mpfr_clears (u, v, x, y, z, (mpfr_ptr) 0);
  return 0;
}

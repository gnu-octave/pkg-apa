# APA - Octave/Matlab arbitrary precision arithmetic.

## Installation

From the Octave command-line run:


```octave
pkg install 'https://github.com/gnu-octave/apa/releases/download/v0.1.7/apa-0.1.7.zip'
pkg load apa
pkg test apa
```

From the Matlab command-line run (also works for Octave):


```octave
urlwrite ('https://github.com/gnu-octave/apa/releases/download/v0.1.7/apa-0.1.7.zip', ...
          'apa-0.1.7.zip');
unzip ('apa-0.1.7.zip');
cd (fullfile ('apa-0.1.7', 'inst'))
install_apa
test_apa
```

## High-level MPFR Interface

The high-level MPFR interface is given through the `@mpfr_t` class.
A variable of that type "behaves" like a "normal" built-in Octave/Matlab
data type.


```octave
op1 = mpfr_t (eye (3) * 4);

rop = op1 + 1
```

    rop =

       5   1   1
       1   5   1
       1   1   5



However, you can adjust the binary precision.

The default Octave/Matlab data type **(double)** has a precision of 53 binary digits.
Thus the following calculation exceeds the given precision:


```octave
format long
too_small = (2 ^ (-60))
A = ones (3);
A(3,3) = A(3,3) + too_small
```

    too_small = 8.673617379884035e-19
    A =

       1   1   1
       1   1   1
       1   1   1




```octave
B = A - ones (3)
```

    B =

       0   0   0
       0   0   0
       0   0   0



The same calculation using APA and quadruple precision (113 binary digits):


```octave
A = mpfr_t (ones (3), 113);
A(3,3) = A(3,3) + too_small
```

    A =

       1   1                                       1
       1   1                                       1
       1   1   1.00000000000000000086736173798840355




```octave
apa ('format.fmt', 'scientific')
apa ('format.base', 2)
B = A - ones (3)
```

    B =

       0 * 2^(0)   0 * 2^(0)     0 * 2^(0)
       0 * 2^(0)   0 * 2^(0)     0 * 2^(0)
       0 * 2^(0)   0 * 2^(0)   1 * 2^(-60)



The high-level MPFR interface is the preferred choice for quick numerical
experiments.

However, if performance is more critical, please use the low-level MPFR
interface (explained below) and vectorization wherever possible.

> Please note that an interface from an interpreted high-level programming
> language like Octave/Matlab is most likely slower than a pre-compiled C
> program.
>
> If performance is highly-critical, use this tool for initial experiments
> and translate the developed algorithm to native MPFR C-code.

## Low-level MPFR Interface

> For information how to compile/develop the interface, see
> [`doc/MEX_INTERFACE.md`](https://github.com/gnu-octave/apa/blob/main/doc/MEX_INTERFACE.md).

The low-level MPFR interface permits efficient access to almost all functions
specified by MPFR 4.1.0 <https://www.mpfr.org/mpfr-4.1.0/mpfr.html>.

All supported functions are [listed in the `inst` folder](inst)
and can be called from Octave/Matlab like in the C programming language.

For example, the C function:

```c
int mpfr_add (mpfr_t rop, mpfr_t op1, mpfr_t op2, mpfr_rnd_t rnd)
```

can be called from Octave/Matlab with scalar, vector, or matrix quantities:


```octave
% Reset to default APA output.
clear apa

% Prepare input and output variables.
rop = mpfr_t (zeros (3));
op1 = mpfr_t (eye (3) * 4);
op2 = mpfr_t (2);
rnd = mpfr_get_default_rounding_mode ();

% Call mpfr_add.  Note unlike Octave/Matlab the
% left-hand side does NOT contain the result.
ret = mpfr_add (rop, op1, op2, rnd);

rop  % Note rop vs. ret!
```

    rop =

       6   2   2
       2   6   2
       2   2   6



In the low-level interface the type checks are stricter,
but scalar and matrix quantities can still be mixed.

Another benefit of using the low-level MPFR interface is that **in-place**
operations are permitted, which do not create new (temporary) variables:


```octave
ret = mpfr_add (op1, op1, op1, rnd);  % op1 += op1
```

# The APA Octave/Matlab MEX interface

The APA Octave/Matlab MEX-interface consists of the source file
`inst/mex/gmp_mpfr_interface.c` (and header file) and in the released package
version with pre-compiled static GMP and MPFR libraries for MS Windows, macOS,
and UNIX (Linux).

If those pre-compiled libraries are missing or not working, please read below.

From Octave call:

```octave
pkg load apa
install_apa
test_apa
```

From Matlab call:

```matlab
cd /path/to/apa/inst
install_apa
test_apa
```

The APA MEX interface is known to work and tested for

- GNU Octave
  - Version 6.3.0
    - MS Windows 10: <https://www.gnu.org/software/octave/download#ms-windows>
    - macOS 11 (Big Sur): <https://formulae.brew.sh/formula/octave>
    - Linux (openSUSE 15.3): <https://github.com/gnu-octave/docker>
- Matlab
  - R2021a
    - MS Windows 10: Official installer
    - macOS 11 (Big Sur): Official installer
    - Linux (openSUSE 15.3): Official installer

> **Matlab Note:** The mex compiler must be fully functional.
> Please consult the official Matlab support or the website
> <https://www.mathworks.com/support/requirements/supported-compilers.html>,
> if you face any issues.
> For MS Windows, the free MinGW compiler must be used.

> **MS Windows Note:** To compile MPFR with Visual Studio, please look at the
> following project <https://github.com/BrianGladman/mpfr>.  However, this
> approach is not supported by APA and requires some adaptions.


## Compiling static GMP and MPFR libraries for MS Windows, macOS, and UNIX (Linux)

For macOS and UNIX/Linux systems

<https://wiki.octave.org/Windows_Installer>

```
hg clone https://hg.octave.org/mxe-octave
cd mxe-octave
./bootstrap
./configure \
  --enable-devel-tools \
  --enable-binary-packages \
  --with-ccache \
  --enable-octave=release \
  --enable-static \
  --disable-shared
make JOBS=8 mpfr
cp ./usr/x86_64-w64-mingw32/lib/libgmp.a \
   ./usr/x86_64-w64-mingw32/lib/libmpfr.a \
   ./usr/x86_64-w64-mingw32/include/gmp.h \
   ./usr/x86_64-w64-mingw32/include/mpfr.h \
   ./usr/x86_64-w64-mingw32/include/mpf2mpfr.h \
   /path/to/apa/inst/mex/mswin
```

### Matlab

> Only Matlab R2018b and later





$(ENABLE_SHARED_OR_STATIC) --with-pic=yes

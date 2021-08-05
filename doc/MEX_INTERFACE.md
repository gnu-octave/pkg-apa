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
> approach is not supported by APA and requires some adaptions of this project.


## Compiling static GMP and MPFR libraries for MS Windows, macOS, and UNIX (Linux)

> Usually this step is **not** necessary, as pre-compiled static library
> versions (`libgmp.a`, `libmpfr.a`, `gmp.h`, `mpfr.h`, and `mpf2mpfr.h`)
> are included in the released package in the respective folders
> - `apa/inst/mex/macos`
> - `apa/inst/mex/mswin`
> - `apa/inst/mex/unix`

For macOS and UNIX/Linux systems (Octave or Matlab) this can be conveniently
done by calling:
```matlab
cd apa/inst/mex
system ('make')
```

For MS Windows one has to cross-compile the respective libraries using
the [octave-mxe](https://wiki.octave.org/Windows_Installer) project.

> The following steps have to be done on Linux (e.g. in a virtual machine).

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

# The APA Octave/Matlab MEX interface

For general installation instructions, please read `README.md`.

The APA Octave/Matlab MEX-interface consists of several source files in the
`inst/mex` directory (C and header files).

## Dependencies

- Multiple Precision Floating-Point Reliable Library (MPFR) 4.1.0+
  - Website: <https://www.mpfr.org/mpfr-current/mpfr.html>
  - Ubuntu Linux: `sudo apt-get install libmpfr-dev` <https://launchpad.net/ubuntu/+source/mpfr4>
  - Fedora Linux: `sudo dnf install mpfr-devel` <https://packages.fedoraproject.org/pkgs/mpfr/mpfr-devel>
  - macOS Homebrew: `brew install mpfr` <https://formulae.brew.sh/formula/mpfr>
  - MS Windows mingw: `pacman -S mingw-w64-x86_64-mpfr` <https://packages.msys2.org/packages/mingw-w64-x86_64-mpfr>
- GNU Multiple Precision Arithmetic Library (GMP) 6.2.0+
  - Website: <https://gmplib.org/>
  - Ubuntu Linux: `sudo apt-get install libgmp-dev` <https://launchpad.net/ubuntu/+source/gmp>
  - Fedora Linux: `sudo dnf install gmp-devel` <https://packages.fedoraproject.org/pkgs/gmp/gmp-devel>
  - macOS Homebrew: `brew install gmp` <https://formulae.brew.sh/formula/gmp>
  - MS Windows mingw: `pacman -S mingw-w64-x86_64-gmp` <https://packages.msys2.org/packages/mingw-w64-x86_64-gmp>

### GNU Octave 9.1.0+

- MS Windows: <https://www.gnu.org/software/octave/download#ms-windows>
- macOS: <https://formulae.brew.sh/formula/octave>
- Linux: <https://github.com/gnu-octave/docker>

### Matlab R2025b+

#### MS Windows

Please follow the instructions from <https://www.fil.ion.ucl.ac.uk/spm/docs/development/compilation/windows/>:

> Download and install MSYS2 from <https://www.msys2.org/> in directory `C:\msys64`.
>
> Then, from MSYS2, type:
> ```
> pacman -Syu
> pacman -Su  
> pacman -S --needed base-devel mingw-w64-x86_64-toolchain
> pacman -S mingw-w64-x86_64-mpfr mingw-w64-x86_64-gmp  
> ```
>
> Start MATLAB and type:
> ```
> setenv('MW_MINGW64_LOC', 'C:\msys64\mingw64')
> mex -setup
> % MEX configured to use 'MinGW64 Compiler (C)' for C language compilation.
> ```
>
> Do not worry if this warning is displayed:
> ```
> Warning: The MATLAB C and Fortran API has changed to support MATLAB
>      variables with more than 2^32-1 elements. You will be required
>      to update your code to utilize the new API.
> ```

See also:
- <https://www.mathworks.com/matlabcentral/fileexchange/52848-matlab-support-for-mingw-w64-c-c-fortran-compiler>
- <https://www.mathworks.com/support/requirements/supported-compilers.html>.

> **Note:** To compile MPFR with Visual Studio, please look at the following project <https://github.com/BrianGladman/mpfr>.
> However, this approach is not supported by APA and requires some adaptions of this project.

#### Linux

To use MATLAB's MEX compiler, MATLAB must eventually started from Terminal with this command

```bash
# For libstdc++ library in Ubuntu. The system libstdc++ can be force loaded with the required version.
export LD_PRELOAD=/lib/x86_64-linux-gnu/libstdc++.so.6 matlab 
```

<https://www.mathworks.com/matlabcentral/answers/2021516-why-doesn-t-simple-mex-compilation-work-on-linux#answer_1340311>

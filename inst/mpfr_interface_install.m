function mpfr_interface_install (opts)
% Install the MPFR interface.

  % Latest GMP and MPFR versions to use in case of custom build.
  gmp_ver = '6.2.1';
  mpfr_ver = '4.1.0';

  custom_build = true;
  install_dir = '';

  verbose = true;
  cflags = {'--std=c99', '-Wall', '-Wextra'};
  if (verbose)
    cflags{end+1} = '-v';
  end
  ldflags = {'-lmpfr', '-lgmp'};

  if (custom_build)
    current_dir = pwd ();
    try
      install_dir = install_gmp_mpfr (verbose, gmp_ver, mpfr_ver);
      cflags{end+1} = ['-I', fullfile(install_dir, 'include')];
      ldflags = fullfile (install_dir, 'lib', {'libmpfr.a', 'libgmp.a'});
    catch ME
      cd (current_dir);
      error ("GMP and MPFR installation failed.\n");
    end
  end

  if (exist('OCTAVE_VERSION', 'builtin') == 5)
    mex (cflags{:}, 'mpfr_interface.c', ldflags{:});
  else
    mex (['CFLAGS="$CFLAGS ', strjoin(cflags, ' '), '"'], ...
        'mpfr_interface.c', ldflags{:});
  end

  [~] = rmdir (install_dir);
end

function install_dir = install_gmp_mpfr (verbose, gmp_ver, mpfr_ver)
% Install The GNU Multiple Precision Arithmetic Library (GMP)

  if (verbose)
    configure_silent = '';
    make_cmd = 'make ';
  else
    configure_silent = '--silent --enable-silent-rules';
    make_cmd = 'make --silent ';
  end

  % Ensure temporary directory to build
  tmp_dir = tempname ();
  if (~ mkdir (tmp_dir))
    error ('mpfr_interface_install:tmp_dir', ...
      'Could not create temproary build directory.');
  end
  old_dir = cd (tmp_dir);

  disp (' ');
  disp ('   Step (1/2) Build GMP    This might take a few minutes...');
  gmp_dir = ['gmp-', gmp_ver];
  gmp_tarball = [gmp_dir, '.tar.lz'];
  urlwrite (['https://gmplib.org/download/gmp/', gmp_tarball], gmp_tarball);
  if (exist('OCTAVE_VERSION', 'builtin') == 5)
    untar (gmp_tarball);
  else
    system (['tar -xf ', gmp_tarball]);
  end
  cd (gmp_dir);
  system (['./configure --prefix=', tmp_dir,' --with-pic=yes ', ...
    ' --libdir=', fullfile(tmp_dir, 'lib'), ' ', configure_silent]);
  system ([make_cmd, '-j4']);
  system ([make_cmd, 'install']);
  cd ('..');

  % Install The GNU MPFR Library

  disp ('Step (2/2) Build MPFR    This might take a few minutes...');
  mpfr_dir = ['mpfr-', mpfr_ver];
  mpfr_tarball = [mpfr_dir, '.tar.gz'];
  urlwrite (['https://www.mpfr.org/mpfr-current/', mpfr_tarball], mpfr_tarball);
  untar (mpfr_tarball);
  cd (mpfr_dir);
  system (['./configure --prefix=', tmp_dir,' --with-pic=yes ', ...
    ' --with-gmp-include=', fullfile(tmp_dir, 'include'), ...
    ' --with-gmp-lib=', fullfile(tmp_dir, 'lib'), ...
    ' --libdir=', fullfile(tmp_dir, 'lib'), ' ', configure_silent]);
  system ([make_cmd, '-j4']);
  system ([make_cmd, 'install']);
  cd (old_dir);
  install_dir = tmp_dir;
  return;
end

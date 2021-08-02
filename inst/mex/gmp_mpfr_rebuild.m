function gmp_mpfr_rebuild ()
% Build static GMP and MPFR libraries in this directory.

  % Latest GMP and MPFR versions to use in case of custom build.
  gmp_ver = '6.2.1';
  mpfr_ver = '4.1.0';

  % Ensure temporary directory to build
  tmp_dir = tempname ();
  if (~ mkdir (tmp_dir))
    error ('mpfr_interface_install:tmp_dir', ...
      'Could not create temproary build directory.');
  end
  old_dir = cd (tmp_dir);

  disp (' ');
  disp ('   Step (1/3) Build GMP    This might take a few minutes...');
  disp (' ');
  gmp_dir = ['gmp-', gmp_ver];
  gmp_tarball = [gmp_dir, '.tar.bz2'];
  urlwrite (['https://gmplib.org/download/gmp/', gmp_tarball], gmp_tarball);
  if (exist('OCTAVE_VERSION', 'builtin') == 5)
    untar (gmp_tarball);
  else
    system (['tar -xf ', gmp_tarball]);
  end
  cd (gmp_dir);
  system (['./configure --prefix=', tmp_dir,' --with-pic=yes ', ...
    ' --libdir=', fullfile(tmp_dir, 'lib')]);
  system ('make -j4');
  system ('make install');
  cd ('..');

  % Install The GNU MPFR Library

  disp (' ');
  disp ('Step (2/3) Build MPFR    This might take a few minutes...');
  disp (' ');
  mpfr_dir = ['mpfr-', mpfr_ver];
  mpfr_tarball = [mpfr_dir, '.tar.gz'];
  urlwrite (['https://www.mpfr.org/mpfr-current/', mpfr_tarball], mpfr_tarball);
  untar (mpfr_tarball);
  cd (mpfr_dir);
  system (['./configure --prefix=', tmp_dir,' --with-pic=yes ', ...
    ' --with-gmp-include=', fullfile(tmp_dir, 'include'), ...
    ' --with-gmp-lib=', fullfile(tmp_dir, 'lib'), ...
    ' --libdir=', fullfile(tmp_dir, 'lib')]);
  system ('make -j4');
  system ('make install');
  cd (old_dir);

  disp (' ');
  disp ('   Step (3/3) Copy GMP and MPFR files');
  disp (' ');

  cfiles = [fullfile(tmp_dir, 'include', {'gmp.h', 'mpfr.h', 'mpf2mpfr.h'}), ...
            fullfile(tmp_dir, 'lib', {'libgmp.a', 'libmpfr.a'})];
  copyfile (cfiles , '.');

  if (exist('OCTAVE_VERSION', 'builtin') == 5)
    confirm_recursive_rmdir (true, 'local');
  end
  rmdir (tmp_dir, 's');

  disp (' ');
  disp ('   FINISHED');
  disp (' ');

end

function install_apa (cmd)
% Install GMP and MPFR MEX interface.
%
%   'rebuild'  -  Rebuild and overwrite the MEX interface.
%

  if (nargin < 1)
    cmd = '';
  end

  [apa_dir, ~, ~] = fileparts (mfilename ('fullpath'));

  old_dir = cd (apa_dir);

  if (strcmp (cmd, 'rebuild') ...
      || exist (['mex_apa_interface.', mexext()], 'file') ~= 3)

    cd (fullfile (apa_dir, 'mex'));

    header = {'gmp.h', 'mpfr.h', 'mpf2mpfr.h'};
    static_libs = {'libmpfr.a', 'libgmp.a'};
    cfiles = {'mex_apa_interface.c', ...
              'mex_gmp_interface.c', ...
              'mex_mpfr_interface.c', ...
              'mex_mpfr_interface_memory_managment.c', ...
              'mex_mpfr_algorithms.c', ...
              'mex_mpfr_algorithms_dot.c', ...
              'mex_mpfr_algorithms_mmm.c', ...
              'mex_mpfr_algorithms_gauss.c'};

    % Set cflags and ldflags according to OS and Octave/Matlab.
    cflags = {'--std=c99', '-Wall', '-Wextra'};
    if (ismac ())
      static_libs_dir = 'macos';
      cflags = [cflags, {'-Xpreprocessor', '-fopenmp'}];
      if (exist ('OCTAVE_VERSION', 'builtin') == 5)
        ldflags = {'-lomp'};
      else
        % Matlab crashes when `gomp` is linked, use omp shipped with Matlab.
        cflags{end+1} = '-Imacos/matlab';  % Provides omp.h.
        ldflags = fullfile (matlabroot (), 'sys', 'os', computer ('arch'));
        ldflags = {['-L', ldflags], '-liomp5'};
      end
    elseif (ispc ())
      static_libs_dir = 'mswin';
      cflags = [cflags, {'-fopenmp'}];
      if (exist('OCTAVE_VERSION', 'builtin') == 5)
        ldflags = {'-lgomp'};
      else
        static_libs = [static_libs, {'libgomp.a'}];
        ldflags = {};
      end
    elseif (isunix ())
      static_libs_dir = 'unix';
      cflags = [cflags, {'-fopenmp'}];
      ldflags = {'-lgomp'};
    else
      error ('install_apa: Could not detect operating system.');
    end

    % Download pre-compiled static GMP and MPFR library.
    if (exist (static_libs_dir, 'dir') ~= 7)
      static_libs_url = 'https://github.com/gnu-octave/pkg-apa/releases/download/';
      static_libs_ver = '1.0.0';
      static_libs_zip = sprintf ('%s-libs-minimal-%s.zip', static_libs_dir, ...
                                 static_libs_ver);
      static_libs_url = sprintf ('%sv%s/%s', static_libs_url, ...
                                 static_libs_ver, static_libs_zip);
      urlwrite (static_libs_url, static_libs_zip);
      unzip  (static_libs_zip);
      delete (static_libs_zip);
    end
    
    % Add static GMP and MPFR library to compiler flags.
    if (is_complete (pwd (), [header, static_libs]))
      cflags{end+1} = '-I.';
      ldflags = [static_libs, ldflags];
    elseif (is_complete (fullfile (pwd (), static_libs_dir), ...
                         [header, static_libs]))
      cflags{end+1} = ['-I', static_libs_dir];
      ldflags = [fullfile(static_libs_dir, static_libs), ldflags];
    else
      error (['install_apa: Could not find pre-built GMP or MPFR ', ...
        'libraries.  Please run the Makefile in the "mex" directory.']);
    end

    try
      if (exist('OCTAVE_VERSION', 'builtin') == 5)
        mex (cflags{:}, cfiles{:}, ldflags{:});
      else
        mex (['CFLAGS="$CFLAGS ', strjoin(cflags, ' '), '"'], ...
             cfiles{:}, ldflags{:});
      end
      movefile (['mex_apa_interface.', mexext()], '..');
    catch
      cd (old_dir);
      error ('MEX interface creation failed.  APA cannot be used.');
    end

    disp ('APA is ready to use.');
  end

  cd (old_dir);

  add_to_path_if_not_exists (apa_dir);
end



function bool = is_complete (dir, files)
  bool = true;
  for ff = files
    f = ff{1};
    bool = bool && (exist (fullfile (dir, f), 'file') == 2);
    if (~ bool)
      return;
    end
  end
end



function add_to_path_if_not_exists (p)

  pc = regexp (path, pathsep, 'split');
  if (ispc ())  % MS Windows is not case-sensitive
    bool = any (strcmpi (p, pc));
  else
    bool = any (strcmp (p, pc));
  end
  if (~ bool)
    addpath (p);
  end

end


% The following PKG_ADD directive is used for the Octave pkg-system, such that
% "pkg (un)load" or addition to the manual addition to the load path ensures
% the mex file installation.
%{
## PKG_ADD: install_apa ();
%}

% Adapter for Octave to run the test suite for "pkg test".
%!test
% test_apa ();

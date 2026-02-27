function install_apa (cmd)
% Install GMP and MPFR MEX interface.
%
%   'rebuild'  -  Rebuild and overwrite the MEX interface.
%
% After installation, find the version of dynamically linked GMP and MPFR libraries with
%
%    >> gmp_version ()
%    ans = 6.3.0
%
%    >> mpfr_get_version ()
%    ans = 4.2.1
%

  if (nargin < 1)
    cmd = '';
  end

  [apa_dir, ~, ~] = fileparts (mfilename ('fullpath'));

  old_dir = cd (apa_dir);

  if (strcmp (cmd, 'rebuild') || exist (['mex_apa_interface.', mexext()], 'file') ~= 3)

    cd (fullfile (apa_dir, 'mex'));

    cfiles = {'mex_apa_interface.c', ...
              'mex_gmp_interface.c', ...
              'mex_mpfr_interface.c', ...
              'mex_mpfr_interface_extractors.c', ...
              'mex_mpfr_interface_memory_managment.c', ...
              'mex_mpfr_algorithms.c', ...
              'mex_mpfr_algorithms_dot.c', ...
              'mex_mpfr_algorithms_mmm.c', ...
              'mex_mpfr_algorithms_gauss.c'};

    % Set cflags and ldflags according to OS and Octave/Matlab.
    cflags = {'-Wall', '-Wextra'};
    if (ismac ())
      cflags = [cflags, {'-Xpreprocessor', '-fopenmp'}];
      if (exist ('OCTAVE_VERSION', 'builtin') == 5)
        ldflags = {'-lomp'};
      else
        % Matlab crashes when `gomp` is linked, use omp shipped with Matlab.
        cflags{end+1} = '-Imacos/matlab';  % Provides omp.h.
        ldflags = fullfile (matlabroot (), 'sys', 'os', computer ('arch'));
        ldflags = {['-L', ldflags], '-liomp5'};
      end
    elseif (isunix () || ispc ())
      cflags = [cflags, {'-fopenmp'}];
      ldflags = {'-lmpfr', '-lgomp'};
    else
      error ('install_apa: Could not detect operating system.');
    end


    try
      if (exist('OCTAVE_VERSION', 'builtin') == 5)
        mex (cflags{:}, cfiles{:}, ldflags{:});
      else
        mex (['CFLAGS="$CFLAGS ', strjoin(cflags, ' '), '"'], cfiles{:}, ldflags{:});
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


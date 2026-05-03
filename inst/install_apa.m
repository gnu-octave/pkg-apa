function install_apa (varargin)
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
%    ans = 4.2.2
%

  [apa_dir, ~, ~] = fileparts (mfilename ('fullpath'));

  old_dir = cd (apa_dir);

  if (any (strcmp (varargin, 'rebuild')) || exist (['mex_apa_interface.', mexext()], 'file') ~= 3)

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
    ldflags = {'-lmpfr', '-lgmp'};
    if ismac()
        [has_brew, brew_path] = system ('brew --prefix');
        if (has_brew == 0)  % return code 0 means brew command is found
            brew_path = deblank (brew_path);
            cflags = [cflags(:)', {['-I', fullfile(brew_path, 'include')]}];
            ldflags = [{['-L', fullfile(brew_path, 'lib')]}, ldflags(:)'];
        end
    end


    try
      if (exist('OCTAVE_VERSION', 'builtin') == 5)
        mex (cflags{:}, cfiles{:}, ldflags{:});
      else
        mex (['CFLAGS="$CFLAGS ', strjoin(cflags, ' '), '"'], cfiles{:}, ldflags{:});
      end
      movefile (['mex_apa_interface.', mexext()], '..');
    catch exception
      cd (old_dir);
      error ('MEX interface creation failed: %s\n\nMPFR cannot be used.', exception.message);
    end

  end

  cd (old_dir);

  add_to_path_if_not_exists (apa_dir);

  disp ('APA is ready to use.');
  disp (['    Detected MPFR version: ', mpfr_get_version(), '']);
  disp (['    Detected  GMP version: ', gmp_version(),      '']);
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

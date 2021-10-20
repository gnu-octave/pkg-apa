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

  if (strcmp (cmd, 'rebuild') || exist ('mex_apa_interface', 'file') ~= 3)

    cd (fullfile (apa_dir, 'mex'));

    header = {'gmp.h', 'mpfr.h', 'mpf2mpfr.h'};
    libs = {'libmpfr.a', 'libgmp.a'};
    cflags = {'--std=c99', '-Wall', '-Wextra'};
    cfiles = {'mex_apa_interface.c', ...
              'mex_gmp_interface.c', ...
              'mex_mpfr_interface.c', ...
              'mex_mpfr_algorithms.c'};

    if (is_complete (pwd (), [header, libs]))
      cflags{end+1} = '-I.';
      ldflags = libs;
    elseif (ismac () && is_complete (fullfile (pwd (), 'macos'), [header, libs]))
      cflags{end+1} = '-Imacos';
      ldflags = fullfile ('macos', libs);
    elseif (ispc () && is_complete (fullfile (pwd (), 'mswin'), [header, libs]))
      cflags{end+1} = '-Imswin';
      ldflags = fullfile ('mswin', libs);
    elseif (isunix () && is_complete (fullfile (pwd (), 'unix'), [header, libs]))
      cflags{end+1} = '-Iunix';
      ldflags = fullfile ('unix', libs);
    else
      error (['Could not find pre-built GMP or MPFR libraries.  ', ...
        'Please run the Makefile in the "mex" directory.']);
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

function install_apa ()
% Install GMP and MPFR MEX interface.

  [apa_dir, ~, ~] = fileparts (mfilename ('fullpath'));
  mex_dir = fullfile (apa_dir, 'mex');


  old_dir = cd (mex_dir);

  cflags = {'--std=c99', '-Wall', '-Wextra', '-I.'};
  ldflags = {'libmpfr.a', 'libgmp.a'};

  if (exist('OCTAVE_VERSION', 'builtin') == 5)
    mex (cflags{:}, 'mpfr_interface.c', ldflags{:});
  else
    mex (['CFLAGS="$CFLAGS ', strjoin(cflags, ' '), '"'], ...
        'mpfr_interface.c', ldflags{:});
  end

  cd (old_dir);

  add_to_path_if_not_exists (apa_dir);
  add_to_path_if_not_exists (mex_dir);

  disp ('APA is ready to use.');

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

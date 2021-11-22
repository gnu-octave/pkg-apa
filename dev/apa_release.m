function apa_release (varargin)
% APA_RELEASE creates a release zip-archive.
%
% Called with an argument: re-run pre-release tasks.
%

[apa_dev_dir, ~, ~] = fileparts (mfilename ('fullpath'));
old_dir = cd (fullfile (apa_dev_dir, '..'));
apa_dir = pwd ();

% Re-run pre-release tasks.
if (nargin)
  cd (fullfile (apa_dir, 'inst', 'mex'));
  run ('generate_m_files');
  system ('make format');

  % Check for uncommitted changes
  [~, cmdout] = system('git status --porcelain');
  if (~isempty (cmdout))
    error ('APA repo has uncommitted changes:\n\n%s\n\n', cmdout);
  end
end

% Grab version.
apa_ver = fileread (fullfile (apa_dir, 'DESCRIPTION'));
apa_ver = regexp (apa_ver, 'version: ([^\n]*)', 'tokens');
apa_ver = apa_ver{1};
apa_ver = apa_ver{1};
apa_zip_dir = ['apa-', apa_ver];
apa_zip = [apa_zip_dir, '.zip'];

% Create release zip-archive.
cd (apa_dir);
system (sprintf ('git archive --format=zip --prefix=%s/ --output %s HEAD\n', ...
  apa_zip_dir, fullfile (apa_dev_dir, apa_zip)));

% Patch zip-archive.
cd (apa_dev_dir);
unzip (apa_zip);

% Delete unpatched zip-archive.
delete (apa_zip);

% Patch zip-archive content.
cd (apa_zip_dir);

% Remove .gitignore
delete ('.gitignore');

% Add static libraries.
src_dir = fullfile (apa_dir, 'inst', 'mex');
dst_dir = fullfile (apa_dev_dir, apa_zip_dir, 'inst', 'mex');
for i = {'macos', 'mswin', 'unix'}
  copyfile (fullfile (src_dir, i{1}), (fullfile (dst_dir, i{1})));
end

% Recreate zip-archive.
cd (apa_dev_dir);
zip (apa_zip, apa_zip_dir);
rmdir (apa_zip_dir, 's');

cd (old_dir);

end
function generate_m_files ()
% This function automatically (re-)generates the m-files corresponding to
% "mex_mpfr_interface.c".

  % Parse "mpfr.info" for help strings.
  help_strings = parse_mpfr_info_file ();
  help_strings_used = false (size (help_strings, 1), 1);
  help_strings_extra = get_help_strings_extra ();
  help_strings_extra_used = false (size (help_strings_extra, 1), 1);

  show_error = @(i, s) error (['generate_m_files: failed on input i = %d:', ...
    '\n\n%s\n\n'], i, s);

  str = fileread ('mex_mpfr_interface.c');
  matches = regexp (str, ' case.*?\n', 'match');
  for i = 1:length (matches)
    str = matches{i};
    [tok, str] = strtok (str, ' ');  % read 'case'
    if (~ strcmp (tok, 'case'))
      show_error (i, matches{i});
    end
    [tok, str] = strtok (str, ':');  % read function number
    fcn.number = str2num (tok);
    [tok, str] = strtok (str, ' ');  % read ':'
    if (~ strcmp (tok, ':'))
      show_error (i, matches{i});
    end
    [tok, str] = strtok (str, '//');  % read 'whitespace'
    if (~ all (tok == ' '))
      show_error (i, matches{i});
    end
    [tok, str] = strtok (str, ' ');  % read '//'
    if (~ strcmp (tok, '//'))
      show_error (i, matches{i});
    end

    % Read C function definition
    [fcn.name, in_args] = strtok (str, '(');
    fcn.name = strsplit (strtrim (fcn.name));
    fcn.out_arg = strjoin (fcn.name(1:end-1), ' ');
    fcn.name = fcn.name{end};
    in_args = strtrim (in_args);
    in_args = in_args(2:end-1);  % Remove braces '()' around input args.
    in_args = strsplit (in_args, ', ');
    for j = 1:length (in_args)
      arg = strsplit (in_args{j});
      fcn.in_args(j).type = strjoin (arg(1:end-1), ' ');
      fcn.in_args(j).name = arg{end};
      % Handle trailing C-array brackets '[]' in variable names (sum, dot).
      if ((length (fcn.in_args(j).name) > 2) ...
          && strcmp (fcn.in_args(j).name(end-1:end), '[]'))
        fcn.in_args(j).name = fcn.in_args(j).name(1:end-2);
      end
      % Handle leading C-pointer stars '*' in variable names (mpfr_set_str).
      if ((~isempty (fcn.in_args(j).name)) && (fcn.in_args(j).name(1) == '*'))
        fcn.in_args(j).name = fcn.in_args(j).name(2:end);
      end
    end

    fprintf (' %3d/%3d: (%4d) %s\n', i, length (matches), fcn.number, fcn.name);

    % Those functions are called by the mpfr_t class.
    if (fcn.number >= 1900)
      disp ('                 skipped');
      continue;
    end

    % Patch changes to C function definition
    switch (fcn.number)
      case 1021
        % char * mpfr_get_str (char *str, mpfr_exp_t *expptr, int base,
        %                      size_t n, mpfr_t op, mpfr_rnd_t rnd)
        % [significant, expptr] = mpfr_get_str (base, n, op, rnd)
        fcn.in_args(1:2) = [];
        fcn.out_arg = '[significant, expptr] = ';
      case 1124
        % int mpfr_lgamma (mpfr_t rop, int *signp, mpfr_t op, mpfr_rnd_t rnd)
        % [ret, signp] = mpfr_lgamma (rop, op, rnd)
        fcn.in_args(2) = [];
        fcn.out_arg = '[ret, signp] = ';
      case {1157, 1159}
        % int mpfr_fmodquo (mpfr_t r, long* q, mpfr_t x, mpfr_t y, mpfr_rnd_t rnd)
        % [ret, q] = mpfr_fmodquo (r, x, y, rnd)
        % int mpfr_remquo (mpfr_t r, long* q, mpfr_t x, mpfr_t y, mpfr_rnd_t rnd)
        % [ret, q] = mpfr_remquo (r, x, y, rnd)
        fcn.in_args(2) = [];
        fcn.out_arg = '[ret, q] = ';
      case 1217
        % int mpfr_strtofr (mpfr_t rop, const char *nptr, char **endptr,
        %                   int base, mpfr_rnd_t rnd)
        % [ret, endptr] = mpfr_strtofr (rop, nptr, base, rnd)
        fcn.in_args(2).name = 'nptr';
        fcn.in_args(3) = [];
        fcn.out_arg = '[ret, endptr] = ';
    end

    % Generate Octave/Matlab function
    if (fcn.out_arg(1) == '[')
      fcn_str_ret = fcn.out_arg;  % already patched
    elseif (~ strcmp (fcn.out_arg, 'void'))
      fcn_str_ret = 'ret = ';
    else
      fcn_str_ret = '';
    end
    if ((length (fcn.in_args) == 1) && strcmp (fcn.in_args(1).name, 'void'))
      fcn_str_args = '';
    else
      fcn_str_args = strjoin({fcn.in_args.name}, ', ');
    end
    if (isempty (fcn_str_args))
      fcn_str_args2 = num2str(fcn.number);
    else
      fcn_str_args2 = strjoin({num2str(fcn.number), fcn_str_args}, ', ');
    end

    % Write function signature.
    fcn_str = ['function ', fcn_str_ret, fcn.name, ' (', fcn_str_args, ')\n'];

    % Write help text.
    fcn_str = [fcn_str, '%% ', fcn_str_ret, fcn.name, ' (', fcn_str_args, ')\n'];
    fcn_str = [fcn_str, '%%\n'];
    help_strings_idx = strcmp (help_strings(:,1), fcn.name);
    help_strings_extra_idx = strcmp (help_strings_extra(:,1), fcn.name);
    if (any (help_strings_idx))
      idx = find(help_strings_idx,1);
      help_strings_used(idx) = true;
      fcn_str = [fcn_str, help_strings{idx,2}, '\n\n'];
    elseif (any (help_strings_extra_idx))
      idx = find(help_strings_extra_idx,1);
      help_strings_extra_used(idx) = true;
      fcn_str = [fcn_str, help_strings_extra{idx,2}, '\n\n'];
    else
      disp ('                 no help text');
    end

    % Write mpfr_t conversion code.
    for j = 1:length (fcn.in_args)
      if (strcmp (fcn.in_args(j).type, 'mpfr_t'))
        var = fcn.in_args(j).name;
        fcn_str = [fcn_str, '  if (isa (', var, ', ''mpfr_t''))\n'];
        fcn_str = [fcn_str, '    ', var, ' = ', var, '.idx;\n'];
        fcn_str = [fcn_str, '  end\n'];
      end
    end

    % Write mex_apa_interface function call.
    fcn_str = [fcn_str, '  ', fcn_str_ret, 'mex_apa_interface (', ...
                              fcn_str_args2, ');\n'];

    % Write EOF.
    fcn_str = [fcn_str, 'end\n\n'];
    fcn_str = [fcn_str, '%% This function was automatically generated by ', ...
      '"generate_m_files".\n'];

    % Write out the generated m-file.
    fp = fopen (fullfile ('..', [fcn.name, '.m']), 'w');
    fprintf (fp, fcn_str);
    fclose (fp);

    clear fcn;
  end
  fprintf ('\n\nUnimplemented documented MPFR functions (%d of %d):\n\n', ...
    sum (~help_strings_used), length (help_strings_used));
  disp (help_strings(~help_strings_used,1));
end


function help_strings_extra = get_help_strings_extra ()
  MPFR_RND_COMMON = '%% Return internal numerical value for the rounding mode';
  help_strings_extra(1,:) = {'MPFR_RNDN', [MPFR_RND_COMMON, ...
    '\n%% "round to nearest", with the even rounding rule (roundTiesToEven ', ...
    'in IEEE 754-2008).']};
  help_strings_extra(2,:) = {'MPFR_RNDZ', [MPFR_RND_COMMON, ...
    '\n%% "round toward minus infinity" (roundTowardNegative in IEEE ', ...
    '754-2008).']};
  help_strings_extra(3,:) = {'MPFR_RNDU', [MPFR_RND_COMMON, ...
    '\n%% "round toward plus infinity" (roundTowardPositive in IEEE ', ...
    '754-2008).']};
  help_strings_extra(4,:) = {'MPFR_RNDD', [MPFR_RND_COMMON, ...
    '\n%% "round toward zero" (roundTowardZero in IEEE 754-2008).']};
  help_strings_extra(5,:) = {'MPFR_RNDA', [MPFR_RND_COMMON, ...
    '\n%% "round away from zero".']};
end


function help_strings = parse_mpfr_info_file ()
  str = read_file_to_cellstr ('mpfr.info');
  idx_start = find (strcmp (str, '5.1 Initialization Functions'));
  idx_end = find (strcmp (str, '5.17 Internals'));
  str = str(idx_start:idx_end);

  help_strings = cell (0, 2);
  item_stack = {};
  text_stack = {};
  state = 'start';
  for i = 1:length (str)
    line = str{i};

    % Read new item, e.g. ' -- Function: '.
    if (strncmp (line, ' -- ', 4))
      % Flush stack if necessary.
      if (strcmp (state, 'read_item_help'))
        [help_strings, item_stack, text_stack] = parse_flush_stack ( ...
         help_strings, item_stack, text_stack);
      end
      state = 'new_item';
      item = strsplit (line, ':');
      item = strsplit (strtrim (item{2}), '(');
      item = strsplit (strtrim (item{1}), ' ');
      item_stack{end + 1} = item{end};
      continue;
    end

    % Ignore new item continuation lines.
    if (strncmp (line, '          ', 10))
      continue;
    end

    % Begin or continue reading the help string.
    if ((strcmp (state, 'new_item') || strcmp (state, 'read_item_help')) ...
        && ((isempty (line)) || strncmp (line, '     ', 5)))
      state = 'read_item_help';
      % Avoid double blank lines.
      if ((isempty (line)) && ((isempty (text_stack)) ...
                               || ~strcmp (text_stack{end}, '%%')))
        text_stack{end + 1} = '%%';
      else
        text_stack{end + 1} = ['%% ', strtrim(line)];
      end
      continue;
    end

    % Nothing suitable found.  Flush stack if necessary.
    state = 'start';
    [help_strings, item_stack, text_stack] = parse_flush_stack ( ...
     help_strings, item_stack, text_stack);
  end
end


function [help_strings, item_stack, text_stack] = parse_flush_stack ( ...
  help_strings, item_stack, text_stack)
% Helper function for info-file parser.
  if (~isempty (item_stack))
    text_stack = strjoin (text_stack, '\n');
    N = size (help_strings, 1);
    idx = 1:length (item_stack);
    help_strings(N + idx, 1) = item_stack(idx);
    help_strings(N + idx, 2) = {text_stack};
    item_stack = {};
    text_stack = {};
  end
end


function content = read_file_to_cellstr (file)
% Reads a given file line by line into a cellstring `content`.
% `fileread` in Octave removes blank lines.

  fid = fopen (file, 'r');
  i = 1;
  content{i} = fgetl (fid);
  while (ischar (content{i}))
    i = i + 1;
    content{i} = fgetl (fid);
  end
  fclose (fid);
  content = content(1:end-1);  % No EOL
end

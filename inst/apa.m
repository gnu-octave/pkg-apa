function settings = apa (key, val)
  % Set or get APA settings.
  %
  % 'verbose' (integer scalar):
  %
  %   0 : no output at all (including no error messages)
  %   1 : show error messages
  %  [2]: show error messages and precision warnings [default]
  %   3 : very verbose debug output.
  %
  %  'format.fmt' (string): ['fixed-point'], 'scientific'
  %  'format.base'          (integer scalar): 2 ... [10] ... 36
  %  'format.inner_padding' (integer scalar): positive
  %  'format.break_at_col'  (integer scalar): positive
  %
  % Use 'clear apa' to reset to default values.
  %

  persistent m_settings;

  if (isempty (m_settings))
    m_settings = apa_defaults ();
  end

  % Update settings which could be altered outside this function.
  m_settings.verbose = mex_apa_interface (9001);

  switch (nargin) 
    case 0  % Get all mode.
      settings = m_settings;
    case 1
      % Set whole struct mode.
      if (isstruct (key) && validate_apa_struct (key) && apply_apa_struct (key))
        m_settings = key;
      elseif (ischar (key)) % Get mode.
        try
          settings = eval (sprintf ('m_settings.%s', key));
        catch
          error ('apa:badInput', 'apa: setting "%s" does not exist', key);
        end
      else
        error ('apa:badInput', 'apa: invalid input argument');
      end
    case 2  % Set mode.
      if (~ischar (key))
        error ('apa:badInput', 'apa: first argument must be a string');
      end

      new_settings = m_settings;
      fnames = strsplit (key, '.');
      types = repmat ({'.'}, 1, length (fnames));
      S = cell2struct ([types; fnames], {'type', 'subs'}, 1);
      try
        subsref (new_settings, S);
      catch
        error ('apa:badInput', 'apa: setting "%s" does not exist', key);
      end
      new_settings = subsasgn (new_settings, S, val);

      if (validate_apa_struct (new_settings) && apply_apa_struct (new_settings))
        m_settings = new_settings;
      end
    otherwise
      error ('apa:badInput', 'apa: invalid number of input parameters');
  end
  
end


function settings = apa_defaults ()
  % Get default APA settings.

  settings.verbose = mex_apa_interface (9001);

  settings.format.fmt = 'fixed-point';
  settings.format.base = 10;
  settings.format.inner_padding = 3;
  settings.format.break_at_col = 80;
end


function bool = validate_apa_struct (s)
  fnames = fieldnames (s);

  % Check for missing fields.
  if (length (fnames) > 2)
    error ('apa:badInput', 'apa: struct has too many fields');
  end

  for f = {'verbose', 'format'}
    if (~ any (strcmp (fnames, f{1})))
      error ('apa:badInput', 'apa: setting "%s" is missing', f{1});
    end
  end

  fnames = fieldnames (s.format);
  for f = {'fmt', 'base', 'inner_padding', 'break_at_col'}
    if (~ any (strcmp (fnames, f{1})))
      error ('apa:badInput', 'apa: setting "format.%s" is missing', f{1});
    end
  end

  % Check individual fields.
  fval = s.verbose;
  if (~ (isnumeric (fval) && isscalar (fval) && (0 <= fval) && (fval <= 3)))
    error ('apa:badInput', 'apa: "verbose" invalid value {0,1,2,3}');
  end

  fval = s.format.fmt;
  if (~ (ischar (fval) && any (strcmp (fval, {'fixed-point', 'scientific'}))))
    error ('apa:badInput', ['apa: "format.fmt" must be "fixed-point" or ', ...
     '"scientific"']);
  end
  
  fval = s.format.base;
  if (~ (isnumeric (fval) && isscalar (fval) && (2 <= fval) && (fval <= 36)))
    error ('apa:badInput', 'apa: "format.base" invalid value (2 to 36)');
  end

  fval = s.format.inner_padding;
  if (~ (isnumeric (fval) && isscalar (fval) && (0 < fval)))
    error ('apa:badInput', ['apa: "format.inner_padding" must be a ', ...
      'positive scalar']);
  end

  fval = s.format.break_at_col;
  if (~ (isnumeric (fval) && isscalar (fval) && (0 < fval)))
    error ('apa:badInput', ['apa: "format.break_at_col" must be a ', ...
      'positive scalar']);
  end

  bool = true;
end


function bool = apply_apa_struct (s)
  try
    mex_apa_interface (9000, s.verbose);
  catch
    error ('apa:badInput', 'apa: "verbose" invalid value {0,1,2,3}');
  end
  bool = true;
end

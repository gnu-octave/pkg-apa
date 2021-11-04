function settings = apa (key, val)
  % Set or get APA settings.
  %
  % Use 'clear apa' to reset to default values.
  %
  
  persistent m_settings;
  
  if (isempty (m_settings))
    m_settings = apa_defaults ();
  end
  
  switch (nargin) 
    case 0  % Get all mode.
      settings = m_settings;
    case 1  % Get mode.
      try
        settings = eval (sprintf ('m_settings.%s', key));
      catch
        error ('apa:badInput', 'apa: setting "%s" does not exist', key);
      end
    case 2  % Set mode.
      try
        fnames = strsplit (key, '.');
        types = repmat ({'.'}, 1, length (fnames));
        S = cell2struct ([types; fnames], {'type', 'subs'}, 1);
        m_settings = subsasgn (m_settings, S, val);
      catch
        error ('apa:badInput', 'apa: setting "%s" does not exist', key);
      end
    otherwise
      error ('apa:badInput', 'apa: invalid number of input parameters');
  end
  
end


function settings = apa_defaults ()
  % Get default APA settings.

  settings.format.fmt = 'fixed-point';
  settings.format.base = 10;
  settings.format.inner_padding = 3;
  settings.format.break_at_col = 80;
end

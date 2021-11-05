function varargout = subsref (obj, s, rnd)
% Subscripted reference `c = obj (s)` using rounding mode `rnd`.

  if (strcmp (s(1).type, '()'))
    if (length (s.subs) > 2)
      error ('mpfr_t:subsref', ...
             'MPFR_T variables can only be indexed in two dimensions.');
    end

    if (nargin < 3)
      rnd = mpfr_get_default_rounding_mode ();
    end
    obj_numel = obj.idx(2) - obj.idx(1) + 1;

    % Analyze indices.
    if (length (s.subs) == 2)
      s_dims = obj.dims;  % assume `obj(:,:)`
    else
      s_dims = [obj_numel, 1];  % assume `obj(:)`
    end
    subs = s.subs;
    for i = 1:length (s.subs)
      max_dim = s_dims(i);
      % If not magic colon ':', override with length of accessed elements.
      if (isnumeric (s.subs{i}))  % Also empty matrix '[]' is numeric.
        s_dims(i) = length (s.subs{i});
        if ((s_dims(i) > 0) ...
            && ((min (s.subs{i}) < 1) || (max (s.subs{i}) > max_dim)))
          error ('mpfr_t:subsref', ['Invalid index range.  Valid' ...
                 'index range for dimension %d is [1 %d], ', ...
                 'but [%d %d] was requested'], ...
                 i, max_dim, min (s.subs{i}), max (s.subs{i}));
        end
        subs{i} = s.subs{i};
      elseif (ischar (s.subs{i}) && strcmp (s.subs{i}, ':'))
        subs{i} = 1:max_dim;  % Convert magic colon ':' into ranges.
      end
    end
    if (length (s.subs) == 2)
      columns = subs{2};
    else
      columns = 1;
      % Fix absolutely empty indexing `obj([])`.
      if (s_dims(1) == 0)
        s_dims(2) = 0;
      end
    end
    subs = subs{1};

    % Shortcut empty dimension.
    if (any (s_dims == 0))
      c = zeros (s_dims);
      ret = 0;

    % Shortcut ':' magic colon indexing.
    elseif (all (cellfun (@ischar, s.subs)) && all (strcmp (s.subs, ':')))
      c = mpfr_t (zeros (s_dims), max (obj.prec));
      ret = mpfr_set (c, obj, rnd);

    % 1D or 2D index, e.g. `obj(1:end)` or `obj(1:end,1:end)`.
    else
      c = mpfr_t (zeros (length (subs), length (columns)), max (obj.prec));
      k = 1;
      for j = columns  % Iterate over all columns.
        c_col_offset = (k - 1) * c.dims(1);
        o_col_offset = (j - 1) * obj.dims(1);

        % Avoid nested for-loop if indices are contiguous.
        if (isequal (subs, (subs(1):subs(end))))
          cidx =   c.idx(1) + [1,    length(subs)] - 1 + c_col_offset;
          oidx = obj.idx(1) + [subs(1), subs(end)] - 1 + o_col_offset;
          ret = mpfr_set (cidx, oidx, rnd);
        else
          ret = zeros (length (subs), 1);
          cidx = @(i)   c.idx(1) + [i, i] - 1 + c_col_offset;
          oidx = @(i) obj.idx(1) + [i, i] - 1 + o_col_offset;
          for i = 1:length (subs)
            ret(i) = mpfr_set (cidx(i), oidx(subs(i)), rnd);
          end
        end
        k = k + 1;
      end
    end
    obj.warnInexactOperation (ret);
    if ((length (s.subs) == 1) && isnumeric (s.subs{1}) && ~isempty (s.subs{1}))
      c.dims = size (s.subs{1});
    end
    varargout{1} = c;
  else
    % Permit things like function calls or attribute access.
    [varargout{1:nargout}] = builtin ('subsref', obj, s);
  end
end

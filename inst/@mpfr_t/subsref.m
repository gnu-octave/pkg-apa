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

    % Shortcut empty dimension.
    if (any (cellfun (@isempty, s.subs)))
      c = zeros (1, 2);
      for i = 1:2
        if (~ isempty (s.subs{i}))
          if (ischar (s.subs{i}) && strcmp (s.subs{i}, ':'))  % magic colon
            c(i) = obj.dims(i);
          else
            c(i) = length (s.subs{i});
          end
        end
      end
      ret = 0;
      c = zeros (c);

    % Shortcut ':' magic colon indexing.
    elseif (all (cellfun (@ischar, s.subs)) && all (strcmp (s.subs, ':')))
      if (length (s.subs) == 2)
        new_dims = obj.dims;
      else
        new_dims = [obj_numel, 1];
      end
      c = mpfr_t (zeros (new_dims), max (obj.prec));
      ret = mpfr_set (c, obj, rnd);

    % 1D or 2D index, e.g. `obj(1:end)` or `obj(1:end,1:end)`.
    else
      if (length (s.subs) == 2)
        max_dim = @(i) obj.dims(i);
      else
        max_dim = @(i) obj_numel;
      end
      for i = 1:length (s.subs)
        % Convert magic colon ':' into ranges.
        if (ischar (s.subs{i}) && strcmp (s.subs{i}, ':'))
          s.subs{i} = 1:max_dim(i);
        end
        % Check index range.
        if ((min (s.subs{i}) < 1) || (max (s.subs{i}) > max_dim(i)))
          error ('mpfr_t:subsref', ['Invalid index range. ' ...
            ' Valid index range is [1 %d], but [%d %d] was requested'], ...
            max_dim(i), min (s.subs{i}), max (s.subs{i}));
        end
      end
      if (length (s.subs) == 2)
        columns = s.subs{2};
      else
        columns = 1;
      end
      subs = s.subs{1};

      c = mpfr_t (zeros (length (subs), length (columns)), ...
                  max (obj.prec));
      k = 1;
      for j = columns  % Iterate over all columns.
        c_col_offset = (k - 1) * c.dims(1);
        o_col_offset = (j - 1) * obj.dims(1);
        %keyboard ();
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
            ret(i) = mpfr_set (cidx(i), oidx(i), rnd);
          end
        end
        k = k + 1;
      end
    end
    obj.warnInexactOperation (ret);
    varargout{1} = c;
  else
    % Permit things like function calls or attribute access.
    [varargout{1:nargout}] = builtin ('subsref', obj, s);
  end
end

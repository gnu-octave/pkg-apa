function obj = subsasgn (obj, s, b, rnd)
% Subscripted assignment `obj(s) = b` using rounding mode `rnd`.

  if (strcmp (s(1).type, '()'))
    if (length (s.subs) > 2)
      error ('mpfr_t:subsref', ...
             'MPFR_T variables can only be indexed in two dimensions.');
    end

    if (nargin < 4)
      rnd = mpfr_get_default_rounding_mode ();
    end
    obj_numel = obj.idx(2) - obj.idx(1) + 1;

    % Analyze indices.
    if (length (s.subs) == 2)
      s_dims = obj.dims;  % assume `obj(:,:) = b`
    else
      s_dims = [obj_numel, 1];  % assume `obj(:) = b`
    end
    subs = s.subs;
    for i = 1:length (s.subs)
      max_dim = s_dims(i);
      % If not magic colon ':', override with length of accessed elements.
      if (isnumeric (s.subs{i}))  % Also empty matrix '[]' is numeric.
        s_dims(i) = length (s.subs{i});
        if ((s_dims(i) > 0) ...
            && ((min (s.subs{i}) < 1) || (max (s.subs{i}) > max_dim)))
          error ('mpfr_t:subsasgn', ['Invalid index range.  Valid' ...
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

    % Analyze `b`.
    if (isa (b, 'mpfr_t'))
      b_dims = b.dims;
    elseif (isnumeric (b))
      b_dims = size (b);
    elseif (iscellstr (b) || ischar (b))
      if (ischar (b))
        b = {b};
      end
      b_dims = size (b);
    else
      error ('mpfr_t:subsasgn', 'Input must be numeric, string, or mpfr_t.');
    end

    % Print error message if `b` is not scalar and dimensions do not match.
    if ((~ all (b_dims == [1, 1])) && any (s_dims ~= b_dims))
      error ('mpfr_t:subsasgn', ...
             'nonconformant arguments (op1 is %dx%d, op2 is %dx%d', ...
             s_dims(1), s_dims(2), b_dims(1), b_dims(2));
    end

    % Shortcut empty dimension.
    if (any (s_dims == 0))
      return;  % Nothing to be done here.
    end

    % Shortcut ':' magic colon indexing.
    if (all (cellfun (@ischar, s.subs)) && all (strcmp (s.subs, ':')))
      if (isa (b, 'mpfr_t'))
        ret = mpfr_set (obj, b, rnd);
      elseif (isnumeric (b))
        ret = mpfr_set_d (obj, b(:), rnd);
      elseif (iscellstr (b))
        [ret, strpos] = mpfr_strtofr (obj, b(:), 0, rnd);
        bad_strs = (cellfun (@numel, b(:)) >= strpos);
        if (any (bad_strs))
          warning ('mpfr_t:bad_conversion', ...
                   'Conversion of %d value(s) failed due to bad input.', ...
                   sum (bad_strs));
        end
      end

    % 1D or 2D index, e.g. `obj(1:end) = b` or `obj(1:end,1:end) = b`.
    else
      ret = zeros (s_dims);
      k = 1;
      for j = columns  % Iterate over all columns.
        b_col_offset = (k - 1) * b_dims(1);
        o_col_offset = (j - 1) * obj.dims(1);

        % Avoid nested for-loop if indices are contiguous.
        if (isequal (subs, (subs(1):subs(end))))
          oidx = obj.idx(1) + [subs(1), subs(end)] - 1 + o_col_offset;
          ridx = 1:length(subs);
          if (isa (b, 'mpfr_t'))
            if (diff (b.idx) == 0)  % b is mpfr_t scalar
              op = b;
            else
              op = b.idx(1) + ridx - 1 + b_col_offset;
            end
            ret(ridx,j) = mpfr_set (oidx, op, rnd);
          elseif (isnumeric (b))
            if (isscalar (b))
              op = @(i,j) b;
            else
              op = @(i,j) b(i,j);
            end
            ret(ridx,j) = mpfr_set_d (oidx, op(ridx,j), rnd);
          elseif (iscellstr (b))
            if (isscalar (b))
              op = @(i,j) b;
            else
              op = @(i,j) b(i,j);
            end
            [ret(ridx,j), strpos] = mpfr_strtofr (oidx, op(ridx,j), 0, rnd);
            bad_strs = (cellfun (@numel, b(ridx,j)) >= strpos);
            if (any (bad_strs))
              warning ('mpfr_t:bad_conversion', ...
                       'Conversion of %d value(s) failed due to bad input.', ...
                       sum (bad_strs));
            end
          end
        else
          oidx = @(i) obj.idx(1) + [i, i] - 1 + o_col_offset;
          if (isa (b, 'mpfr_t'))
            if (diff (b.idx) == 0)  % b is mpfr_t scalar
              op = @(i,j) b;
            else
              op = @(i,j) b.idx(1) + [i, i] - 1 + b_col_offset;
            end
            for i = 1:length (subs)
              ret(i,j) = mpfr_set (oidx(i), op(i), rnd);
            end
          elseif (isnumeric (b))
            if (isscalar (b))
              op = @(i,j) b;
            else
              op = @(i,j) b(i,j);
            end
            ret(i,j) = mpfr_set_d (obj, op(i,j), rnd);
          elseif (iscellstr (b))
            if (isscalar (b))
              op = @(i,j) b;
            else
              op = @(i,j) b(i,j);
            end
            [ret(i,j), strpos] = mpfr_strtofr (obj, op(i,j), 0, rnd);
            bad_strs = (cellfun (@numel, op(i,j)) >= strpos);
            if (any (bad_strs))
              warning ('mpfr_t:bad_conversion', ...
                       'Conversion of %d value(s) failed due to bad input.', ...
                       sum (bad_strs));
            end
          end
        end
        k = k + 1;
      end
    end
    obj.warnInexactOperation (ret);
  else
    % Permit things like assignment to attributes.
    obj = builtin ('subsasgn', obj, s, b);
  end
end

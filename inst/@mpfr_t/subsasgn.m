function obj = subsasgn (obj, s, b, rnd)
  % Subscripted assignment `obj(s) = b` using rounding mode `rnd`.
  if (strcmp (s(1).type, '()'))
    if (nargin < 4)
      rnd = mpfr_get_default_rounding_mode ();
    end
    subs = s.subs{:};
    obj_numel = obj.idx(2) - obj.idx(1) + 1;

    if (isnumeric (subs) ...
        && ((min (subs) < 1) || (max (subs) > obj_numel)))
      error ('mpfr_t:subsasgn', ['Invalid index range. ' ...
        ' Valid index range is [1 %d], but [%d %d] was requested'], ...
        obj_numel, min (subs), max (subs));
    end
    % Avoid for-loop if ':' magic colon or indices are contiguous.
    if ((ischar (subs) && strcmp (subs, ':')) ...
        || (isequal (subs, (subs(1):subs(end)))))
      if (ischar (subs))  % ':' magic colon
        rop = obj;
      else
        rop = obj.idx(1) + [subs(1), subs(end)] - 1;
      end
      if (isa (b, 'mpfr_t'))
        ret = mpfr_set (rop, b, rnd);
      elseif (isnumeric (b))
        ret = mpfr_set_d (rop, b(:), rnd);
      elseif (iscellstr (b) || ischar (b))
        if (ischar (b))
          b = {b};
        end
        [ret, strpos] = mpfr_strtofr (rop, b(:), 0, rnd);
        bad_strs = (cellfun (@numel, b(:)) >= strpos);
        if (any (bad_strs))
          warning ('mpfr_t:bad_conversion', ...
                   'Conversion of %d value(s) failed due to bad input.', ...
                   sum (bad_strs));
        end
      else
        error ('mpfr_t:subsasgn', ...
               'Input must be numeric, string, or mpfr_t.');
      end
    else
      rop = @(i) obj.idx(1) + [i, i] - 1;
      ret = zeros (length (subs), 1);
      if (isa (b, 'mpfr_t'))
        if (diff (b.idx) == 0)  % b is scalar mpfr_t.
          op = @(i) b;
        else
          op = @(i) b.idx(1) + [i, i] - 1;
        end
        for i = 1:length (subs)
          ret(i) = mpfr_set (rop(i), op(i), rnd);
        end
      elseif (isnumeric (b))
        if (isscalar (b))
          op = @(i) b;
        else
          op = @(i) b(i);
        end
        for i = 1:length (subs)
          ret(i) = mpfr_set_d (rop(i), op(i), rnd);
        end
      elseif (iscellstr (b) || ischar (b))
        if (ischar (b))
          b = {b};
        end
        if (isscalar (b))
          op = @(i) b;
        else
          op = @(i) b(i);
        end
        bad_strs = 0;
        for i = 1:length (subs)
          [ret(i), strpos] = mpfr_strtofr (rop(i), op(i), 0, rnd);
          if (numel (op(i)) >= strpos)
            bad_strs = bad_strs + 1;
          end
        end
        if (any (bad_strs))
          warning ('mpfr_t:bad_conversion', ...
                   'Conversion of %d value(s) failed due to bad input.', ...
                   sum (bad_strs));
        end
      else
        error ('mpfr_t:mpfr_t', ...
               'Input must be numeric, string, or mpfr_t.');
      end
    end
    obj.warnInexactOperation (ret);
  else
    % Permit things like assignment to attributes.
    obj = builtin ('subsasgn', obj, s, b);
  end
end

classdef mpfr_t

  properties (SetAccess = protected)
    dims  % Original object dimensions.
    idx   % MPFR variable indices.
  end


  methods (Static)
    function num = get_data_capacity ()
      % [internal] Return the number of pre-allocated MPFR variables.

      num = gmp_mpfr_interface (9000);
    end


    function num = get_data_size ()
      % [internal] Return the number of currently used MPFR variables.

      num = gmp_mpfr_interface (9001);
    end


    function set_verbose (level)
      % [internal] Set the output verbosity `level` of the GMP MPFR interface.
      % - level = 0: no output at all (including no error messages)
      % - level = 1: show error messages
      % - level = 2: show error messages and precision warnings [default]
      % - level = 3: very verbose debug output.

      gmp_mpfr_interface (9002, level);
    end


    function level = get_verbose ()
      % [internal] Get the output verbosity `level` of the GMP MPFR interface.
      % See also `mpfr_t.set_verbose`.

      level = gmp_mpfr_interface (9003);
    end


    function idx = allocate (count)
      % [internal] Return the start and end index of a newly created MPFR
      % variable for `count` elements.

      idx = gmp_mpfr_interface (9004, count);
    end
  end


  methods (Access = private)
    function warnInexactOperation (~, ret)
      if (any (ret))
        warning ('mpfr_t:inexactOperation', ...
                 'Conversion of %d value(s) was inexact.', ...
                 sum (ret ~= 0));
      end
    end
  end


  methods (Static, Access = private)
    function c = call_comparison_op (a, b, op)
      if (isa (a, 'mpfr_t'))
        new_dims = a.dims;
      else
        a = mpfr_t (a);
        new_dims = b.dims;
      end
      if (~isa (b, 'mpfr_t'))
         b = mpfr_t (b);
      end
      c = (op (a, b) ~= 0);
      c = reshape (c, new_dims);
    end
  end


  methods
    function obj = mpfr_t (x, prec, rnd)
      % Construct a mpfr_t variable of precision `prec` from `x` using rounding
      % mode `rnd`.

      if (nargin < 1)
        error ('mpfr_t:mpfr_t', 'At least one argument must be provided.');
      end

      if (~ismatrix (x))
        error ('mpfr_t:mpfr_t', ...
          'Only two dimensional matrix input is supported.');
      end

      if (nargin < 2)
        prec = mpfr_get_default_prec ();
      end
      if (nargin < 3)
        rnd = mpfr_get_default_rounding_mode ();
      end

      if (isa (x, 'mpfr_t'))
        obj.dims = x.dims;
      else
        obj.dims = size (x);
      end
      num_elems = prod (obj.dims);
      obj.idx = mpfr_t.allocate (num_elems)';
      mpfr_set_prec (obj, prec);
      s.type = '()';
      s.subs = {':'};
      obj.subsasgn (s, x, rnd);
    end


    function prec = prec (obj)
      % Return the precision of obj, i.e., the number of bits used to store
      % its significant.

      prec = mpfr_get_prec (obj);
    end


    function d = double (obj, rnd)
      % Return a double approximation of `obj` using rounding mode `rnd`.

      if (nargin < 2)
        rnd = mpfr_get_default_rounding_mode ();
      end
      d = reshape (mpfr_get_d (obj, rnd), obj.dims(1), obj.dims(2));
    end


    % More information about class methods.
    % https://octave.org/doc/v6.3.0/Operator-Overloading.html
    % https://www.mathworks.com/help/matlab/matlab_oop/implementing-operators-for-your-class.html
    % https://www.mathworks.com/help/matlab/matlab_oop/methods-that-modify-default-behavior.html


    function disp (obj)
      % Object display.

      if (isscalar (obj))
        if (prod (obj.dims) == 1)
          dim_str = 'scalar';
        else
          dim_str = sprintf ('%dx%d matrix', obj.dims(1), obj.dims(2));
        end
        prec = obj.prec;
        prec = [min(prec), max(prec)];
        if (prec(1) == prec(2))
          prec_str = num2str (prec(1));
        else
          prec_str = sprintf ('between %d and %d', prec(1), prec(2));
        end
        fprintf (1, '  MPFR %s (precision %s binary digits)\n\n', dim_str, ...
                                                                  prec_str);
        if (prod (obj.dims) == 1)
          fprintf (1, '  Double approximation: %f\n', double (obj));
        elseif ((obj.dims(1) <= 5) && (obj.dims(2) <= 5))
          fprintf (1, '  Double approximation:\n\n');
          disp (double (obj));
        end
      else
        builtin ('disp', obj);
      end
    end


    function c = plus (a, b, rnd, prec)
      % Binary addition `c = a + b` using rounding mode `rnd`.
      %
      % If no rounding mode `rnd` is given, the default rounding mode is used.
      %
      % If no precision `prec` is given for `c` the maximum precision of a and
      % is used b.

      if (nargin < 3)
        rnd = mpfr_get_default_rounding_mode ();
      end
      if (nargin < 4)
        prec = [];
      end
      if (isnumeric (a))
        c = plus (b, a, rnd, prec);
      elseif (isa (a, 'mpfr_t') && isnumeric (b))
        if (isscalar (b) || isequal (a.dims, size (b)))
          if (isempty (prec))
            prec = max (mpfr_get_prec (a));
          end
          cc = mpfr_t (zeros (a.dims), prec);
        else
          error ('mpfr_t:plus', 'Incompatible dimensions of a and b.');
        end
        mpfr_add_d (cc, a, double (b(:)), mpfr_get_default_rounding_mode ());
        c = cc;  % Do not assign c before calculation succeeded!
      elseif (isa (a, 'mpfr_t') && isa (b, 'mpfr_t'))
        if (isempty (prec))
          prec = max (max (mpfr_get_prec (a)), max (mpfr_get_prec (b)));
        end
        if (isequal (a.dims, b.dims) || isequal (b.dims, [1 1]))
          cc = mpfr_t (zeros (a.dims), prec);
        elseif (isequal (a.dims, [1 1]))
          cc = mpfr_t (zeros (b.dims), prec);
        else
          error ('mpfr_t:plus', 'Incompatible dimensions of a and b.');
        end
        mpfr_add (cc, a, b, rnd);
        c = cc;  % Do not assign c before calculation succeeded!
      else
        error ('mpfr_t:plus', 'Invalid operands a and b.');
      end
    end


    function c = minus (a, b, rnd, prec)
      % Binary subtraction `c = a - b` using rounding mode `rnd`.
      %
      % If no rounding mode `rnd` is given, the default rounding mode is used.
      %
      % If no precision `prec` is given for `c` the maximum precision of a and
      % is used b.

      if (nargin < 3)
        rnd = mpfr_get_default_rounding_mode ();
      end
      if (nargin < 4)
        prec = [];
      end
      if (isnumeric (a) && isa (b, 'mpfr_t'))
        if (isscalar (a) || isequal (size (a), b.dims))
          if (isempty (prec))
            prec = max (mpfr_get_prec (b));
          end
          cc = mpfr_t (zeros (b.dims), prec);
        else
          error ('mpfr_t:minus', 'Incompatible dimensions of a and b.');
        end
        mpfr_d_sub (cc, double (a(:)), b, rnd);
        c = cc;  % Do not assign c before calculation succeeded!
      elseif (isa (a, 'mpfr_t') && isnumeric (b))
        if (isscalar (b) || isequal (a.dims, size (b)))
          if (isempty (prec))
            prec = max (mpfr_get_prec (a));
          end
          cc = mpfr_t (zeros (a.dims), prec);
        else
          error ('mpfr_t:minus', 'Incompatible dimensions of a and b.');
        end
        mpfr_sub_d (cc, a, double (b(:)), rnd);
        c = cc;  % Do not assign c before calculation succeeded!
      elseif (isa (a, 'mpfr_t') && isa (b, 'mpfr_t'))
        if (isempty (prec))
          prec = max (max (mpfr_get_prec (a)), max (mpfr_get_prec (b)));
        end
        if (isequal (a.dims, b.dims) || isequal (b.dims, [1 1]))
          cc = mpfr_t (zeros (a.dims), prec);
        elseif (isequal (a.dims, [1 1]))
          cc = mpfr_t (zeros (b.dims), prec);
        else
          error ('mpfr_t:minus', 'Incompatible dimensions of a and b.');
        end
        mpfr_sub (cc, a, b, rnd);
        c = cc;  % Do not assign c before calculation succeeded!
      else
        error ('mpfr_t:minus', 'Invalid operands a and b.');
      end
    end


    function c = uminus (a)
      % Unary minus `c = -a`

      c = minus (0, a);
    end


    function c = uplus (a)
      % Unary plus `c = +a`

      c = a;
    end


    function c = times (a, b, rnd, prec)
      % Element-wise multiplication `c = a .* b` using rounding mode `rnd`.
      %
      % If no rounding mode `rnd` is given, the default rounding mode is used.
      %
      % If no precision `prec` is given for `c` the maximum precision of a and
      % is used b.

      if (nargin < 3)
        rnd = mpfr_get_default_rounding_mode ();
      end
      if (nargin < 4)
        prec = [];
      end
      if (isnumeric (a))
        c = times (b, a);
      elseif (isa (a, 'mpfr_t') && isnumeric (b))
        if (isscalar (b) || isequal (a.dims, size (b)))
          if (isempty (prec))
            prec = max (mpfr_get_prec (a));
          end
          cc = mpfr_t (zeros (a.dims), prec);
        else
          error ('mpfr_t:times', 'Incompatible dimensions of a and b.');
        end
        mpfr_mul_d (cc, a, double (b(:)), rnd);
        c = cc;  % Do not assign c before calculation succeeded!
      elseif (isa (a, 'mpfr_t') && isa (b, 'mpfr_t'))
        if (isempty (prec))
          prec = max (max (mpfr_get_prec (a)), max (mpfr_get_prec (b)));
        end
        if (isequal (a.dims, b.dims) || isequal (b.dims, [1 1]))
          cc = mpfr_t (zeros (a.dims), prec);
        elseif (isequal (a.dims, [1 1]))
          cc = mpfr_t (zeros (b.dims), prec);
        else
          error ('mpfr_t:times', 'Incompatible dimensions of a and b.');
        end
        mpfr_mul (cc, a, b, rnd);
        c = cc;  % Do not assign c before calculation succeeded!
      else
        error ('mpfr_t:times', 'Invalid operands a and b.');
      end
    end


%    function mtimes(a,b)
%    % Matrix multiplication `a*b`
%    end


    function c = rdivide (a, b, rnd, prec)
      % Right element-wise division `c = a ./ b` using rounding mode `rnd`.
      %
      % If no rounding mode `rnd` is given, the default rounding mode is used.
      %
      % If no precision `prec` is given for `c` the maximum precision of a and
      % is used b.

      if (nargin < 3)
        rnd = mpfr_get_default_rounding_mode ();
      end
      if (nargin < 4)
        prec = [];
      end
      if (isnumeric (a) && isa (b, 'mpfr_t'))
        if (isscalar (a) || isequal (size (a), b.dims))
          if (isempty (prec))
            prec = max (mpfr_get_prec (b));
          end
          cc = mpfr_t (zeros (b.dims), prec);
        else
          error ('mpfr_t:rdivide', 'Incompatible dimensions of a and b.');
        end
        mpfr_d_div (cc, double (a(:)), b, rnd);
        c = cc;  % Do not assign c before calculation succeeded!
      elseif (isa (a, 'mpfr_t') && isnumeric (b))
        if (isscalar (b) || isequal (a.dims, size (b)))
          if (isempty (prec))
            prec = max (mpfr_get_prec (a));
          end
          cc = mpfr_t (zeros (a.dims), prec);
        else
          error ('mpfr_t:rdivide', 'Incompatible dimensions of a and b.');
        end
        mpfr_div_d (cc, a, double (b(:)), rnd);
        c = cc;  % Do not assign c before calculation succeeded!
      elseif (isa (a, 'mpfr_t') && isa (b, 'mpfr_t'))
        if (isempty (prec))
          prec = max (max (mpfr_get_prec (a)), max (mpfr_get_prec (b)));
        end
        if (isequal (a.dims, b.dims) || isequal (b.dims, [1 1]))
          cc = mpfr_t (zeros (a.dims), prec);
        elseif (isequal (a.dims, [1 1]))
          cc = mpfr_t (zeros (b.dims), prec);
        else
          error ('mpfr_t:rdivide', 'Incompatible dimensions of a and b.');
        end
        mpfr_div (cc, a, b, rnd);
        c = cc;  % Do not assign c before calculation succeeded!
      else
        error ('mpfr_t:rdivide', 'Invalid operands a and b.');
      end
    end


    function c = ldivide (a, b, varargin)
      % Left element-wise division `c = a .\ b` using rounding mode `rnd`.
      %
      % If no rounding mode `rnd` is given, the default rounding mode is used.
      %
      % If no precision `prec` is given for `c` the maximum precision of a and
      % is used b.

      c = rdivide (b, a, varargin{:});
    end


    function x = mrdivide (b, a, rnd, prec)
      % Right matrix division `x = B / A` using rounding mode `rnd`.
      %
      % Find x, such that x*A = B.
      %
      % If no rounding mode `rnd` is given, the default rounding mode is used.
      %
      % If no precision `prec` is given for `c` the maximum precision of a and
      % is used b.

      if (nargin < 3)
        rnd = mpfr_get_default_rounding_mode ();
      end
      if (nargin < 4)
        prec = [];
      end

      if ((isnumeric (a) && isscalar (a)) ...
          || (isa (a, 'mpfr_t') && (prod (a.dims) == 1)))
        x = rdivide (b, a, rnd, prec);
      else
        error ('mpfr_t:mrdivide', ...
          'Solving systems of linear equations is not yet supported.');
      end
    end


    function x = mldivide (a, b, rnd, prec)
      % Left matrix division `x = A \ B` using rounding mode `rnd`.
      %
      % Find x, such that A*x = B.
      %
      % If no rounding mode `rnd` is given, the default rounding mode is used.
      %
      % If no precision `prec` is given for `c` the maximum precision of a and
      % is used b.

      if (nargin < 3)
        rnd = mpfr_get_default_rounding_mode ();
      end
      if (nargin < 4)
        prec = [];
      end

      if ((isnumeric (a) && isscalar (a)) ...
          || (isa (a, 'mpfr_t') && (prod (a.dims) == 1)))
        x = rdivide (b, a, rnd, prec);
      else
        error ('mpfr_t:mrdivide', ...
          'Solving systems of linear equations is not yet supported.');
      end
    end


    function c = power (a, b, rnd, prec)
      % Element-wise power `c = a.^b` using rounding mode `rnd`.
      %
      % If no rounding mode `rnd` is given, the default rounding mode is used.
      %
      % If no precision `prec` is given for `c` the maximum precision of a and
      % is used b.

      if (nargin < 3)
        rnd = mpfr_get_default_rounding_mode ();
      end
      if (nargin < 4)
        prec = [];
      end
      if (isa (a, 'mpfr_t') && isnumeric (b))
        if (isscalar (b) || isequal (a.dims, size (b)))
          if (isempty (prec))
            prec = max (mpfr_get_prec (a));
          end
          cc = mpfr_t (zeros (a.dims), prec);
        else
          error ('mpfr_t:power', 'Incompatible dimensions of a and b.');
        end
        mpfr_pow_si (cc, a, double (b(:)), rnd);
        c = cc;  % Do not assign c before calculation succeeded!
      elseif (isa (a, 'mpfr_t') && isa (b, 'mpfr_t'))
        if (isempty (prec))
          prec = max (max (mpfr_get_prec (a)), max (mpfr_get_prec (b)));
        end
        if (isequal (a.dims, b.dims) || isequal (b.dims, [1 1]))
          cc = mpfr_t (zeros (a.dims), prec);
        elseif (isequal (a.dims, [1 1]))
          cc = mpfr_t (zeros (b.dims), prec);
        else
          error ('mpfr_t:power', 'Incompatible dimensions of a and b.');
        end
        mpfr_pow (cc, a, b, rnd);
        c = cc;  % Do not assign c before calculation succeeded!
      else
        error ('mpfr_t:power', 'Invalid operands a and b.');
      end
    end


    function c = mpower (a, b, rnd, prec)
      % Matrix power `c = A ^ B` using rounding mode `rnd`.
      %
      % If no rounding mode `rnd` is given, the default rounding mode is used.
      %
      % If no precision `prec` is given for `c` the maximum precision of a and
      % is used b.

      if (nargin < 3)
        rnd = mpfr_get_default_rounding_mode ();
      end
      if (nargin < 4)
        prec = [];
      end

      if (((isnumeric (a) && isscalar (a)) ...
           || (isa (a, 'mpfr_t') && (prod (a.dims) == 1))) ...
          && ((isnumeric (b) && isscalar (b)) ...
              || (isa (b, 'mpfr_t') && (prod (b.dims) == 1))))
        c = power (a, b, rnd, prec);
      else
        error ('mpfr_t:mrdivide', ...
          'Matrix power not yet supported.');
      end
    end


    function c = lt (a, b)
      % Less than `c = (a < b)`.

      c = mpfr_t.call_comparison_op (a, b, @mpfr_less_p);
    end


    function c = gt (a, b)
      % Greater than `a > b`.

      c = mpfr_t.call_comparison_op (a, b, @mpfr_greater_p);
    end


    function c = le (a, b)
      % Less than or equal to `a <= b`.

      c = mpfr_t.call_comparison_op (a, b, @mpfr_lessequal_p);
    end


    function c = ge (a, b)
      % Greater than or equal to `a >= b`.

      c = mpfr_t.call_comparison_op (a, b, @mpfr_greaterequal_p);
    end


    function c = ne (a, b)
      % Not equal to `a ~= b`.

      c = ~eq (a, b);
    end


    function c = eq (a, b)
      % Equality `a == b`.

      c = mpfr_t.call_comparison_op (a, b, @mpfr_equal_p);
    end


    function and (varargin)
      % Logical AND `a & b`.
      error ('mpfr_t:and', 'Logical AND not supported for MPFR_T variables.');
    end


    function or (varargin)
      % Logical OR `a | b`.
      error ('mpfr_t:or', 'Logical OR not supported for MPFR_T variables.');
    end


    function not (varargin)
      % Logical NOT `~a`.
      error ('mpfr_t:not', 'Logical NOT not supported for MPFR_T variables.');
    end


    function colon (varargin)
      % Colon operator `a:b` or `a:d:b`.
      error ('mpfr_t:colon', 'Colon not supported for MPFR_T variables.');
    end


%    function ctranspose(a)
%    % Complex conjugate transpose `a'`
%    end


%    function transpose(a)
%    % Matrix transpose a.'`
%    end


    function c = horzcat (a, b, varargin)
      %TODO Horizontal concatenation `c = [a, b]`.
      error ('mpfr_t:horzcat', ...
        ['Arrays of MPFR_T variables are not supported.  ', ...
         'Use cell arrays {a, b} instead.']);
    end


    function c = vertcat (a, b, varargin)
      %TODO Vertical concatenation `c = [a; b]`.
      error ('mpfr_t:vertcat', ...
        ['Arrays of MPFR_T variables are not supported.  ', ...
         'Use cell arrays {a; b} instead.']);
    end


    function varargout = subsref (obj, s, rnd)
      % Subscripted reference `c = obj (s)`  using rounding mode `rnd`.
      if (strcmp (s(1).type, '()'))
        if (nargin < 3)
          rnd = mpfr_get_default_rounding_mode ();
        end
        obj_numel = obj.idx(2) - obj.idx(1) + 1;

        % Shortcut empty dimension.
        if ((length (s.subs) <= 2) && any (cellfun (@isempty, s.subs)))
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
        elseif ((length (s.subs) <= 2) ...
            && all (cellfun (@ischar, s.subs)) && all (strcmp (s.subs, ':')))
          if (length (s.subs) == 2)
            new_dims = obj.dims;
          else
            new_dims = [obj_numel, 1];
          end
          c = mpfr_t (zeros (new_dims), max (obj.prec));
          ret = mpfr_set (c, obj, rnd);

        % 1D index, e.g. `obj(1:end)`
        elseif (length (s.subs) == 1)
          subs = s.subs{1};
          if ((min (subs) < 1) || (max (subs) > obj_numel))
            error ('mpfr_t:subsref', ['Invalid index range. ' ...
              ' Valid index range is [1 %d], but [%d %d] was requested'], ...
              obj_numel, min (subs), max (subs));
          end
          c = mpfr_t (zeros (1, length (subs)), max (obj.prec));
          % Avoid for-loop if indices are contiguous.
          if (isequal (subs, (subs(1):subs(end))))
            ret = mpfr_set (c, obj.idx(1) + [subs(1), subs(end)] - 1, rnd);
          else
            ret = zeros (length (subs), 1);
            cidx = @(i)   c.idx(1) + [i, i] - 1;
            oidx = @(i) obj.idx(1) + [i, i] - 1;
            for i = 1:length (subs)
              ret(i) = mpfr_set (cidx(i), oidx(i), rnd);
            end
          end

        % 2D index, e.g. `obj(1:end,1:end)`
        elseif (length (s.subs) == 2)
          subs = s.subs;
          for i = 1:2
            % Convert magic colon ':' into ranges.
            if (ischar (subs{i}) && strcmp (subs{i}, ':'))
              subs{i} = 1:obj.dims(i);
            end
            % Check index range.
            if ((min (subs{i}) < 1) || (max (subs{i}) > obj.dims(i)))
              error ('mpfr_t:subsref', ['Invalid index range. ' ...
                ' Valid index range is [1 %d], but [%d %d] was requested'], ...
                obj.dims(i), min (subs{i}), max (subs{i}));
            end
          end
          columns = subs{2};
          subs    = subs{1};

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
        else
          error ('mpfr_t:subsref', ...
                 'MPFR_T variables can only be indexed in two dimensions.');
        end
        obj.warnInexactOperation (ret);
        varargout{1} = c;
      else
        % Permit things like function calls or attribute access.
        [varargout{1:nargout}] = builtin ('subsref', obj, s);
      end
    end


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
          if (isa (b, 'mpfr_t'))
            if (diff (b.idx) == 0)  % b is scalar mpfr_t.
              op = @(i) b;
            else
              op = @(i) b.idx(1) + [i, i] - 1;
            end
            for i = 1:length (subs)
              ret = mpfr_set (rop(i), op(i), rnd);
            end
          elseif (isnumeric (b))
            if (isscalar (b))
              op = @(i) b;
            else
              op = @(i) b(i);
            end
            for i = 1:length (subs)
              ret = mpfr_set_d (rop(i), op(i), rnd);
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
            ret = zeros (1, length (subs));
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
            error ('mpfr_t:mpfr_t', 'Input must be numeric, string, or mpfr_t.');
          end
        end
        obj.warnInexactOperation (ret);
      else
        % Permit things like assignment to attributes.
        obj = builtin ('subsasgn', obj, s, b);
      end
    end


    function c = end (varargin)
      % Return last object index `obj(1:end)`.

      switch (varargin{3})
        case 1
          c = prod (varargin{1}.dims);
        case 2
          c = varargin{1}.dims(varargin{2});
        otherwise
          error ('mpfr_t:end', ...
                 'MPFR_T variables can only be indexed in two dimensions.');
      end
    end


    function subsindex (varargin)
      % Subscript index `c = b(a)`.

      error ('mpfr_t:vertcat', 'MPFR_T variables cannot be used as index.');
    end


    function b = sqrt (a, rnd, prec)
      % Square root `b = sqrt(a)` using rounding mode `rnd`.
      %
      % If no rounding mode `rnd` is given, the default rounding mode is used.
      %
      % If no precision `prec` is given for `b` the maximum precision of a.

      if (nargin < 2)
        rnd = mpfr_get_default_rounding_mode ();
      end
      if (nargin < 3)
        prec = max (mpfr_get_prec (a));
      end

      bb = mpfr_t (zeros (a.dims), prec);
      mpfr_sqrt (bb, a, rnd);
      b = bb;  % Do not assign b before calculation succeeded!
    end


    function b = abs (a, rnd, prec)
      % Absolute value `b = abs(a)` using rounding mode `rnd`.
      %
      % If no rounding mode `rnd` is given, the default rounding mode is used.
      %
      % If no precision `prec` is given for `b` the maximum precision of a.

      if (nargin < 2)
        rnd = mpfr_get_default_rounding_mode ();
      end
      if (nargin < 3)
        prec = max (mpfr_get_prec (a));
      end

      bb = mpfr_t (zeros (a.dims), prec);
      mpfr_abs (bb, a, rnd);
      b = bb;  % Do not assign b before calculation succeeded!
    end

  end

end

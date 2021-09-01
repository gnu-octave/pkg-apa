classdef mpfr_t

  properties (SetAccess = protected)
    dims  % Original object dimensions.
    idx   % MPFR variable indices.
  end


  methods (Static)
    function num = get_data_capacity ()
      % [internal] Return the number of pre-allocated MPFR variables.
      num = mpfr_ ('get_data_capacity');
    end

    function num = get_data_size ()
      % [internal] Return the number of currently used MPFR variables.
      num = mpfr_ ('get_data_size');
    end

    function prec = get_default_prec ()
      % Return the current default MPFR precision in bits.
      prec = mpfr_ ('get_default_prec');
    end

    function set_default_prec (prec)
      % Set the default precision to be exactly prec bits, where prec can be
      % any integer between MPFR_PREC_MIN and MPFR_PREC_MAX.  The precision of
      % a variable means the number of bits used to store its significand. All
      % subsequent calls to mpfr will use this precision, but previously
      % initialized variables are unaffected.  The default precision is set to
      % 53 bits initially.
      mpfr_ ('set_default_prec', prec);
    end

    function rnd = get_default_rounding_mode ()
      % Get the default rounding mode.  See set_default_rounding_mode for the
      % meaning of the values.
      rnd = mpfr_ ('get_default_rounding_mode');
    end

    function set_default_rounding_mode (rnd)
      % Set the default rounding mode to rnd.  The default rounding mode is
      % to nearest initially.
      %
      % -1.0 = MPFR_RNDD: round toward minus infinity
      %                   (roundTowardNegative in IEEE 754-2008).
      %  0.0 = MPFR_RNDN: round to nearest, with the even rounding rule
      %                   (roundTiesToEven in IEEE 754-2008); see details below.
      %  1.0 = MPFR_RNDU: round toward plus infinity
      %                   (roundTowardPositive in IEEE 754-2008).
      %  2.0 = MPFR_RNDZ: round toward zero (roundTowardZero in IEEE 754-2008).
      %  3.0 = MPFR_RNDA: round away from zero.
      mpfr_ ('set_default_rounding_mode', rnd);
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
        prec = mpfr_ ('get_default_prec');
      end
      if (nargin < 3)
        rnd = mpfr_ ('get_default_rounding_mode');
      end

      if (isa (x, 'mpfr_t'))
        obj.dims = x.dims;
      else
        obj.dims = size (x);
      end
      num_elems = prod (obj.dims);
      obj.idx = mpfr_ ('mex_mpfr_allocate', num_elems)';
      mpfr_ ('set_prec', obj.idx, prec);
      s.type = '()';
      s.subs = {':'};
      obj.subsasgn (s, x, rnd);
    end


    function prec = prec (obj)
      % Return the precision of obj, i.e., the number of bits used to store
      % its significant.

      prec = mpfr_ ('get_prec', obj.idx);
    end


    function d = double (obj, rnd)
      % Return a double approximation of `obj` using rounding mode `rnd`.

      if (nargin < 2)
        rnd = mpfr_ ('get_default_rounding_mode');
      end
      d = reshape (mpfr_ ('get_d', obj.idx, rnd), obj.dims(1), obj.dims(2));
    end


    % More information about class methods.
    % https://octave.org/doc/v6.3.0/Operator-Overloading.html
    % https://www.mathworks.com/help/matlab/matlab_oop/implementing-operators-for-your-class.html
    % https://www.mathworks.com/help/matlab/matlab_oop/methods-that-modify-default-behavior.html


    function disp (obj)
      % Object display
      if (isscalar (obj))
        if (prod (obj.dims) == 1)
          fprintf (1, '  MPFR scalar (precision %d binary digits)\n\n', obj.prec);
          fprintf (1, '    double approximation: %f\n', double (obj));
        else
          fprintf (1, '  %dx%d MPFR matrix\n\n', obj.dims(1), obj.dims(2));
        end
      else
        [m, n] = size (obj);
        fprintf (1, '  %dx%d MPFR array\n\n', m, n);
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
        rnd = mpfr_t.get_default_rounding_mode ();
      end
      if (nargin < 4)
        prec = [];
      end
      if (isnumeric (a))
        c = plus (b, a, rnd, prec);
      elseif (isa (a, 'mpfr_t') && isnumeric (b))
        if (isscalar (b) || isequal (a.dims, size (b)))
          if (isempty (prec))
            prec = max (mpfr_ ('get_prec', a));
          end
          cc = mpfr_t (zeros (a.dims), prec);
        else
          error ('mpfr_t:plus', 'Incompatible dimensions of a and b.');
        end
        mpfr_ ('add_d', cc, a, double (b(:)), ...
               mpfr_t.get_default_rounding_mode ());
        c = cc;  % Do not assign c before calculation succeeded!
      elseif (isa (a, 'mpfr_t') && isa (b, 'mpfr_t'))
        if (isempty (prec))
          prec = max (max (mpfr_ ('get_prec', a)), max (mpfr_ ('get_prec', b)));
        end
        if (isequal (a.dims, b.dims) || isequal (b.dims, [1 1]))
          cc = mpfr_t (zeros (a.dims), prec);
        elseif (isequal (a.dims, [1 1]))
          cc = mpfr_t (zeros (b.dims), prec);
        else
          error ('mpfr_t:plus', 'Incompatible dimensions of a and b.');
        end
        mpfr_ ('add', cc, a, b, rnd);
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
        rnd = mpfr_t.get_default_rounding_mode ();
      end
      if (nargin < 4)
        prec = [];
      end
      if (isnumeric (a) && isa (b, 'mpfr_t'))
        if (isscalar (a) || isequal (size (a), b.dims))
          if (isempty (prec))
            prec = max (mpfr_ ('get_prec', b));
          end
          cc = mpfr_t (zeros (b.dims), prec);
        else
          error ('mpfr_t:minus', 'Incompatible dimensions of a and b.');
        end
        mpfr_ ('d_sub', cc, double (a(:)), b, rnd);
        c = cc;  % Do not assign c before calculation succeeded!
      elseif (isa (a, 'mpfr_t') && isnumeric (b))
        if (isscalar (b) || isequal (a.dims, size (b)))
          if (isempty (prec))
            prec = max (mpfr_ ('get_prec', a));
          end
          cc = mpfr_t (zeros (a.dims), prec);
        else
          error ('mpfr_t:minus', 'Incompatible dimensions of a and b.');
        end
        mpfr_ ('sub_d', cc, a, double (b(:)), rnd);
        c = cc;  % Do not assign c before calculation succeeded!
      elseif (isa (a, 'mpfr_t') && isa (b, 'mpfr_t'))
        if (isempty (prec))
          prec = max (max (mpfr_ ('get_prec', a)), max (mpfr_ ('get_prec', b)));
        end
        if (isequal (a.dims, b.dims) || isequal (b.dims, [1 1]))
          cc = mpfr_t (zeros (a.dims), prec);
        elseif (isequal (a.dims, [1 1]))
          cc = mpfr_t (zeros (b.dims), prec);
        else
          error ('mpfr_t:minus', 'Incompatible dimensions of a and b.');
        end
        mpfr_ ('sub', cc, a, b, rnd);
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
        rnd = mpfr_t.get_default_rounding_mode ();
      end
      if (nargin < 4)
        prec = [];
      end
      if (isnumeric (a))
        c = times (b, a);
      elseif (isa (a, 'mpfr_t') && isnumeric (b))
        if (isscalar (b) || isequal (a.dims, size (b)))
          if (isempty (prec))
            prec = max (mpfr_ ('get_prec', a));
          end
          cc = mpfr_t (zeros (a.dims), prec);
        else
          error ('mpfr_t:times', 'Incompatible dimensions of a and b.');
        end
        mpfr_ ('mul_d', cc, a, double (b(:)), rnd);
        c = cc;  % Do not assign c before calculation succeeded!
      elseif (isa (a, 'mpfr_t') && isa (b, 'mpfr_t'))
        if (isempty (prec))
          prec = max (max (mpfr_ ('get_prec', a)), max (mpfr_ ('get_prec', b)));
        end
        if (isequal (a.dims, b.dims) || isequal (b.dims, [1 1]))
          cc = mpfr_t (zeros (a.dims), prec);
        elseif (isequal (a.dims, [1 1]))
          cc = mpfr_t (zeros (b.dims), prec);
        else
          error ('mpfr_t:times', 'Incompatible dimensions of a and b.');
        end
        mpfr_ ('mul', cc, a, b, rnd);
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
        rnd = mpfr_t.get_default_rounding_mode ();
      end
      if (nargin < 4)
        prec = [];
      end
      if (isnumeric (a) && isa (b, 'mpfr_t'))
        if (isscalar (a) || isequal (size (a), b.dims))
          if (isempty (prec))
            prec = max (mpfr_ ('get_prec', b));
          end
          cc = mpfr_t (zeros (b.dims), prec);
        else
          error ('mpfr_t:rdivide', 'Incompatible dimensions of a and b.');
        end
        mpfr_ ('d_div', cc, double (a(:)), b, rnd);
        c = cc;  % Do not assign c before calculation succeeded!
      elseif (isa (a, 'mpfr_t') && isnumeric (b))
        if (isscalar (b) || isequal (a.dims, size (b)))
          if (isempty (prec))
            prec = max (mpfr_ ('get_prec', a));
          end
          cc = mpfr_t (zeros (a.dims), prec);
        else
          error ('mpfr_t:rdivide', 'Incompatible dimensions of a and b.');
        end
        mpfr_ ('div_d', cc, a, double (b(:)), rnd);
        c = cc;  % Do not assign c before calculation succeeded!
      elseif (isa (a, 'mpfr_t') && isa (b, 'mpfr_t'))
        if (isempty (prec))
          prec = max (max (mpfr_ ('get_prec', a)), max (mpfr_ ('get_prec', b)));
        end
        if (isequal (a.dims, b.dims) || isequal (b.dims, [1 1]))
          cc = mpfr_t (zeros (a.dims), prec);
        elseif (isequal (a.dims, [1 1]))
          cc = mpfr_t (zeros (b.dims), prec);
        else
          error ('mpfr_t:rdivide', 'Incompatible dimensions of a and b.');
        end
        mpfr_ ('div', cc, a, b, rnd);
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
        rnd = mpfr_t.get_default_rounding_mode ();
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
        rnd = mpfr_t.get_default_rounding_mode ();
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
        rnd = mpfr_t.get_default_rounding_mode ();
      end
      if (nargin < 4)
        prec = [];
      end
      if (isa (a, 'mpfr_t') && isnumeric (b))
        if (isscalar (b) || isequal (a.dims, size (b)))
          if (isempty (prec))
            prec = max (mpfr_ ('get_prec', a));
          end
          cc = mpfr_t (zeros (a.dims), prec);
        else
          error ('mpfr_t:power', 'Incompatible dimensions of a and b.');
        end
        mpfr_ ('pow_si', cc, a, double (b(:)), rnd);
        c = cc;  % Do not assign c before calculation succeeded!
      elseif (isa (a, 'mpfr_t') && isa (b, 'mpfr_t'))
        if (isempty (prec))
          prec = max (max (mpfr_ ('get_prec', a)), max (mpfr_ ('get_prec', b)));
        end
        if (isequal (a.dims, b.dims) || isequal (b.dims, [1 1]))
          cc = mpfr_t (zeros (a.dims), prec);
        elseif (isequal (a.dims, [1 1]))
          cc = mpfr_t (zeros (b.dims), prec);
        else
          error ('mpfr_t:power', 'Incompatible dimensions of a and b.');
        end
        mpfr_ ('pow', cc, a, b, rnd);
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
        rnd = mpfr_t.get_default_rounding_mode ();
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

      a = mpfr_t (a);
      c = (mpfr_ ('less_p', a, mpfr_t (b)) ~= 0);
      c = reshape (c, a.dims(1), a.dims(2));
    end


    function c = gt (a, b)
      % Greater than `a > b`.

      a = mpfr_t (a);
      c = (mpfr_ ('greater_p', a, mpfr_t (b)) ~= 0);
      c = reshape (c, a.dims(1), a.dims(2));
    end


    function c = le (a, b)
      % Less than or equal to `a <= b`.

      a = mpfr_t (a);
      c = (mpfr_ ('lessequal_p', a, mpfr_t (b)) ~= 0);
      c = reshape (c, a.dims(1), a.dims(2));
    end


    function c = ge (a, b)
      % Greater than or equal to `a >= b`.

      a = mpfr_t (a);
      c = (mpfr_ ('greaterequal_p', a, mpfr_t (b)) ~= 0);
      c = reshape (c, a.dims(1), a.dims(2));
    end


    function c = ne (a, b)
      % Not equal to `a ~= b`.

      c = ~eq (a, b);
    end


    function c = eq (a, b)
      % Equality `a == b`.

      a = mpfr_t (a);
      c = (mpfr_ ('equal_p', a, mpfr_t (b)) ~= 0);
      c = reshape (c, a.dims(1), a.dims(2));
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
          rnd = obj.get_default_rounding_mode ();
        end
        subs = s.subs{:};
        obj_numel = obj.idx(2) - obj.idx(1) + 1;

        % Shortcut ':' magic colon indexing.
        if (ischar (subs) && strcmp (subs, ':'))
          c = mpfr_t (zeros (1, obj_numel), max (obj.prec));
          ret = mpfr_ ('set', c.idx, obj.idx, rnd);
          obj.warnInexactOperation (ret);
          return;
        end

        % Numerical indices.
        if ((min (subs) < 1) || (max (subs) > obj_numel))
          error ('mpfr_t:subsref', ['Invalid index range. ' ...
            ' Valid index range is [1 %d], but [%d %d] was requested'], ...
            obj_numel, min (subs), max (subs));
        end
        c = mpfr_t (zeros (1, length (subs)), max (obj.prec));
        % Avoid for-loop if indices are contiguous.
        if (isequal (subs, (subs(1):subs(end))))
          ret = mpfr_ ('set', c.idx, obj.idx(1) + [subs(1), subs(end)] - 1, ...
                       rnd);
        else
          for i = 1:length (subs)
            ret = mpfr_ ('set', c.idx(1) + [i, i] - 1, ...
                              obj.idx(1) + [i, i] - 1, rnd);
          end
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
          rnd = obj.get_default_rounding_mode ();
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
            rop = obj.idx;
          else
            rop = obj.idx(1) + [subs(1), subs(end)] - 1;
          end
          if (isa (b, 'mpfr_t'))
            ret = mpfr_ ('set', rop, b.idx, rnd);
          elseif (isnumeric (b))
            ret = mpfr_ ('set_d', rop, b(:), rnd);
          elseif (iscellstr (b) || ischar (b))
            if (ischar (b))
              b = {b};
            end
            [ret, strpos] = mpfr_ ('strtofr', rop, b(:), 0, rnd);
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
              op = @(i) b.idx;
            else
              op = @(i) b.idx(1) + [i, i] - 1;
            end
            for i = 1:length (subs)
              ret = mpfr_ ('set', rop(i), op(i), rnd);
            end
          elseif (isnumeric (b))
            if (isscalar (b))
              op = @(i) b;
            else
              op = @(i) b(i);
            end
            for i = 1:length (subs)
              ret = mpfr_ ('set_d', rop(i), op(i), rnd);
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
              [ret(i), strpos] = mpfr_ ('strtofr', rop(i), op(i), 0, rnd);
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


    function subsindex (varargin)
      % Subscript index `c = b(a)`
      error ('mpfr_t:vertcat', 'MPFR_T variables cannot be used as index.');
    end
    
    
    function b = sqrt (a, rnd, prec)
      % Square root `b = sqrt(a)` using rounding mode `rnd`.
      %
      % If no rounding mode `rnd` is given, the default rounding mode is used.
      %
      % If no precision `prec` is given for `b` the maximum precision of a.

      if (nargin < 2)
        rnd = mpfr_t.get_default_rounding_mode ();
      end
      if (nargin < 3)
        prec = max (mpfr_ ('get_prec', a));
      end

      bb = mpfr_t (zeros (a.dims), prec);
      mpfr_ ('sqrt', bb, a, rnd);
      b = bb;  % Do not assign c before calculation succeeded!
    end
    
    
    function b = abs (a, rnd, prec)
      % Absolute value `b = abs(a)` using rounding mode `rnd`.
      %
      % If no rounding mode `rnd` is given, the default rounding mode is used.
      %
      % If no precision `prec` is given for `b` the maximum precision of a.

      if (nargin < 2)
        rnd = mpfr_t.get_default_rounding_mode ();
      end
      if (nargin < 3)
        prec = max (mpfr_ ('get_prec', a));
      end

      bb = mpfr_t (zeros (a.dims), prec);
      mpfr_ ('abs', bb, a, rnd);
      b = bb;  % Do not assign c before calculation succeeded!
    end

  end

end

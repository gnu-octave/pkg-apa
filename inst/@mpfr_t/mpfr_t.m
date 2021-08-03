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


  methods
    function obj = mpfr_t (x, prec, rnd)
      if (nargin < 1)
        error ('mpfr_t:mpfr_t', 'At least one argument must be provided.');
      end

      if (isa (x, 'mpfr_t'))
        obj = x;  % Do nothing.  Good to ensure mpfr_t.
        return;
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

      if (ischar (x))
        obj = mpfr_t ({x}, prec, rnd);
      elseif (isnumeric (x))
        obj.dims = size (x);
        obj.idx = mpfr_ ('mex_mpfr_allocate', prod (obj.dims))';
        mpfr_ ('set_prec', obj.idx, prec);
        mpfr_ ('set_d', obj.idx, x(:), rnd);
      elseif (iscellstr (x))
        obj.dims = size (x);
        obj.idx = mpfr_ ('mex_mpfr_allocate', prod (obj.dims))';
        mpfr_ ('set_prec', obj.idx, prec);
        [ret, strpos] = mpfr_ ('strtofr', obj.idx, x(:), 0, rnd);
        if (any (ret))
          warning ('mpfr_t:inexact_conversion', ...
                   'Conversion of %d value(s) was inexact.', sum (ret ~= 0));
        end
        bad_strs = (cellfun (@numel, x(:)) >= strpos);
        if (any (bad_strs))
          warning ('mpfr_t:bad_conversion', ...
                   'Conversion of %d value(s) failed due to bad input.', ...
                   sum (bad_strs));
        end
      else
        error ('mpfr_t:mpfr_t', 'Input must be numeric, string, or mpfr_t.');
      end
    end

    function prec = prec (obj)
      % Return the precision of obj, i.e., the number of bits used to store
      % its significant.
      prec = mpfr_ ('get_prec', obj.idx);
    end

    function d = double (obj, rnd)
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

    function uplus(a)
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

%    function mrdivide(a,b)
%    % Matrix right division `a/b`
%    end

%    function mldivide(a,b)
%    % Matrix left division `a\b`
%    end

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

%    function mpower(a,b)
%    % Matrix power `a^b`
%    end

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

    function and (a, b)
      % Logical AND `a & b`.
      error ('mpfr_t:and', 'Logical AND not supported for MPFR_T variables.');
    end

    function or (a, b)
      % Logical OR `a | b`.
      error ('mpfr_t:or', 'Logical OR not supported for MPFR_T variables.');
    end

    function not (a)
      % Logical NOT `~a`.
      error ('mpfr_t:not', 'Logical NOT not supported for MPFR_T variables.');
    end


%    function colon(a,d,b)
%    % Colon operator `a:d:b`
%    end

%    function colon(a,b)
%    % Colon operator `a:b`
%    end

%    function ctranspose(a)
%    % Complex conjugate transpose `a'`
%    end

%    function transpose(a)
%    % Matrix transpose a.'`
%    end

    function c = horzcat (a, b, varargin)
      %TODO Horizontal concatenation `[a b]`.
      error ('mpfr_t:horzcat', ...
        ['Arrays of MPFR_T variables are not supported.  ', ...
         'Use cell arrays {a, b} instead.']);
    end

    function c = vertcat (a, b, varargin)
      %TODO Vertical concatenation `[a; b]`.
      error ('mpfr_t:vertcat', ...
        ['Arrays of MPFR_T variables are not supported.  ', ...
         'Use cell arrays {a; b} instead.']);
    end

%    function subsref(a,s)
%    % Subscripted reference `a(s1,s2,...sn)`
%    end

%    function subsasgn(a,s,b)
%    % Subscripted assignment `a(s1,...,sn) = b`
%    end

%    function subsindex(a)
%    % Subscript index `b(a)`
%    end

  end

end

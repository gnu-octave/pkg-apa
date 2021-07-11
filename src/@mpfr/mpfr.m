classdef mpfr

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
    function obj = mpfr (x, prec, rnd)
      if (nargin < 1)
        error ('mpfr:mpfr', 'At least one argument must be provided.');
      end

      if (ndims (x) > 2)
        error ('mpfr:mpfr', 'Only two dimensional matrix input is supported.');
      end
      obj.dims = size (x);

      if (nargin < 2)
        prec = mpfr_ ('get_default_prec');
      end
      if (nargin < 3)
        rnd = mpfr_ ('get_default_rounding_mode');
      end

      if (isnumeric (x))
        obj.idx = mpfr_ ('allocate', prod (obj.dims))';
        mpfr_ ('set_prec', obj.idx, prec);
        mpfr_ ('set_d', obj.idx, x(:), rnd);
      elseif (ischar (x))
        error ('mpfr:mpfr', 'TODO');
      elseif (isa (x, 'mpfr'))
        error ('mpfr:mpfr', 'TODO');
      else
        error ('mpfr:mpfr', 'Input must be numeric, string, or MPFR.');
      end
    end

    function d = double (obj, rnd)
      if (nargin < 2)
        rnd = mpfr_ ('get_default_rounding_mode');
      end
      d = reshape (mpfr_ ('get_d', obj.idx, rnd), obj.dims(1), obj.dims(2));
    end

    % More information about class methods.
    % https://octave.org/doc/v6.2.0/Operator-Overloading.html
    % https://www.mathworks.com/help/matlab/matlab_oop/implementing-operators-for-your-class.html
    % https://www.mathworks.com/help/matlab/matlab_oop/methods-that-modify-default-behavior.html

    function display (obj)
      % Object display
      disp (obj);
    end

    function disp (obj)
      % Object display
      fprintf (1, '\n');
      fprintf (1, '  %dx%d MPFR matrix\n', obj.dims(1), obj.dims(2));
      fprintf (1, '\n');
    end

    function c = plus (a, b)
      % Binary addition `a + b`
      % Precision of result is the maximum precision of a and b.
      % Using default rounding mode.
      if (isnumeric (a))
        c = plus (b, a);
      elseif (isa (a, 'mpfr') && isnumeric (b))
        if (isscalar (b) || isequal (a.dims, size (b)))
          prec = max (mpfr_ ('get_prec', a));
          cc = mpfr (zeros (a.dims), prec);
        else
          error ('mpfr:plus', 'Incompatible dimensions of a and b.');
        end
        mpfr_ ('add_d', cc, a, double (b(:)),
               mpfr.get_default_rounding_mode ());
        c = cc;  % Do not assign c before calculation succeeded!
      elseif (isa (a, 'mpfr') && isa (b, 'mpfr'))
        prec = max (max (mpfr_ ('get_prec', a)), max (mpfr_ ('get_prec', b)));
        if (isequal (a.dims, b.dims) || isequal (b.dims, [1 1]))
          cc = mpfr (zeros (a.dims), prec);
        elseif (isequal (a.dims, [1 1]))
          cc = mpfr (zeros (b.dims), prec);
        else
          error ('mpfr:plus', 'Incompatible dimensions of a and b.');
        end
        mpfr_ ('add', cc, a, b, mpfr.get_default_rounding_mode ());
        c = cc;  % Do not assign c before calculation succeeded!
      else
        error ('mpfr:plus', 'Invalid operands a and b.');
      end
    end

    function c = minus (a, b)
      % Binary subtraction `a - b`
      % Precision of result is the maximum precision of a and b.
      % Using default rounding mode.
      if (isnumeric (a) && isa (b, 'mpfr'))
        if (isscalar (a) || isequal (size (a), b.dims))
          prec = max (mpfr_ ('get_prec', b));
          cc = mpfr (zeros (b.dims), prec);
        else
          error ('mpfr:minus', 'Incompatible dimensions of a and b.');
        end
        mpfr_ ('d_sub', cc, double (a(:)), b,
               mpfr.get_default_rounding_mode ());
        c = cc;  % Do not assign c before calculation succeeded!
      elseif (isa (a, 'mpfr') && isnumeric (b))
        if (isscalar (b) || isequal (a.dims, size (b)))
          prec = max (mpfr_ ('get_prec', a));
          cc = mpfr (zeros (a.dims), prec);
        else
          error ('mpfr:minus', 'Incompatible dimensions of a and b.');
        end
        mpfr_ ('sub_d', cc, a, double (b(:)),
               mpfr.get_default_rounding_mode ());
        c = cc;  % Do not assign c before calculation succeeded!
      elseif (isa (a, 'mpfr') && isa (b, 'mpfr'))
        prec = max (max (mpfr_ ('get_prec', a)), max (mpfr_ ('get_prec', b)));
        if (isequal (a.dims, b.dims) || isequal (b.dims, [1 1]))
          cc = mpfr (zeros (a.dims), prec);
        elseif (isequal (a.dims, [1 1]))
          cc = mpfr (zeros (b.dims), prec);
        else
          error ('mpfr:minus', 'Incompatible dimensions of a and b.');
        end
        mpfr_ ('sub', cc, a, b, mpfr.get_default_rounding_mode ());
        c = cc;  % Do not assign c before calculation succeeded!
      else
        error ('mpfr:minus', 'Invalid operands a and b.');
      end
    end

%    function uminus(a)
%    % Unary minus `-a`
%    end

%    function uplus(a)
%    % Unary plus `+a`
%    end

    function c = times (a, b)
      % Element-wise multiplication `a .* b`
      % Precision of result is the maximum precision of a and b.
      % Using default rounding mode.
      if (isnumeric (a))
        c = times (b, a);
      elseif (isa (a, 'mpfr') && isnumeric (b))
        if (isscalar (b) || isequal (a.dims, size (b)))
          prec = max (mpfr_ ('get_prec', a));
          cc = mpfr (zeros (a.dims), prec);
        else
          error ('mpfr:times', 'Incompatible dimensions of a and b.');
        end
        mpfr_ ('mul_d', cc, a, double (b(:)),
               mpfr.get_default_rounding_mode ());
        c = cc;  % Do not assign c before calculation succeeded!
      elseif (isa (a, 'mpfr') && isa (b, 'mpfr'))
        prec = max (max (mpfr_ ('get_prec', a)), max (mpfr_ ('get_prec', b)));
        if (isequal (a.dims, b.dims) || isequal (b.dims, [1 1]))
          cc = mpfr (zeros (a.dims), prec);
        elseif (isequal (a.dims, [1 1]))
          cc = mpfr (zeros (b.dims), prec);
        else
          error ('mpfr:times', 'Incompatible dimensions of a and b.');
        end
        mpfr_ ('mul', cc, a, b, mpfr.get_default_rounding_mode ());
        c = cc;  % Do not assign c before calculation succeeded!
      else
        error ('mpfr:times', 'Invalid operands a and b.');
      end
    end

%    function mtimes(a,b)
%    % Matrix multiplication `a*b`
%    end

     function c = rdivide (a, b)
      % Right element-wise division `a ./ b`
      % Precision of result is the maximum precision of a and b.
      % Using default rounding mode.
      if (isnumeric (a) && isa (b, 'mpfr'))
        if (isscalar (a) || isequal (size (a), b.dims))
          prec = max (mpfr_ ('get_prec', b));
          cc = mpfr (zeros (b.dims), prec);
        else
          error ('mpfr:rdivide', 'Incompatible dimensions of a and b.');
        end
        mpfr_ ('d_div', cc, double (a(:)), b,
               mpfr.get_default_rounding_mode ());
        c = cc;  % Do not assign c before calculation succeeded!
      elseif (isa (a, 'mpfr') && isnumeric (b))
        if (isscalar (b) || isequal (a.dims, size (b)))
          prec = max (mpfr_ ('get_prec', a));
          cc = mpfr (zeros (a.dims), prec);
        else
          error ('mpfr:rdivide', 'Incompatible dimensions of a and b.');
        end
        mpfr_ ('div_d', cc, a, double (b(:)),
               mpfr.get_default_rounding_mode ());
        c = cc;  % Do not assign c before calculation succeeded!
      elseif (isa (a, 'mpfr') && isa (b, 'mpfr'))
        prec = max (max (mpfr_ ('get_prec', a)), max (mpfr_ ('get_prec', b)));
        if (isequal (a.dims, b.dims) || isequal (b.dims, [1 1]))
          cc = mpfr (zeros (a.dims), prec);
        elseif (isequal (a.dims, [1 1]))
          cc = mpfr (zeros (b.dims), prec);
        else
          error ('mpfr:rdivide', 'Incompatible dimensions of a and b.');
        end
        mpfr_ ('div', cc, a, b, mpfr.get_default_rounding_mode ());
        c = cc;  % Do not assign c before calculation succeeded!
      else
        error ('mpfr:rdivide', 'Invalid operands a and b.');
      end
    end

    function c = ldivide (a, b)
      % Left element-wise division `a .\ b`
      c = rdivide (b, a);
    end

%    function mrdivide(a,b)
%    % Matrix right division `a/b`
%    end

%    function mldivide(a,b)
%    % Matrix left division `a\b`
%    end

%    function power(a,b)
%    % Element-wise power `a.^b`
%    end

%    function mpower(a,b)
%    % Matrix power `a^b`
%    end

%    function lt(a,b)
%    % Less than `a < b`
%    end

%    function gt(a,b)
%    % Greater than `a > b`
%    end

%    function le(a,b)
%    % Less than or equal to `a <= b`
%    end

%    function ge(a,b)
%    % Greater than or equal to `a >= b`
%    end

%    function ne(a,b)
%    % Not equal to `a ~= b`
%    end

%    function eq(a,b)
%    % Equality `a == b`
%    end

%    function and(a,b)
%    % Logical AND `a & b`
%    end

%    function or(a,b)
%    % Logical OR `a | b`
%    end

%    function not(a)
%    % Logical NOT `~a`
%    end

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

%    function horzcat(a,b,...)
%    % Horizontal concatenation `[a b]`
%    end

%    function vertcat(a,b,...)
%    % Vertical concatenation `[a; b]`
%    end

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

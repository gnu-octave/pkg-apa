function obj = mpfr (x, prec)
  if (nargin < 1)
    error ('mpfr:mpfr', 'At least one argument must be provided');
  end
  
  if (nargin < 2)
    prec = mpfr_ ();
  end
  obj.prec = prec;
end

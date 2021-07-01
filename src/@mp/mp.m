function obj = mp (x, prec)
  if (nargin < 1)
    error ('mp:mp', 'At least one argument must be provided');
  end
  
  if (nargin < 2)
    prec = __mpfr__ ();
  end
  obj.prec = prec;
end

function varargout = mpfr_ (varargin)
  % Wrapper function to convert MPFR objects to internal MPFR variable indices.
  varargin = cellfun (@mpfr2idx, varargin, 'UniformOutput', false);
  [varargout{1:nargout}] = mpfr_interface (varargin{:});
end

function y = mpfr2idx (x)
  % Convert MPFR objects to internal MPFR variable indices.
  if (isa (x, 'mpfr'))
    y = x.idx;
  else
    y = x;
  end
end

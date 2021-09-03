function mpfr_nextabove (x)
% mpfr_nextabove (x)
%
% Equivalent to ‘mpfr_nexttoward’ where Y is plus infinity (resp.
% minus infinity).
%

  if (isa (x, 'mpfr_t'))
    x = x.idx;
  end
  gmp_mpfr_interface (167, x);
end

% This function was automatically generated by "generate_m_files".

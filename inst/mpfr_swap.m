function mpfr_swap (x, y)
% Swap the structures pointed to by X and Y.  In particular, the
% values are exchanged without rounding (this may be different from
% three ‘mpfr_set’ calls using a third auxiliary variable).
%
% Warning!  Since the precisions are exchanged, this will affect
% future assignments.  Moreover, since the significand pointers are
% also exchanged, you must not use this function if the allocation
% method used for X and/or Y does not permit it.  This is the case
% when X and/or Y were declared and initialized with
% ‘MPFR_DECL_INIT’, and possibly with ‘mpfr_custom_init_set’ (*note
% Custom Interface::).
%

  if (isa (x, 'mpfr_t'))
    x = x.idx;
  end
  if (isa (y, 'mpfr_t'))
    y = y.idx;
  end
  gmp_mpfr_interface (13, x, y);
end

% This function was automatically generated by "generate_m_files".

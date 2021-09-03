function ret = mpfr_cmp_d (op1, op2)
% ret = mpfr_cmp_d (op1, op2)
%
% Compare OP1 and OP2.  Return a positive value if OP1 > OP2, zero if
% OP1 = OP2, and a negative value if OP1 < OP2.  Both OP1 and OP2 are
% considered to their full own precision, which may differ.  If one
% of the operands is NaN, set the _erange_ flag and return zero.
%
% Note: These functions may be useful to distinguish the three
% possible cases.  If you need to distinguish two cases only, it is
% recommended to use the predicate functions (e.g., ‘mpfr_equal_p’
% for the equality) described below; they behave like the IEEE 754
% comparisons, in particular when one or both arguments are NaN.  But
% only floating-point numbers can be compared (you may need to do a
% conversion first).
%

  if (isa (op1, 'mpfr_t'))
    op1 = op1.idx;
  end
  ret = gmp_mpfr_interface (64, op1, op2);
end

% This function was automatically generated by "generate_m_files".

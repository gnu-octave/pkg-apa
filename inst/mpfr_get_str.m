function ret = mpfr_get_str (*str, *expptr, base, n, op, rnd)
  if (isa (op, 'mpfr_t'))
    op = op.idx;
  end
  ret = gmp_mpfr_interface (21, *str, *expptr, base, n, op, rnd);
end

% This function was automatically generated by "generate_m_files".
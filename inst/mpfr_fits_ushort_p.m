function ret = mpfr_fits_ushort_p (op, rnd)
  if (isa (op, 'mpfr_t'))
    op = op.idx;
  end
  ret = gmp_mpfr_interface (27, op, rnd);
end

% This function was automatically generated by "generate_m_files".

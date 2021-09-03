function ret = mpfr_modf (iop, fop, op, rnd)
  if (isa (iop, 'mpfr_t'))
    iop = iop.idx;
  end
  if (isa (fop, 'mpfr_t'))
    fop = fop.idx;
  end
  if (isa (op, 'mpfr_t'))
    op = op.idx;
  end
  ret = gmp_mpfr_interface (155, iop, fop, op, rnd);
end

% This function was automatically generated by "generate_m_files".
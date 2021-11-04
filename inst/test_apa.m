function test_apa ()
% Self-test for @mpfr_t class and MPFR-low-level interface.

  default_verbosity_level = apa ('verbose');

  % Check if @mpfr_t variables exist(ed).
  if (mpfr_t.get_data_size () || mpfr_t.get_data_capacity ())
    error ('apa:test:dirtyEnvironment', ...
      ['Existing @mpfr_t variables detected.  Please run the test suite ', ...
       'in a clean Octave/Matlab session.\n\nRun:\n  clear all; test_apa']);
  end


  % =====================
  % mpfr_set_default_prec
  % =====================

  % Good input
  % The value `intmax ("int32") - 256` is taken from "mpfr.h" and should work
  % for 32- and 64-bit precision types.
  for i = [4, intmax("int32") - 256 - 1, 53]
    mpfr_set_default_prec (i);
    assert (mpfr_get_default_prec () == i);
  end

  % Bad input
  mpfr_set_default_prec (42)
  apa ('verbose', 0);
  for i = {0, inf, -42, -2, 1/6, nan, 'c', eye(3), intmax('int64')}
    assert (strcmp (check_error ('mpfr_set_default_prec (i{1})'), ...
                    'apa:mexFunction'));
  end
  apa ('verbose', default_verbosity_level);


  % ==============================
  % mpfr_set_default_rounding_mode
  % ==============================

  % Good input
  for i = -1:3
    mpfr_set_default_rounding_mode (i);
    assert (mpfr_get_default_rounding_mode () == i);
  end

  % Bad input
  mpfr_set_default_rounding_mode (0);
  apa ('verbose', 0);
  for i = {inf, -42, -2, 4, 1/6, nan, 'c', eye(3)}
    assert (strcmp (check_error ('mpfr_set_default_rounding_mode (i{1})'), ...
                    'apa:mexFunction'));
  end
  apa ('verbose', default_verbosity_level);


  % ===============
  % mpfr_t.allocate
  % ===============

  % Good input
  DATA_CHUNK_SIZE = 1000;
  N = ceil (sqrt (DATA_CHUNK_SIZE));
  obj = mpfr_t (1);
  assert (isequal (obj.idx, [1, 1]));
  assert (mpfr_t.get_data_size () == 1);
  assert (mpfr_t.get_data_capacity () == DATA_CHUNK_SIZE);
  obj = mpfr_t (eye (N));
  assert (isequal (obj.idx, [2, N^2 + 1]));
  assert (mpfr_t.get_data_size () == N^2 + 1);
  assert (mpfr_t.get_data_capacity () == 2 * DATA_CHUNK_SIZE);

  % Bad input
  apa ('verbose', 0);
  for i = {1/2, inf, -42, -1, -2, nan, 'c', eye(3)}
    assert (strcmp (check_error ('mpfr_t.allocate (i{1})'), 'apa:mexFunction'));
    assert (mpfr_t.get_data_size () == N^2 + 1);
    assert (mpfr_t.get_data_capacity () == 2 * DATA_CHUNK_SIZE);
  end
  apa ('verbose', default_verbosity_level);


  % ==================
  % mpfr_t constructor
  % ==================

  % Good input
  mpfr_set_default_prec (53);
  vals = {inf, nan, pi, 1/6, 42, 2, 1, 2.5, eye(4), [1, 2; nan, 4]};
  vals = [vals, cellfun(@uminus, vals, "UniformOutput", false)];
  for i = vals
    assert (isequaln (double (mpfr_t (i{1})), i{1}));
  end

  % Bad input (precision)
  apa ('verbose', 0);
  for i = {inf, -42, -2, 1/6, nan, 'c', eye(3), intmax('int64')}
    assert (strcmp (check_error ('mpfr_t (1, i{1})'), 'apa:mexFunction'));
  end
  apa ('verbose', default_verbosity_level);

  % Bad input (rounding mode)
  apa ('verbose', 0);
  for i = {inf, -42, -2, 4, 1/6, nan, 'c', eye(3)}
    assert (strcmp (check_error ('mpfr_t (1, 53, i{1})'), 'apa:mexFunction'));
  end
  apa ('verbose', default_verbosity_level);


  % ==================================
  % Indexing read operations (subsref)
  % ==================================

  N = 4;
  A = rand (N);
  Ampfr = mpfr_t (A);

  % 1D indexing
  for i = 1:numel (A)
    assert (isequal (double (Ampfr(i)), A(i)));
  end
  assert (isequal (double (Ampfr(:)), A(:)));
  assert (isequal (double (Ampfr([])), A([])));

  % 2D indexing
  for i = 1:N
    for j = 1:N
      assert (isequal (double (Ampfr(i,j)), A(i,j)));
      assert (isequal (double (Ampfr(1:i,j)), A(1:i,j)));
      assert (isequal (double (Ampfr(i,1:j)), A(i,1:j)));
      assert (isequal (double (Ampfr(1:i,1:j)), A(1:i,1:j)));
      assert (isequal (double (Ampfr(i:end,j)), A(i:end,j)));
      assert (isequal (double (Ampfr(i,j:end)), A(i,j:end)));
      assert (isequal (double (Ampfr(i:end,j:end)), A(i:end,j:end)));
      assert (isequal (double (Ampfr(i:j,i:j)), A(i:j,i:j)));
      assert (isequal (double (Ampfr(j:i,j:i)), A(j:i,j:i)));
    end
    assert (isequal (double (Ampfr(i,:)), A(i,:)));
    assert (isequal (double (Ampfr(i,[])), A(i,[])));
  end
  for j = 1:N
    assert (isequal (double (Ampfr(:,j)), A(:,j)));
    assert (isequal (double (Ampfr([],j)), A([],j)));
  end
  assert (isequal (double (Ampfr([],:)), A([],:)));
  assert (isequal (double (Ampfr(:,[])), A(:,[])));


  % ====================================
  % Indexing write operations (subsasgn)
  % ====================================

  N = 4;
  A = rand (N);
  Ampfr = mpfr_t (A);

  to_str = @(i) cellfun (@num2str, num2cell (i), 'UniformOutput', false);
  for ops = {@(i) i, @(i) mpfr_t(i), to_str}
    op = ops{1};

    % 1D indexing
    for i = 1:numel (A)
      A(i)     = i;
      Ampfr(i) = op(i);
      assert (isequal (double (Ampfr), A));
    end
    vec = numel (A):-1:1;
    A(:)     = vec;
    Ampfr(:) = op(vec);
    assert (isequal (double (Ampfr), A));
    vec = 1:numel (A);
    A(:)     = vec';
    Ampfr(:) = op(vec');
    assert (isequal (double (Ampfr), A));
    A([])     = 1;
    Ampfr([]) = op(1);
    assert (isequal (double (Ampfr), A));
    A(1:4)     = 11:14;
    Ampfr(1:4) = op(11:14);
    assert (isequal (double (Ampfr), A));

    % 2D indexing
    for i = 1:N
      for j = 1:N
        A(i,j)     = i*j;
        Ampfr(i,j) = op(i*j);
        assert (isequal (double (Ampfr), A));
      end
      A(i,:)     = i*j*10;
      Ampfr(i,:) = op(i*j*10);
      assert (isequal (double (Ampfr), A));
      vec = (1:N) + 20;
      A(i,:)     = vec;
      Ampfr(i,:) = vec;
      assert (isequal (double (Ampfr), A));
      vec = vec + 20;
      A(i,:)     = vec';
      Ampfr(i,:) = vec';
      assert (isequal (double (Ampfr), A));
    end
    for j = 1:N
      vec = (1:N) + 50;
      A(:,j)     = vec;
      Ampfr(:,j) = vec;
      assert (isequal (double (Ampfr), A));
      vec = vec + 50;
      A(:,j)     = vec';
      Ampfr(:,j) = vec';
      assert (isequal (double (Ampfr), A));
    end
    A([],[])     = 1;
    Ampfr([],[]) = op(1);
    assert (isequal (double (Ampfr), A));
    A([],1)     = 1;
    Ampfr([],1) = op(1);
    assert (isequal (double (Ampfr), A));
    A([],:)     = 1;
    Ampfr([],:) = op(1);
    assert (isequal (double (Ampfr), A));
    A(1,[])     = 1;
    Ampfr(1,[]) = op(1);
    assert (isequal (double (Ampfr), A));
    A(:,[])     = 1;
    Ampfr(:,[]) = op(1);
    assert (isequal (double (Ampfr), A));
  end


  % ================
  % (C)Transposition
  % ================

  for ops = {@ctranspose, @transpose}
    op = ops{1};
    for m = 1:8
      for n = 1:8
        A = rand (m, n);
        assert (isequal (double (op (mpfr_t (A))), op (A)));
      end
    end
  end


  % =============
  % Concatenation
  % =============

  % vertcat (X) == cat (1, X)
  % horzcat (X) == cat (2, X)
  ops = {@vertcat, @horzcat};
  for i = 1:length (ops)
    op = ops{i};

    if (i == 1)  % vertcat
      a = (1:3)';
      b = (4:6)';
      aa = [a, a];
      bb = [b, b];
    else  % horzcat
      a = 1:3;
      b = 4:6;
      aa = [a; a];
      bb = [b; b];
    end

    am = mpfr_t (a);
    bm = mpfr_t (b);
    aam = mpfr_t (aa);
    bbm = mpfr_t (bb);
    assert (isequal (double (op (am)), op (a)));
    assert (isequal (double (op (am, am)), op (a, a)));
    assert (isequal (double (op (am, bm)), op (a, b)));
    assert (isequal (double (op (bm, am)), op (b, a)));
    assert (isequal (double (op (bm, bm)), op (b, b)));
    assert (isequal (double (op (a, am)), op (a, a)));
    assert (isequal (double (op (a, bm)), op (a, b)));
    assert (isequal (double (op (bm, a)), op (b, a)));
    assert (isequal (double (op (bm, b)), op (b, b)));
    assert (isequal (double (op (am, b, a, bm)), op (a, b, a, b)));
    assert (isequal (double (op (a, bm, am, b)), op (a, b, a, b)));

    assert (isequal (double (op (aam)), op (aa)));
    assert (isequal (double (op (aam, aam)), op (aa, aa)));
    assert (isequal (double (op (aam, bbm)), op (aa, bb)));
    assert (isequal (double (op (bbm, aam)), op (bb, aa)));
    assert (isequal (double (op (bbm, bbm)), op (bb, bb)));
    assert (isequal (double (op (aa, aam)), op (aa, aa)));
    assert (isequal (double (op (aa, bbm)), op (aa, bb)));
    assert (isequal (double (op (bbm, aa)), op (bb, aa)));
    assert (isequal (double (op (bbm, bb)), op (bb, bb)));
    assert (isequal (double (op (aam, bb, aa, bbm)), op (aa, bb, aa, bb)));
    assert (isequal (double (op (aa, bbm, aam, bb)), op (aa, bb, aa, bb)));

    % Bad example
    assert (strcmp (check_error ('op (aam, a)'), 'mpfr_t:cat:badDimensions'));
  end


  % ====================================
  % Arithmetic operations (element-wise)
  % ====================================

  for ops = {@uminus}
    op = ops{1};
    assert (isequal (double (op (mpfr_t (1))),   op (1)));
    assert (isequal (double (op (mpfr_t (1:3))), op (1:3)));
  end

  % Suppress warning for `ldivide (1:3, 1)` and `rdivide (1, 1:3)`.
  S = warning ('off', 'mpfr_t:inexactOperation');
  for ops = {@plus, @minus, @times, @ldivide, @rdivide}
    op = ops{1};
    assert (isequal (double (op (mpfr_t (1:3), mpfr_t (1:3))), op ((1:3), (1:3))));
    assert (isequal (double (op (mpfr_t (1:3), mpfr_t (1)  )), op ((1:3), 1)));
    assert (isequal (double (op (mpfr_t (1)  , mpfr_t (1:3))), op (1, (1:3))));

    assert (isequal (double (op (mpfr_t (1:3), (1:3))), op ((1:3), (1:3))));
    assert (isequal (double (op ((1:3), mpfr_t (1:3))), op ((1:3), (1:3))));
    assert (isequal (double (op (mpfr_t (1:3), 1)), op ((1:3), 1)));
    assert (isequal (double (op (1, mpfr_t (1:3))), op (1, (1:3))));
  end
  warning (S);

  for ops = {@power}
    op = ops{1};
    assert (isequal (double (op (mpfr_t (1:3), mpfr_t (1:3))), op ((1:3), (1:3))));
    assert (isequal (double (op (mpfr_t (1:3), mpfr_t (1)  )), op ((1:3), 1)));
    assert (isequal (double (op (mpfr_t (1)  , mpfr_t (1:3))), op (1, (1:3))));

    assert (isequal (double (op (mpfr_t (1:3), (1:3))), op ((1:3), (1:3))));
    assert (isequal (double (op (mpfr_t (1:3), 1)),     op ((1:3), 1)));
  end


  % =================
  % Matrix operations
  % =================

  op = @mtimes;
  for m = 1:8
    for n = 1:8
      for k = 1:8
        a = reshape (1:m*k, m, k);
        b = reshape (1:k*n, k, n);
        assert (isequal (double (op (mpfr_t (a), mpfr_t (b))), op (a, b)));
        assert (isequal (double (op (a, mpfr_t (b))), op (a, b)));
        assert (isequal (double (op (mpfr_t (a), b)), op (a, b)));
      end
    end
  end

  % ====================
  % Comparison functions
  % ====================

  for ops = {@lt, @gt, @le, @ge, @ne, @eq}
    op = ops{1};
    assert (isequal (double (op (mpfr_t (1:3), mpfr_t (1:3))), op ((1:3), (1:3))));
    assert (isequal (double (op (mpfr_t (1:3), (1:3))), op ((1:3), (1:3))));
    assert (isequal (double (op ((1:3), mpfr_t (1:3))), op ((1:3), (1:3))));
  end


  % ==========================
  % Error test helper function
  % ==========================
  
  function err_identifier = check_error (check_error_code)
    try
      eval (check_error_code);
      error ('apa:test:missed', 'Should never be reached');
    catch check_error_e
      err_identifier = check_error_e.identifier;
    end
  end

end

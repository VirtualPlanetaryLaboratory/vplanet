flarevar trappist-1e comparison
=================================

This directory contains a minimal TRAPPIST-1e atmospheric escape setup for
comparing baseline stellar XUV evolution against coarse bursty variability that
preserves the same long-time mean XUV energy. The module keyword used in
``saModules`` is ``flarevar``.

configurations
--------------

- ``baseline/``: STELLAR only (modulation OFF).
- ``periodic/``: STELLAR + FLAREVAR with ``sFlareVarModel PERIODIC``.
- ``random_bin/``: STELLAR + FLAREVAR with ``sFlareVarModel RANDOM_BIN``.
- ``periodic_dt5/``: same as ``periodic/`` with ``dTimeStep`` reduced by 5x for
  resolution checks.

run
---

From each subdirectory:

.. code-block:: bash

   vplanet vpl.in

Key star outputs are:

- ``FlareVarMult``: instantaneous modulation multiplier ``M(t)``
- ``LXUVBase``: pre-modulation stellar XUV luminosity
- ``LXUVTotal``: post-modulation stellar XUV luminosity

Key TRAPPIST-1e diagnostics in ``e.forward`` are:

- ``OxygenDragActive``: boolean flag for ``FXUV > FXUVCRITDRAG``
- ``DiffLimActive``: boolean flag for diffusion-limited water escape regime
- ``DiffLimFirstAge``: first age when diffusion-limited water escape appears
- ``-OxygenMass``: atmospheric O2 surface pressure in bars

figures
-------

The plotting script uses LaTeX text rendering (MacTeX) and is configured for
publication-style axis labels, titles, and units.

.. code-block:: bash

   conda activate vpl
   python makeplot.py pdf --rerun
   python makeplot.py png

Generated files are written to ``figures/``:

- ``flarevar_trappist1e_response.pdf`` (or ``.png``)
- ``flarevar_trappist1e_diagnostics.pdf`` (or ``.png``)
- ``flarevar_trappist1e_regimes.pdf`` (or ``.png``)

*Short reading guide:*

- The response figure compares the same baseline mean XUV history against
  periodic and random-bin flarevar forcing.
- Differences in surface water evolution isolate nonlinear escape responses
  because the time-averaged XUV forcing is preserved.
- The diagnostics figure shows instantaneous multiplier states and the running
  mean convergence toward unity.

comparison table
----------------

After running ``baseline/``, ``periodic/``, ``random_bin/``, and
``periodic_dt5/``:

.. code-block:: bash

   conda activate vpl
   python analyze_flarevar.py

This writes:

- ``figures/flarevar_trappist1e_comparison_table.md``
- ``figures/flarevar_trappist1e_comparison_table.csv``
- ``figures/flarevar_trappist1e_regime_log.md``

The comparison table includes final water inventory, total water lost, final O2,
diffusion switch age, and the time-weighted fraction with ``FXUV > FXUVCRITDRAG``.
The regime log focuses on transition timing, oxygen buildup values, and oxygen-drag
activity fractions for each run.

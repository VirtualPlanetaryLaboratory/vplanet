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

run
---

From each subdirectory:

.. code-block:: bash

   vplanet vpl.in

Key star outputs are:

- ``FlareVarMult``: instantaneous modulation multiplier ``M(t)``
- ``LXUVBase``: pre-modulation stellar XUV luminosity
- ``LXUVTotal``: post-modulation stellar XUV luminosity

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

*Short reading guide:*

- The response figure compares the same baseline mean XUV history against periodic and random-bin flarevar forcing.
- Differences in surface water evolution isolate nonlinear escape responses because the time-averaged XUV forcing is preserved.
- The diagnostics figure shows instantaneous multiplier states and the running mean convergence toward unity.

simple checks
-------------

From ``periodic/`` (after the run):

.. code-block:: bash

   awk 'NR>=1{sum+=$6;n++;if(NR==2||$5<min)min=$5}END{printf("mean(M)=%.8f\\nmin(LXUVTotal)=%.8e\\n",sum/n,min)}' trappist1e_flarevar_periodic.star.forward

Expected:

- ``mean(M)`` very close to 1 (about ``1e-3`` level for this setup)
- ``min(LXUVTotal) >= 0``

From ``random_bin/`` (after the run):

.. code-block:: bash

   awk 'NR>=1{sum+=$6;n++;if(NR==2||$5<min)min=$5}END{printf("mean(M)=%.8f\\nmin(LXUVTotal)=%.8e\\n",sum/n,min)}' trappist1e_flarevar_random.star.forward

Expected:

- ``mean(M)`` statistically close to 1 (looser tolerance than periodic)
- ``min(LXUVTotal) >= 0``

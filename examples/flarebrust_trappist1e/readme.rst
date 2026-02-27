FlareBrust TRAPPIST-1e Comparison
=================================

This directory contains a minimal TRAPPIST-1e atmospheric escape setup for
comparing baseline stellar XUV evolution against coarse bursty variability that
preserves the same long-time mean XUV energy. The module keyword used in
``saModules`` is ``flarebrust``.

Configurations
--------------

- ``baseline/``: STELLAR only (modulation OFF).
- ``periodic/``: STELLAR + FLAREBRUST with ``sFlareVarModel PERIODIC``.
- ``random_bin/``: STELLAR + FLAREBRUST with ``sFlareVarModel RANDOM_BIN``.

Run
---

From each subdirectory:

.. code-block:: bash

   vplanet vpl.in

Key star outputs are:

- ``FlareVarMult``: instantaneous modulation multiplier ``M(t)``
- ``LXUVBase``: pre-modulation stellar XUV luminosity
- ``LXUVTotal``: post-modulation stellar XUV luminosity

Simple checks
-------------

From ``periodic/`` (after the run):

.. code-block:: bash

   awk 'NR>=1{sum+=$6;n++;if(NR==2||$5<min)min=$5}END{printf("mean(M)=%.8f\\nmin(LXUVTotal)=%.8e\\n",sum/n,min)}' trappist1e_flarebrust_periodic.star.forward

Expected:

- ``mean(M)`` very close to 1 (about ``1e-3`` level for this setup)
- ``min(LXUVTotal) >= 0``

From ``random_bin/`` (after the run):

.. code-block:: bash

   awk 'NR>=1{sum+=$6;n++;if(NR==2||$5<min)min=$5}END{printf("mean(M)=%.8f\\nmin(LXUVTotal)=%.8e\\n",sum/n,min)}' trappist1e_flarebrust_random.star.forward

Expected:

- ``mean(M)`` statistically close to 1 (looser tolerance than periodic)
- ``min(LXUVTotal) >= 0``

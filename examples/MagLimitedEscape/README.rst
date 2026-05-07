Magnetic-Limited Atmospheric Escape
===================================

Six simulations that exercise the magnetic-limited escape mode in ATMESC
(option ``bMagLimitedEscape``). Three pairs, one Gunell+2018 and one
Driscoll & Bercovici 2013 each, selected by ``sMagLimitedEscapeModel``:

``trappist1e/`` and ``trappist1e_driscoll/``
   Trappist-1e with a constant 1-Earth-dipole field. Reproduces a single
   point on the T1e curve in the source notebook. Solar-wind density and
   velocity are taken from the notebook (T1e block).

``earth_const/`` and ``earth_const_driscoll/``
   Earth with a constant 1-Earth-dipole field, present-day quiet-Sun wind
   conditions. Reference Earth case for each model.

``earth_thermint/`` and ``earth_thermint_driscoll/``
   Earth coupled to THERMINT + RADHEAT. The geodynamo-predicted
   ``dMagMom`` is fed into the magnetic-limited kernel each timestep via
   the ATMESC<->THERMINT PropsAuxMulti hook, so the dipole moment evolves
   self-consistently from ~1.7 to ~1.0 Earth units over 4.5 Gyr.

Run each simulation with ``vplanet vpl.in`` from inside its subdirectory.

Figures
-------

``makeplot.py`` produces three figures that summarize the example:

``Earth.png``
   Magnetic-moment sweep for modern Earth (Gunell18). Reproduces
   notebook-style log-log plot of escape rate vs dipole moment.

``TRAPPIST1e.png``
   Magnetic-moment sweep for Trappist-1e (Gunell18). Same axes as
   ``Earth.png``; the notebook's published page-27 plot is the
   reference.

``EarthEvolution.png``
   Four-panel time evolution: per-mechanism mass loss rates for both
   the constant-B and thermint-coupled Earth runs, the dipole moment
   from each, the core temperature, and the inner core radius from
   the thermint run.

The two sweep figures run vplanet 60 times each at logarithmically
spaced magnetic moments and cache the results to ``.npz`` files
(gitignored). Delete the cache to force a re-sweep.

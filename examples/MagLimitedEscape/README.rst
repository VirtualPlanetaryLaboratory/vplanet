Gunell+2018 Magnetic-Limited Atmospheric Escape
===============================================

Three simulations that exercise the new Gunell+2018 magnetic-limited escape
mode in ATMESC (option ``bMagLimitedEscape``):

``trappist1e/``
   Trappist-1e with a constant 1-Earth-dipole field. Reproduces a single point
   on the T1e curve in the Combined-for-Paper notebook (M = 1 Earth unit).
   Solar-wind density and velocity are taken from the notebook (T1e block).

``earth_const/``
   Earth with a constant 1-Earth-dipole field, present-day quiet-Sun wind
   conditions. Acts as the ATMESC-only reference for Earth.

``earth_thermint/``
   Earth coupled to THERMINT + RADHEAT. The geodynamo-predicted
   ``dMagMom`` is fed into the Gunell kernel each timestep via the new
   ATMESC<->THERMINT PropsAuxMulti hook, so the dipole moment evolves
   self-consistently from ~1.7 to ~1.0 Earth units over 4.5 Gyr.

Run each simulation with ``vplanet vpl.in`` from inside its subdirectory.
``makeplot.py`` reads all three forward files and produces a comparison plot.

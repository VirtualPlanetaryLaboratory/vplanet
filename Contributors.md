VPLanet has been developed by a team of scientists with a broad range of
expertise. This file records who is responsible for which aspects of the code.

**Rory Barnes** is the lead developer who conceived the project, identified and
implemented the scheme of matrices and vectors of function pointers, and designed
the I/O scheme. He also wrote most of the top-level files, such as vplanet.h,
control.c, body.c, evolve.c, etc. He wrote most of eqtide.c and radheat.c.
Finally, he co-maintains the repository.

**Russell Deitrick** wrote the DistOrb, DistRot, GalHabit and POISE modules, as
well as VSPACE.

**Rodrigo Luger** write the AtmEsc and STELLAR modules, wrote the accompanying
VPLOT resource, and deisgned the repository, including the connecting continuous
integration to TRAVIS.

**David Fleming** wrote the BINARY module and has contributed to the development
of STELLAR, AtmEsc, EqTide. He also developed BigPlanet.

**Hayden Smotherman** wrote SpiNBody and has contributed to various aspects of
the code that have sped up the executable.

**Peter Driscoll** developed the ThermInt module and contributed to RadHeat.

**Thomas Quinn** has found many bugs through valgrind, and has assisted in the
development of DistOrb, DistRot, and GalHabit.

**Diego McDonald** added the Lehmer-Catling model to AtmEsc.

**Benjamin Guyer** assisted in the coupled of EqTide and ThermInt.

**Caitlyn Wilhelm** added forced eccentricity and obliquity oscillations to
POISE.

**Rodolfo Garcia** assisted in the AtmEsc and ThermInt development.

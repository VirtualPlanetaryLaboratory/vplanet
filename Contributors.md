# VPLanet Contributors

`VPLanet` has been developed by a team of scientists with a broad range of
expertise. This file records who is responsible for which aspects of the code.

**Rory Barnes** is the lead developer who conceived the project,
implemented the matrices and vectors of function pointers, designed
the I/O scheme, and maintains this repository. He also wrote most of the top-level files, such as vplanet.h,
control.c, body.c, evolve.c, etc. He wrote most of EqTide and RadHeat.
He can be reached at rory@astro.washington.edu.

**Russell Deitrick** wrote the DistOrb, DistRot, GalHabit and POISE modules, as
well as VSPACE.

**Rodrigo Luger** wrote the AtmEsc and STELLAR modules, as well as the accompanying
VPLOT resource. He also designed this repository, including setting up the continuous
integration and calculating code coverage.

**David Fleming** wrote the BINARY module and has contributed to the development
of STELLAR, AtmEsc, and EqTide. He also wrote BigPlanet.

**Hayden Smotherman** wrote SpiNBody and has contributed to various subroutines
that have signficantly increased the efficiency of the executable.

**Peter Driscoll** developed the ThermInt module, contributed to RadHeat, and
connected them both to EqTide.

**Thomas Quinn** assisted in the development of DistOrb, DistRot, and GalHabit. He
also found many bugs with valgrind and address-sanitizer.

**Diego McDonald** added the Lehmer-Catling model to AtmEsc.

**Benjamin Guyer** assisted in the coupling of EqTide and ThermInt.

**Caitlyn Wilhelm** added forced eccentricity and obliquity oscillations to
POISE, and assisted in debugging it.

**Rodolfo Garcia** assisted in AtmEsc and ThermInt development.

**Patrick Barth** assisted in AtmEsc development.

**Pramod Gupta** ported VPLanet to Windows 10.

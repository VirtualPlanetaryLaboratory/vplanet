Authors
=======

`VPLanet` has been developed by a team of scientists with a broad range of
expertise. The following list give the names and contributions of all
developers.

**Rory Barnes (University of Washington [UW])** is the lead developer who conceived the project, implemented the
matrices and vectors of function pointers, designed the I/O scheme, and
maintains this repository. He also wrote most of the top-level files, such as
vplanet.h, control.c, etc. He also developed EqTide and RadHeat. He can be
reached at rory@astro.washington.edu.

**Russell Deitrick (UW, Canadian Centre for Climate Modelling and Analysis)** developed the DistOrb, DistRot, GalHabit and POISE modules,
as well as VSPACE.

**Rodrigo Luger (UW, Anthropic AI)** developed the AtmEsc and STELLAR modules, as well as the
accompanying VPLOT resource. He also designed this repository, including setting
up the continuous integration and documentation.

**Peter Driscoll (UW, Carnegie Institute for Science)** developed the ThermInt module, contributed to RadHeat, and
connected them both to EqTide.

**Patrick Barth (MPIA, St Andrews, IWF Graz)* developed the first version of the magma ocean model.

**Ludmila Carone (IWF Graz)** is leading the development of the magma ocean model from its 1st version (with Patrick Barth) to include multi-species outgassing and radiative transfer.

**David Fleming (UW, Bayer)** developed the BINARY module and has contributed to STELLAR,
AtmEsc, and EqTide. He also wrote the first version ofBigPlanet.

**Hayden Smotherman (UW)** developed SpiNBody and has contributed to various
subroutines that have significantly increased the efficiency of the executable.
He also added OpenMP functionality for multithreading.

**Laura Amaral (UNAM, ASU)** developed the FLARE module and contributed to AtmEsc.

**Thomas Quinn (UW)** assisted in the development of DistOrb, DistRot, and GalHabit.
He also found many bugs with valgrind and address-sanitizer.

**Caitlyn Wilhelm (UW)** added forced eccentricity and obliquity oscillations to
POISE, and assisted in debugging it. She also built the BigPlanet and
MultiPlanet scripts.

**Rodolfo Garcia (UW)** assisted in AtmEsc and ThermInt development.

**Billy Quarles (Valdosta St.)** enabled POISE to read in previously computed orbital and rotational
data.

**Sean Matt (U. of Oklahoma)** added the Briemann stellar torque model.

**Pramod Gupta (UW)** ported VPLanet to Windows 10.

**Diego McDonald (UW, Berkeley)** added the Lehmer-Catling model to AtmEsc.

**Benjamin Guyer (UW)** assisted in the coupling of EqTide and ThermInt.

**Lyan Guez (UW, St. Andrews)** contributed to the development of the mpstatus script.

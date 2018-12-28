# VPLanet
[![Build Status](https://travis-ci.com/VirtualPlanetaryLaboratory/vplanet.svg?token=jABaFLLgJNHTWSqkT7CM&branch=master)](https://travis-ci.com/VirtualPlanetaryLaboratory/vplanet)
[![Documentation](https://img.shields.io/badge/read-the_docs-blue.svg?style=flat)](https://VirtualPlanetaryLaboratory.github.io/vplanet)

© 2018 The VPLanet Team.

`VPLanet` is software to simulate planetary system evolution, with a focus on habitability. Physical models, typically consisting of ordinary differential equations, are coupled together to simulate evolution for the age of a system. We strive for full transparency and reproducibility in our software, and this repository contains the [source code](src), [extensive documentation](https://VirtualPlanetaryLaboratory.github.io/vplanet), [the scripts and files to generate published figures](examples), and [scripts to validate the current release](tests). We can't claim we found aliens with closed source software!

To get started, ensure you have clang/gcc installed and follow the [QuickStart Guide](https://virtualplanetarylaboratory.github.io/vplanet/quickstart.html).

`VPLanet` currently consists of 11 functioning "modules," each containing a set of equations
that models a specifc physical process:

**AtmEsc**: Thermal escape of an atmosphere, including water photolyzation, hydrogen
escape, oxygen escape, and oxygen build-up.

**Binary**: Orbital evolution of a circumbinary planet.

**DistOrb**: 2nd and 4th order semi-analytic models of orbital evolution outside
of resonance.

**DistRot**: Evolution of a world's rotational axis due to orbital evolution and
the stellar torque (includes shape evolution as a function of rotational frequency).

**EqTide**: Tidal evolution in the equilibrium tide framework.

**GalHabit**: Evolution of a wide orbit due to the galactic tide and impulses from
passing stars (includes radial migration).

**POISE**: Energy balance climate model including dynamic ice sheets and lithospheric
compression/rebound.

**RadHeat**: Radiogenic heating of a world's interior.

**SpiNBody**: N-body integrator for the evolution of a system of massive particles.

**Stellar**: Stellar luminosity, temperature, radius, and mass concentration.

**ThermInt**: Thermal interior evolution, including magnetic fields, for planets
undergoing plate tectonics or stagnant lid evolution.

`VPLanet` is a community project. We're happy to take pull requests; if you want to create one, please issue it to the *dev* branch. Soon we will include tutorials on adding new input options, governing variables, and modules. It's a platform for planetary science that can grow exponentially, either by adding new physics or by adding competing models for clean comparisons.

The [examples/](examples) directory contains input files and scripts for generating the figures in Barnes et al. (2018) and all subsequent module descriptions. The Manual/ directory contains the source code to generate a pdf of Barnes et al. (2018) plus methods sections from subsequent modules. This pdf describes the physics of each module, validates each module against observations or past results, and uses figures from the [examples/](examples) directory. {Soon!}

An ecosystem of support software is also publicly available. In this repo, [vspace/](vspace) contains scripts to generate input files for a parameter space sweep. [bigplanet/](bigplanet) contains scripts to store large datasets in HDF5 format and quickly calculate summary properties from an integration, like change in surface temperature. In a separate repository is [vplot](https://github.com/VirtualPlanetaryLaboratory/vplot), which consists of both a command line tool to quickly plot the evolution of a system, but also matplotlib functions to more easily generate publication-worthy figures. Finally, we recommend using [approxposterior](https://github.com/dflemin3/approxposterior) to quickly obtain posterior distributions of model parameters.

Behind the scenes, the vplanet team maintains code integrity through continuous integration, in which numerous scientific and numerical tests are validated at every commit. See the [tests/](tests) directory for the validation checks that the current build passes. Additionally, we use valgrind and addresssanitizer to periodically search for memory issues like leaks, accessing memory beyond array bounds, etc. We are committed to maintaining a stable tool that planetary scientists can rely on for fast analyses of the Solar System and beyond.  

Additional `VPLanet` examples can be found at:

[Rory Barnes' GitHub Page](https://github.com/RoryBarnes)

[Virtual Planetary Laboratory GitHub Page](https://github.com/VirtualPlanetaryLaboratory)

If you'd like to stay up to date on vplanet by joining the e-mail list, please send a request to Rory Barnes, rory@astro.washington.edu.

`VPLanet` development has been supported by NASA grants NNA13AA93A, NNX15AN35G, and 13-13NAI7_0024. We also acnkowledge support from the University of Washington and the Carnegie Institute for Science.

Enjoy!

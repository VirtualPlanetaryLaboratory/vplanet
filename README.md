# VPLanet: The Virtual Planet Simulator
<p align="center">
  <a href="https://VirtualPlanetaryLaboratory.github.io/vplanet"><img src="https://img.shields.io/badge/read-the_docs-blue.svg?style=flat"></a>
  <a href="https://arxiv.org/abs/1905.06367"><img src="https://img.shields.io/badge/read-the_paper-7d93c7.svg?style=flat"/></a>
  <a href="http://ascl.net/1811.017"><img src="https://img.shields.io/badge/ascl-1811.017-blue.svg?colorB=262255" alt="ascl:1811.017" /></a>
  <a href="https://raw.github.com/VirtualPlanetaryLaboratory/vplanet/master/LICENSE"><img src="https://img.shields.io/badge/license-MIT-purple.svg"/></a>
  <img src = "https://img.shields.io/badge/platform-Linux_|%20macOS-darkgreen.svg?style=flat"><br>
  <a href="https://travis-ci.com/VirtualPlanetaryLaboratory/vplanet"><img src="https://travis-ci.com/VirtualPlanetaryLaboratory/vplanet.svg?token=jABaFLLgJNHTWSqkT7CM&branch=master"></a>
  <img src = "https://img.shields.io/badge/memcheck-dirty-darkred.svg?style=flat">
  <img src = "https://img.shields.io/badge/coverage-74%25-red.svg?style=flat">
</p>



`VPLanet` is software to simulate planetary system evolution, with a focus on habitability. Physical models, typically consisting of ordinary differential equations, are coupled together to simulate evolution for the age of a system. We strive for full transparency and reproducibility in our software, and this repository contains the [source code](src), [extensive documentation](https://VirtualPlanetaryLaboratory.github.io/vplanet), [the scripts and files to generate published figures](examples), and [scripts to validate the current release](tests). We can't claim we found aliens with closed source software!

To get started, ensure you have clang/gcc installed and follow the [QuickStart Guide](https://virtualplanetarylaboratory.github.io/vplanet/quickstart.html). Note that VPLanet does not currently function on Windows operating systems.

`VPLanet` currently consists of 11 functioning "modules," each containing a set of equations
that models a specifc physical process:

**AtmEsc**: Thermal escape of an atmosphere, including water photolyzation, hydrogen
escape, oxygen escape, and oxygen build-up.

**Binary**: Orbital evolution of a circumbinary planet.

**DistOrb**: 2nd and 4th order semi-analytic models of orbital evolution outside
of resonance.

**DistRot**: Evolution of a world's rotational axis due to orbital evolution and
the stellar torque (including shape evolution as a function of rotational frequency).

**EqTide**: Tidal evolution in the equilibrium tide framework.

**GalHabit**: Evolution of a wide orbit due to the galactic tide and impulses from
passing stars (including radial migration).

**POISE**: Energy balance climate model including dynamic ice sheets and lithospheric
compression/rebound.

**RadHeat**: Radiogenic heating in a world's core, mantle, and crust.

**SpiNBody**: N-body integrator for the evolution of a system of massive particles.

**Stellar**: Stellar luminosity, temperature, radius, and mass concentration.

**ThermInt**: Thermal interior evolution, including magnetic fields, for planets
undergoing plate tectonics or stagnant lid evolution.

The [examples/](examples) directory contains input files and scripts for generating the figures in [Barnes et al. (2019)](https://arxiv.org/abs/1905.06367) and all subsequent module descriptions. The [Manual/](Manual) directory contains the pdf of [Barnes et al. (2019)](https://arxiv.org/abs/1905.06367), which describes the physics of each module, validates the software against observations and/or past results, and uses figures from the [examples/](examples) directory.

An ecosystem of support software is also publicly available. In this repo, [vspace/](vspace) contains scripts to generate input files for a parameter space sweep. [bigplanet/](bigplanet) contains scripts to store large datasets in HDF5 format and quickly calculate summary properties from an integration, such as change in surface temperature. In a separate repository is [vplot](https://github.com/VirtualPlanetaryLaboratory/vplot), which consists of both a command line tool to quickly plot the evolution of a system, and also matplotlib functions to generate publication-worthy figures. Finally, we recommend using [approxposterior](https://github.com/dflemin3/approxposterior) to quickly obtain posterior distributions of model parameters.

`VPLanet` is a community project. We're happy to take pull requests; if you want to create one, please issue it to the *dev* branch. Soon we will include tutorials on adding new input options, governing variables, and modules. It's a platform for planetary science that can grow exponentially, either by adding new physics or by adding competing models for clean comparisons.

Behind the scenes, the VPLanet team maintains code integrity through [continuous integration](https://travis-ci.org), in which numerous scientific and numerical tests are validated at every commit. Check the "build" badge above for the current status. See the [tests/](tests) directory for the validation checks that the current build passes. The "coverage" badge shows the percentage of the code (by line number) that is currently tested. Additionally, we use [valgrind](http://valgrind.org) and [addresssanitizer](https://github.com/google/sanitizers/wiki/AddressSanitizer) to periodically search for memory issues like use of uninitialized memory, accessing memory beyond array bounds, etc. The "memcheck" badge shows the current status, either clean (no errors) or dirty. We are committed to maintaining a stable tool for scientists to analyze any planetary system.  

Additional `VPLanet` examples can be found at the following GitHub pages:

[Virtual Planetary Laboratory](https://github.com/VirtualPlanetaryLaboratory) <br />
[Rory Barnes](https://github.com/RoryBarnes) <br />
[David Fleming](https://github.com/dflemin3)

If you'd like to stay up to date on `VPLanet` by joining the e-mail list, please send a request to Rory Barnes, rory@astro.washington.edu. You can also follow VPLanet on twitter: [@VPLanetCode](https://twitter.com/VPLanetCode).

Finally, if you use this code to generate results used in any publication or conference contribution, please cite [Barnes, R. et al. (2019), PASP, submitted](https://arxiv.org/abs/1905.06367).

`VPLanet` development has been supported by NASA grants NNA13AA93A, NNX15AN35G, 80NSSC17K048, and 13-13NAI7_0024. We also acknowledge support from the University of Washington and the Carnegie Institute for Science.

Enjoy!


© 2018 The VPLanet Team.

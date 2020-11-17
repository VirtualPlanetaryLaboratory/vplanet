<p align="center">
  <img width = "250" src="docs/VPLanetLogo.png?raw=true"/>
</p>

<h1 align="center">VPLanet: The Virtual Planet Simulator</h1>

<p align="center">
  <a href="https://VirtualPlanetaryLaboratory.github.io/vplanet"><img src="https://img.shields.io/badge/read-the_docs-blue.svg?style=flat"></a>
  <a href="https://ui.adsabs.harvard.edu/abs/2019arXiv190506367B/abstract"><img src="https://img.shields.io/badge/read-the_paper-7d93c7.svg?style=flat"/></a>
  <a href="http://twitter.com/VPLanetCode"><img src="https://img.shields.io/twitter/follow/VPLanetCode.svg?style=flat-square&logo=twitter&label=Follow"></a><br>
  <a href="http://ascl.net/1811.017"><img src="https://img.shields.io/badge/ascl-1811.017-blue.svg?colorB=262255" alt="ascl:1811.017" /></a>
  <a href="LICENSE"><img src="https://img.shields.io/badge/license-MIT-purple.svg"/></a>
  <a href="examples"><img src="https://img.shields.io/badge/examples-33-darkblue.svg"/></a>
  <img src = "https://img.shields.io/badge/platform-Linux_|%20macOS_|%20Windows10-darkgreen.svg?style=flat"><br>
  <a href="https://travis-ci.com/VirtualPlanetaryLaboratory/vplanet"><img src="https://travis-ci.com/VirtualPlanetaryLaboratory/vplanet.svg?token=jABaFLLgJNHTWSqkT7CM&branch=master"></a>
  <a href="tests"><img src = "https://img.shields.io/badge/tests-46-darkorange.svg?style=flat"></a>
  <img src = "https://img.shields.io/badge/memcheck-clean-green.svg?style=flat">
  <img src = "https://img.shields.io/badge/coverage-77%25-red.svg?style=flat">
</p>

### Overview

`VPLanet` is software to simulate planetary system evolution, with a focus on habitability. Physical models, typically consisting of ordinary differential equations, are coupled together to simulate evolution, from planetary cores to passing stars, for the age of a system. We strive for full transparency and reproducibility in our software, and this repository contains 1) the [source code](src), 2) [extensive documentation](https://VirtualPlanetaryLaboratory.github.io/vplanet), 3) [the scripts and files to generate published figures](examples), and 4) [scripts to validate the current release](tests). We can't claim we found aliens with closed source software!

To get started, ensure you have clang/gcc installed and follow the [Installation Guide](https://virtualplanetarylaboratory.github.io/vplanet/quickstart.html).

### Modules

`VPLanet` currently consists of 12 functioning "modules," each containing a set of equations
that simulates a specifc physical process:

**AtmEsc**: Roche lobe overflow and thermal escape (energy-limited and radiation-recombination-limited) of an atmosphere, including water photolyzation, hydrogen
escape, oxygen escape, and oxygen build-up.

**Binary**: Orbital evolution of a single circumbinary planet.

**DistOrb**: 2nd and 4th order semi-analytic models of orbital evolution outside
of resonance.

**DistRot**: Evolution of a world's rotational axis due to orbital evolution and
the stellar torque.

**EqTide**: Tidal evolution in the equilibrium tide framework.

**GalHabit**: Evolution of a wide orbit due to the galactic tide and impulses from
passing stars (including radial migration).

**MagmOc**: Thermal and geochemical evolution of a magma ocean.

**POISE**: Energy balance climate model including dynamic ice sheets and lithospheric
compression/rebound.

**RadHeat**: Radiogenic heating in a world's core, mantle, and crust.

**SpiNBody**: N-body integrator for the evolution of a system of massive particles.

**Stellar**: Evolution of a star's bolometeric and XUV luminosity, temperature, radius, and mass concentration. Also includes magnetic braking and stellar wind spin-down.

**ThermInt**: Thermal interior evolution, including magnetic fields, for planets
undergoing plate tectonics or stagnant lid evolution.

Many of these modules can be combined together to simulate numerous phenomena and feedback loops in planetary systems.

### Resources

The [examples/](examples) directory contains input files and scripts for generating the figures in [Barnes et al. (2020)](https://ui.adsabs.harvard.edu/abs/2020PASP..132b4502B/abstract) and subsequent publications. The [Manual/](Manual) directory contains the pdf of [Barnes et al. (2020)](https://ui.adsabs.harvard.edu/abs/2020PASP..132b4502B/abstract), which describes the physics of the first 11 modules, validates the software against observations and/or past results, and uses figures from the [examples/](examples) directory.

An ecosystem of support software is also publicly available. In this repo, [vspace/](vspace) contains scripts to generate input files for a parameter space sweep, and then use [multi-planet/](multiplanet) to perform the simulations on an arbitrary number of cores. [bigplanet/](bigplanet) contains scripts to store large datasets in HDF5 format and quickly calculate summary properties from an integration, such as change in surface temperature. In a separate repository is [vplot](https://github.com/VirtualPlanetaryLaboratory/vplot), which consists of both a command line tool to quickly plot the evolution of a system, and also matplotlib functions to generate publication-worthy figures. Finally, we recommend using [approxposterior](https://github.com/dflemin3/approxposterior) to quickly obtain posterior distributions of model parameters. These python scripts are optimized for [anaconda](https://www.anaconda.com/) distributions versions 3.5-3.7.

### Code Integrity

Behind the scenes, the VPLanet team maintains code integrity through [continuous integration](https://travis-ci.org), in which numerous scientific and numerical tests are validated at every commit. Check the "build" badge above for the current status. See the [tests/](tests) directory for the validation checks that the current build passes. The "coverage" badge shows the percentage of the code (by line number) that is currently tested. Additionally, we use [valgrind](http://valgrind.org) and [addresssanitizer](https://github.com/google/sanitizers/wiki/AddressSanitizer) to periodically search for memory issues like use of uninitialized memory, accessing memory beyond array bounds, etc. The "memcheck" badge shows the current status of the master branch, either clean (no errors) or dirty. If dirty, check the [Issues](https://github.com/VirtualPlanetaryLaboratory/vplanet/issues) for more information about the current status. Note that all releases are clean. We are committed to maintaining a stable tool for scientists to analyze any planetary system.  


### Community

`VPLanet` is a community project. We're happy to take pull requests; if you want to create one, please issue it to the *dev* branch. Soon we will include tutorials on adding new input options, governing variables, and modules. It's a platform for planetary science that can grow exponentially, either by adding new physics or by adding competing models for clean comparisons.

Additional `VPLanet` examples can be found at the following GitHub pages:

[Virtual Planetary Laboratory](https://github.com/VirtualPlanetaryLaboratory) <br />
[Rory Barnes](https://github.com/RoryBarnes) <br />
[David Fleming](https://github.com/dflemin3) <br />
[Héctor Martínez-Rodríguez](https://github.com/hector-mr)<br />
[Juliette Becker](https://github.com/jxcbecker)<br />
[Patrick Barth](https://github.com/pbfeu)<br />
[David Graham](https://github.com/dglezg7)

If you believe you have encountered a bug, please raise an issue using the [Issues](https://github.com/VirtualPlanetaryLaboratory/vplanet/issues) tab at the top of this page.

If you'd like to stay up to date on `VPLanet` by joining the e-mail list, please send a request to Rory Barnes, rory@astro.washington.edu. You can also follow VPLanet on twitter: [@VPLanetCode](https://twitter.com/VPLanetCode).

### Acknowledgments

If you use this code to generate results used in any publication or conference contribution, please cite [Barnes, R. et al. (2020), PASP, 132, 24502](https://ui.adsabs.harvard.edu/abs/2020PASP..132b4502B/abstract).

`VPLanet` development has been supported by NASA grants NNA13AA93A, NNX15AN35G, 80NSSC17K048, 13-13NAI7_0024, and 80NSSC20K0229. We also acknowledge support from the University of Washington and the Carnegie Institute for Science.

Enjoy!


© 2018-2020 The VPLanet Team.

<p align="center">
  <img width = "250" src="docs/VPLanetLogo.png?raw=true"/>
</p>

<h1 align="center">VPLanet: The Virtual Planet Simulator</h1>

<p align="center">
  <a href="https://VirtualPlanetaryLaboratory.github.io/vplanet"><img src="https://img.shields.io/badge/Read-the_docs-blue.svg?style=flat"></a>
  <a href="https://github.com/VirtualPlanetaryLaboratory/vplanet/actions/workflows/docs.yml">
  <img src="https://github.com/VirtualPlanetaryLaboratory/vplanet/actions/workflows/docs.yml/badge.svg"></a>
  <a href="https://ui.adsabs.harvard.edu/abs/2019arXiv190506367B/abstract"><img src="https://img.shields.io/badge/Read-the_paper-darkgreen.svg?style=flat"></a>
  <a href="https://VirtualPlanetaryLaboratory.github.io/vplanet/conduct.html"><img src="https://img.shields.io/badge/Code%20of-Conduct-7d93c7.svg"></a>
  <a href="http://twitter.com/VPLanetCode"><img src="https://img.shields.io/twitter/follow/VPLanetCode.svg?style=flat-square&logo=twitter&label=Follow"></a><br>
  <img src="https://img.shields.io/badge/Unit%20Tests-14,182-darkblue.svg"></a>
  <a href="https://github.com/VirtualPlanetaryLaboratory/vplanet/actions/workflows/tests.yml">
  <img src="https://github.com/VirtualPlanetaryLaboratory/vplanet/actions/workflows/tests.yml/badge.svg"></a>
  <img src="https://img.shields.io/badge/Memory%20Checks-43-darkblue.svg"></a>
  <a href="https://github.com/VirtualPlanetaryLaboratory/vplanet/actions/workflows/memcheck.yml">
  <img src="https://github.com/VirtualPlanetaryLaboratory/vplanet/actions/workflows/memcheck.yml/badge.svg"></a>
  <a href="https://codecov.io/gh/VirtualPlanetaryLaboratory/vplanet">
  <img src="https://codecov.io/gh/VirtualPlanetaryLaboratory/vplanet/branch/master/graph/badge.svg?token=3LFJQO1M6H"><br>
  <a href="examples"><img src="https://img.shields.io/badge/Examples-41-darkblue.svg"></a>
  <a href="https://github.com/VirtualPlanetaryLaboratory/vplanet/actions/workflows/examples.yml">
  <img src="https://github.com/VirtualPlanetaryLaboratory/vplanet/actions/workflows/examples.yml/badge.svg"></a>
  <img src="https://img.shields.io/badge/Python-3.6%20--%203.11-7d93c7.svg"></a>
  <a href="https://github.com/VirtualPlanetaryLaboratory/vplanet/actions/workflows/pip-install.yml">
  <img src="https://github.com/VirtualPlanetaryLaboratory/vplanet/actions/workflows/pip-install.yml/badge.svg"></a><br>
  <img src = "https://img.shields.io/badge/Platforms-Linux_|%20macOS_|%20Windows10-darkgreen.svg?style=flat">
  <a href="http://ascl.net/1811.017"><img src="https://img.shields.io/badge/ASCL-1811.017-orange.svg?colorB=orange" alt="ascl:1811.017" /></a>
  <a href="LICENSE"><img src="https://img.shields.io/badge/License-MIT-purple.svg"/></a><br>

  
  </a>
</p>

<h1 align="center">The Third VPLanet Workshop will take place 29-30 Aug 2023. The workshop will be virtual and is open to everyone. <a href="https://forms.gle/wk6Sed9fgjkdVHRP9">Register here.</a></h1>

### Overview

`VPLanet` is software to simulate planetary system evolution, with a focus on habitability. Physical models, typically consisting of ordinary differential equations, are coupled together to simulate evolution, from planetary cores to passing stars, for the age of a system. We strive for full transparency and reproducibility in our software, and this repository contains 1) the [source code](src), 2) [extensive documentation](https://VirtualPlanetaryLaboratory.github.io/vplanet), 3) scripts and files to [generate published figures](examples) and perform [parameter sweeps](https://virtualplanetarylaboratory.github.io/vplanet/parametersweep.html), and 4) [scripts to validate the current release](tests). We can't claim we found life beyond the Earth with closed-source or unreliable software!

To get started, ensure you have clang/gcc installed and follow the [Installation Guide](https://virtualplanetarylaboratory.github.io/vplanet/quickstart.html). To stay up to date on this repository, [follow it on twitter](https://twitter.com/VPLanetCode).

### Modules

`VPLanet` currently consists of 13 functioning "modules," each containing a set of equations
that simulates a specifc physical process:

**AtmEsc**: Roche lobe overflow and thermal escape (energy-limited and radiation-recombination-limited) of an atmosphere, including water photolyzation, hydrogen
escape, oxygen escape, and oxygen build-up.

**Binary**: Orbital evolution of a single circumbinary planet.

**DistOrb**: 2nd and 4th order semi-analytic models of orbital evolution outside
of resonance.

**DistRot**: Evolution of a world's rotational axis due to orbital evolution and
the stellar torque.

**EqTide**: Tidal evolution in the equilibrium tide framework.

**Flare**: Flare frequency distribution and flare XUV luminosity evolution in low-mass stars.

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

The [examples/](examples) directory contains input files and scripts for generating the figures in [Barnes et al. (2020)](https://ui.adsabs.harvard.edu/abs/2020PASP..132b4502B/abstract) and subsequent publications. The "examples" badge shows if all the examples can be built with the most recent version. The [Manual/](Manual) directory contains the pdf of [Barnes et al. (2020)](https://ui.adsabs.harvard.edu/abs/2020PASP..132b4502B/abstract), which describes the physics of the first 11 modules, validates the software against observations and/or past results, and uses figures from the [examples/](examples) directory.

An ecosystem of support software is also publicly available. [VPLot](https://github.com/VirtualPlanetaryLaboratory/vplot) is both a command line tool to quickly plot the evolution of a single integration, and also includes matplotlib functions to generate publication-worthy figures. The [VSPACE](https://github.com/VirtualPlanetaryLaboratory/vspace) script generates input files for a parameter space sweep, which can then be performed on an arbitrary number of cores with [MultiPlanet](https://github.com/VirtualPlanetaryLaboratory/multi-planet). For large parameter sweeps, an enormous amount of data can be generated, which can slow analyses. To overcome this barrier, the [BigPlanet](https://github.com/VirtualPlanetaryLaboratory/bigplanet) code can both compress datasets into HDF5 format, including statistics of an integration, and tools to faciliate plotting. These three scripts can be executed from he command line to seamlessly [perform parameter sweeps](https://virtualplanetarylaboratory.github.io/vplanet/parametersweep.html). These Python scripts are optimized for [anaconda](https://www.anaconda.com/) distributions versions 3.5-3.9. The "wheels" badge indicates if you can download and install the executables with pip for these Python distributions.

### Code Integrity

Behind the scenes, the VPLanet team maintains code integrity through via various automatic checks at every merge into the main branch. You can see the status of these checks via the "badges" the GitHub logo above. Currently we perform 5 checks: documentation ("docs"), units tests ("tests"), memory checks via [valgrind](http://valgrind.org) ("memcheck"), confirmation that all [examples](examples/) are working ("examples"), and that the code is pip-installable on Linux, Mac, and Windows machines ("pip-install") for the Python distributions listed. The "coverage" badge shows the percentage of the code (by line number) that is currently tested by <a href="https://codecov.io/gh/VirtualPlanetaryLaboratory/vplanet">Codecov</a> at every commit. We are committed to maintaining a stable tool for scientists to analyze any planetary system.  

### Community

`VPLanet` is a community project. We're happy to take pull requests; if you want to create one, please issue it to the *dev* branch. The documentation includes [tutorials on adding new features and modules](https://VirtualPlanetaryLaboratory.github.io/vplanet/tutorials.html). It's a platform for planetary science that can grow exponentially, either by adding new physics or by adding competing models for clean comparisons.

A list of additional GitHub repositories with `VPLanet` examples can be found [here](https://VirtualPlanetaryLaboratory.github.io/vplanet/repos.html).

If you believe you have encountered a bug, please raise an issue using the [Issues](https://github.com/VirtualPlanetaryLaboratory/vplanet/issues) tab at the top of this page.

If you'd like to stay up to date on `VPLanet` by joining the e-mail list, please send a request to Rory Barnes, rory@astro.washington.edu. You can also follow VPLanet on twitter: [@VPLanetCode](https://twitter.com/VPLanetCode).

### Acknowledgments

If you use this code to generate results used in any publication or conference contribution, please cite [Barnes, R. et al. (2020), PASP, 132, 24502](https://ui.adsabs.harvard.edu/abs/2020PASP..132b4502B/abstract).

`VPLanet` development has been supported by NASA grants NNA13AA93A, NNX15AN35G, 80NSSC17K048, 13-13NAI7_0024, and 80NSSC20K0229. We also acknowledge support from the University of Washington and the Carnegie Institute for Science.

Enjoy!

© 2018-2023 The VPLanet Team.

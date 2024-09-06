<p align="center">
  <img width = "250" src="docs/VPLanetLogo.png?raw=true"/>
</p>

<h1 align="center">VPLanet: The Virtual Planet Simulator</h1>

<p align="center">
  <a href="https://VirtualPlanetaryLaboratory.github.io/vplanet">
    <img src="https://img.shields.io/badge/Read-the_docs-blue.svg?style=flat">
  </a>
  <img src="https://github.com/VirtualPlanetaryLaboratory/vplanet/actions/workflows/docs.yml/badge.svg">  
  <a href="https://ui.adsabs.harvard.edu/abs/2019arXiv190506367B/abstract">
    <img src="https://img.shields.io/badge/Read-the_paper-orange.svg?style=flat">
  </a>
  <a href="https://VirtualPlanetaryLaboratory.github.io/vplanet/conduct.html">
    <img src="https://img.shields.io/badge/Code%20of-Conduct-black.svg">
  </a>
  <a href="https://www.youtube.com/@VPLanetCode/playlists">
    <img src="https://img.shields.io/badge/You-Tube-darkred.svg">
  </a>
   <a href="https://github.com/VirtualPlanetaryLaboratory/vplanet/discussions">
     <img src="https://img.shields.io/badge/Discussions-orange.svg">
  </a>
  <img src="https://github.com/VirtualPlanetaryLaboratory/vplanet/actions/workflows/pip-install.yml/badge.svg">
  <br>
  <img src="https://img.shields.io/badge/Unit%20Tests-19,599-darkblue.svg">
  <img src="https://github.com/VirtualPlanetaryLaboratory/vplanet/actions/workflows/tests-linux.yml/badge.svg">
  <img src="https://img.shields.io/badge/Ubuntu%2020-Python%203.6--3.11-7d93c7.svg">
  <img src="https://img.shields.io/badge/Ubuntu%2022-Python%203.7--3.11-7d93c7.svg">
  <a href="https://codecov.io/gh/VirtualPlanetaryLaboratory/vplanet">
    <img src="https://codecov.io/gh/VirtualPlanetaryLaboratory/vplanet/branch/main/graph/badge.svg?token=3LFJQO1M6H">
  </a>
  <br>
  <img src="https://github.com/VirtualPlanetaryLaboratory/vplanet/actions/workflows/tests-macos-intel.yml/badge.svg">
  <img src="https://img.shields.io/badge/MacOS%2012--13-Python%203.6--3.11-7d93c7.svg">
  <img src="https://github.com/VirtualPlanetaryLaboratory/vplanet/actions/workflows/tests-macos-silicon.yml/badge.svg">
  <img src="https://img.shields.io/badge/MacOS%2014-Python%203.10--3.11-7d93c7.svg">  
  <br>
  <img src="https://img.shields.io/badge/Test%20Sims-70-darkblue.svg">
  <img src="https://github.com/VirtualPlanetaryLaboratory/vplanet/actions/workflows/memcheck.yml/badge.svg">
  <img src="https://github.com/VirtualPlanetaryLaboratory/vplanet/actions/workflows/floatingpoint.yml/badge.svg">
  <img src="https://github.com/VirtualPlanetaryLaboratory/vplanet/actions/workflows/sanitizer.yml/badge.svg">
  <br>
  <a href="examples">
    <img src="https://img.shields.io/badge/Examples-41-darkblue.svg">
  </a>
  <img src="https://github.com/VirtualPlanetaryLaboratory/vplanet/actions/workflows/examples.yml/badge.svg">
  <img src="https://img.shields.io/badge/Ubuntu%2022-3.6%20--%203.11-7d93c7.svg">

</p>

### Overview

`VPLanet` is software to simulate planetary system evolution, with a focus on habitability. Physical models, typically consisting of ordinary differential equations, are coupled together to simulate evolution, from planetary cores to passing stars, for the age of a system. We strive for full transparency and reproducibility in our software, and this repository contains 1) the [source code](src), 2) [extensive documentation](https://VirtualPlanetaryLaboratory.github.io/vplanet), 3) scripts and files to [generate published figures](examples) and perform [parameter sweeps](https://virtualplanetarylaboratory.github.io/vplanet/parametersweep.html), and 4) [scripts to validate the current release](tests). We can't claim we found life beyond the Earth with closed-source or unreliable software!

To get started, ensure you have clang/gcc installed and follow the [Installation Guide](https://virtualplanetarylaboratory.github.io/vplanet/quickstart.html). You can also watch videos on our [YouTube channel](https://www.youtube.com/@VPLanetCode/playlists) on how to install and run `VPLanet`, as well as updates on recent results .

### Modules

`VPLanet` currently consists of 13 functioning "modules," each containing a set of equations
that simulates a specific physical process:

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

**Stellar**: Evolution of a star's bolometric and XUV luminosity, temperature, radius, and mass concentration. Also includes magnetic braking and stellar wind spin-down.

**ThermInt**: Thermal interior evolution, including magnetic fields, for planets
undergoing plate tectonics or stagnant lid evolution.

Many of these modules can be combined together to simulate numerous phenomena and feedback loops in planetary systems.

### Resources

The [examples/](examples) directory contains input files and scripts for generating the figures in [Barnes et al. (2020)](https://ui.adsabs.harvard.edu/abs/2020PASP..132b4502B/abstract) and subsequent publications. The [Manual/](Manual) directory contains the pdf of [Barnes et al. (2020)](https://ui.adsabs.harvard.edu/abs/2020PASP..132b4502B/abstract) that describes the physics of the first 11 modules, validates the software against observations and/or past results, and includes figures from the [examples/](examples) directory. 

An ecosystem of support software is also publicly available in other repositories of the [Virtual Planetary Laboratory](https://vpl.uw.edu/). The [vplot](https://github.com/VirtualPlanetaryLaboratory/vplot) package is both a command line tool to quickly plot the evolution of a single simulation and a Pythom module for generating publication-worthy figures. The [VSPACE](https://github.com/VirtualPlanetaryLaboratory/vspace) script generates input files for a parameter space sweep, which can then be performed on an arbitrary number of cores with [MultiPlanet](https://github.com/VirtualPlanetaryLaboratory/multi-planet). For large parameter sweeps, an enormous amount of data can be generated, which can slow analyses. To overcome this barrier, the [BigPlanet](https://github.com/VirtualPlanetaryLaboratory/bigplanet) code can both compress datasets into HDF5 format, including statistics of an integration, and tools to facilitate plotting. These three scripts can be executed from the command line to seamlessly [perform parameter sweeps](https://virtualplanetarylaboratory.github.io/vplanet/parametersweep.html). These Python scripts are optimized for [anaconda](https://www.anaconda.com/) distributions. 

The "pip-install" badge indicates if the latest executables are available for installation via pip for the Python distributions and operating systems for which we perform unit tests (see below). (*Note: This badge is currently broken due to an unknown GitHub issue, so if you pip install, you may want to check your `VPLanet` version, which is printed near the top of the log file that is generated when you perform a simulation.*) 

### Code Integrity

We are committed to maintaining a stable tool for scientists to analyze planetary system. Behind the scenes, the `VPLanet` team maintains code integrity through automatic checks at every merge into the main branch. You can see the status of these checks via the badges above. Currently we perform "Unit Tests" for the initial and final conditions across an orthogonal set of "Test Sims" (simulations), with the numbers of tests for each shown via badges. We perform the tests across all permutations of operating systems and Python version shown by the badges. 

We also check for memory violations via [valgrind's memcheck tool](http://valgrind.org) ("memcheck") and [address sanitizer](https://github.com/google/sanitizers/wiki/AddressSanitizer) ("sanitizer"), check for overflow, invalid operation, and divide-by-zero floating point exceptions ("floating-point"), and test if all the [examples](examples/) work across the operating system and Python versions listed after the "examples" badge. 

The percentage of the lines of code that are executed by the unit tests is shown with the "codecov" badge, with details available at our <a href="https://codecov.io/gh/VirtualPlanetaryLaboratory/vplanet">Codecov</a> account. 

### Community

`VPLanet` is a community project. We're happy to take pull requests; if you want to create one, please issue it to the *dev* branch. The documentation includes [tutorials on adding new features and modules](https://VirtualPlanetaryLaboratory.github.io/vplanet/tutorials.html). `VPLanet` is a platform for planetary science that can grow exponentially, either by adding new physics or by adding competing models for clean comparisons.

A list of additional GitHub repositories with `VPLanet` examples can be found [here](https://VirtualPlanetaryLaboratory.github.io/vplanet/repos.html).

If you have questions or are running into issues, you can read or post to a [Discussion](discussions).

If you believe you have encountered a bug, please raise an issue using the [Issues](https://github.com/VirtualPlanetaryLaboratory/vplanet/issues) tab at the top of this page.

### Acknowledgments

If you use this code to generate results used in any publication or conference contribution, please cite [Barnes, R. et al. (2020), PASP, 132, 24502](https://ui.adsabs.harvard.edu/abs/2020PASP..132b4502B/abstract).

`VPLanet` development has been supported by NASA grants NNA13AA93A, NNX15AN35G, 80NSSC17K048, 13-13NAI7_0024, 80NSSC20K0229, and 80NSSC20K0261. We also acknowledge support from the University of Washington and the Carnegie Institute for Science.

Enjoy!

© 2018-2024 The VPLanet Team.

# vplanet
[![Build Status](https://travis-ci.com/VirtualPlanetaryLaboratory/vplanet.svg?token=jABaFLLgJNHTWSqkT7CM&branch=master)](https://travis-ci.com/VirtualPlanetaryLaboratory/vplanet)
[![Documentation](https://img.shields.io/badge/read-the_docs-blue.svg?style=flat)](https://VirtualPlanetaryLaboratory.github.io/vplanet)

© 2018 The VPLANET Team.

`vplanet` is software to simulate planetary system evolution, with a focus on habitability. Physical models, typically consisting of ordinary differential equations, are coupled together to simulate evolution for the age of a system. We strive for full transparency and reproducibility in our software, and this repository contains the source code, extensive documentation, and the scripts and files to generate published figures and validate the current release. We can't claim we found aliens with closed source software!

To get started, read the docs and follow the QuickStart guide.

`vplanet` is a community project. We're happy to take pull requests; if you want to create one, please issue it to the dev branch. Soon we will include tutorials on adding new input options, governing variables, and modules. It's a platform for planetary science that can grow exponentially, either by adding new physics or by adding competing models for clean comparisons. 

The examples/ directory contains input files and scripts for generating the figures in Barnes et al. (2018) and all subsequent module descriptions. The Manual/ directory contains the source code to generate a pdf of Barnes et al. (2018) plus methods sections from subsequent modules. This pdf describes the physics of each module, validates each module against observations or past results, and uses figures from the examples/ directory. [Soon]

An ecosystem of support software is also publicly available. In this repo, vspace/ contains scripts to generate input files for a parameter space sweep. bigplanet/ contains scripts to store large datasets in HDF5 format and quickly calculate summary properties from an integration, like change in surface temperature. In a separate repository is `vplot`, which consists of both a command line tool to quickly plot the evolution of a system, but also matplotlib functions to more easily generate publication-worthy figures. Finally, we recommend using `approxposterior` to quickly obtain posterior distributions of model parameters.

Behind the scenes, the vplanet team maintains code integrity through continuous integration, in which numerous scientific and numerical tests are validated at every commit. See the tests/ directory for the validation checks that the current build passes. Additionally, we use valgrind and addresssanitizer to periodically search for memory issues like leaks, accessing memory beyond array bounds, etc. We are committed to maintaining a stable tool that planetary scientists can rely on for fast analyses of the Solar System and beyond.  

`vplanet` development has been supported by NASA grants NNA13AA93A, NNX15AN35G, and 13-13NAI7_0024. We also acnkowledge support from the University of Washington and the Carnegie Institute for Science.

Enjoy!

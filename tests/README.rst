VPLANET Tests
-----

This directory contains the testing information for the continuous integration (CI)
checks. You can run the checks locally with the command

make test

but you must have `vplot <https://github.com/VirtualPlanetaryLaboratory/vplot>`_ installed and in your PATH.

**DO NOT MODIFY THESE FILES UNLESS YOU KNOW WHAT YOU ARE DOING!!**

Every time somebody issues a pull request to the master branch, a `Travis CI <https://travis-ci.org/>`_
check is automatically triggered. Briefly, this check compiles and runs **VPLanet**
on all the subdirectories listed here and compares the results to the expected
behavior. This quality control ensures that as changes are made to the code base,
the outcomes of the simulations match expectations. Each check here corresponds
to a figure in the `examples <../examples>`_ directory, so if the tests are passing, 
then all figures should be reproduced by **VPLanet**.

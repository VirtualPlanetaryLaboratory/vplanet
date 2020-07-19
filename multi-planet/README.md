# Multi-Planet: Multicore Simulations with VPLanet

Use Multi-Planet to run a large number of simulations with more than one core. `Multi-planet` is designed to work with `vspace`.

NOTE: This script is still experimental!

## Multi-Planet Setup

Prior to running Multi-planet, it must be added to your $PATH. To do so, navigate to the multi-planet directory and type the following command on the command line:
```
python setup.py
```

##Running Multi-planet
To run Multi-Planet on a large number of simulations, first type  `vspace <input file>` to create the simulation folders that Multi-Planet will run. Afterwards, type the following:
```
multi-planet <input file> <number of cores>
```

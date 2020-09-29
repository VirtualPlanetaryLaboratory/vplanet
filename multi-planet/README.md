# Multi-Planet: Multicore Simulations with VPLanet

Use Multi-Planet to run a large number of simulations with more than one core. `Multi-planet` is designed to work with `vspace`.

## Multi-Planet Setup

Prior to running Multi-planet, it must be added to your $PATH. To do so, navigate to the multi-planet directory and type the following command on the command line:
```
python setup.py
```

## Running Multi-planet

To run Multi-Planet on a large number of simulations, first type  `vspace <input file>` to create the simulation folders that Multi-Planet will run. Afterwards, type the following:
```
multi-planet <input file> <number of cores>
```
where `<input file>` is the same for `vspace` and `multi-planet`.


## Checking the status of Multi-planet

To check the status of multi-planet type `mpstatus` in the directory that is currently running multi-planet. The following will be printed to the command line:
```
--Multi-Planet Status--
Number of Simulations completed:
Number of Simulations in progress:
Number of Simulations remaining:
```

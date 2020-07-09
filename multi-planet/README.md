# Multi-Planet: using multiproccesing with Vplanet

This program, dubbed 'Multi-Planet' works in conjuction with the Vplanet code found [here on Github](https://github.com/VirtualPlanetaryLaboratory/vplanet).

## Installation of Multi-Planet

Multi-planet will work if installed like so:

```
git clone https://github.com/caitlyn-wilhelm/vplanet-multiprocess
```

This assumes you already have Vplanet installed and are familiar with the vspace functionality.

## Multi-Planet Formatting

There are two separate options for running Multi-Planet: A singular case or a list of cases given by the user. Regardless, there is a specific type of format for the files for Multi-Planet to run smoothly. In each case, there should be a vspace file named `vspace.in` like so:

![Formatting Guide Image](https://github.com/caitlyn-wilhelm/vplanet-multiprocess/blob/master/docs/format_guide.png)

## Singular Case with Muli-Planet

Using the singular case option with multi_planet is simple. Just run this line of code on the command line:

```
python multi_process.py [Case_Name] [Number of Cores]
```

### Multiple Cases with Multi-planet

Let's say you have a list of cases to run. Inside the `vplanet/multiprocess` repo, there is file titled `multi_list.txt`. List the same arguments you would use for the singular case like so:
```
[Path to Case A] [Number of Cores]
[Path to Case B] [Number of Cores]
[Path to Case C] [Number of Cores]
```

After setting up the `multi_list.txt` file correctly, run the following line in the command line:

```
./wrapper.sh
```

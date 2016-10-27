vemcee is a tool designed to search large parameter spaces for combinations of input parameters that
match a desired output. This is controlled by defining a list of input parameters and their acce

This tool is essentially a wrapper for the "brains" provided by the python module emcee, a marakov
-chain monte-carlo thingy. You can read the (frustratingly minimal) documentation on the page here:
http://dan.iel.fm/emcee/. The gist is that the parameter space is explored by "walkers" who search
for the extrema of some fitness function (or statistical distribution) in a smart way.

To run this program, modify the vemcee.ini file to reflect the runtime conditions you desire, and
call the program by typing "python vemcee.py" in the directory.

vemcee is a tool designed to find what set of input parameters result in behavior similar
to a desired test case. For example, modifying the reference viscosity of the mantle to
produce a core similar in size to that of Earth today. This tool was created to efficiently
explore the parameter space when the number of degrees of freedom are too large to be efficiently
sampled by hand.

This tool is essentially a wrapper for the "brains" provided by the python module emcee, a marakov
-chain monte-carlo thingy. You can read the (frustratingly minimal) documentation on the page here:
http://dan.iel.fm/emcee/. The gist is that the parameter space is explored by "walkers" who search
for the extrema of some fitness function (or statistical distribution) in a smart way.

To run this program, modify the vemcee.ini file to reflect the runtime conditions you desire, and
call the program by typing "python vemcee.py" in the directory.

This tool was written by Ben Guyer (benguyer@uw.edu) some undergraduate with no "real" training in
coding. Apologies for whatever coding standards are broken.

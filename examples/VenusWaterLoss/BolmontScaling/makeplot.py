from __future__ import division, print_function, absolute_import, \
    unicode_literals
import matplotlib.pyplot as pl
import numpy as np
import vplot as vpl
import sys

pl.rcParams.update({'font.size': 16})

# Check correct number of arguments
if (len(sys.argv) != 2):
    print('ERROR: Incorrect number of arguments.')
    print('Usage: '+sys.argv[0]+' <pdf | png>')
    exit(1)
if (sys.argv[1] != 'pdf' and sys.argv[1] != 'png'):
    print('ERROR: Unknown file format: '+sys.argv[1])
    print('Options are: pdf, png')
    exit(1)

output = vpl.GetOutput()
time = output.sun.Age
planets = output.bodies[1:]
N = len(planets)
eps = np.array([planet.AtmXAbsEffH2O for planet in planets])
FXUV = np.array([planet.FXUV for planet in planets])

# Plot
fig, ax = pl.subplots(1,1)
fig.subplots_adjust(wspace=0.2)
lines = [None for i in range(N)]

lines[0], = ax.plot(FXUV[0], eps[0])
ax.plot(FXUV[0], eps[0], color = 'black')


# Tweaks
ax.set_xlabel('XUV Flux Received by Planet (W/m$^2$)')
ax.set_ylabel('XUV Atmospheric Escape Efficiency for H$_2$O')

if (sys.argv[1] == 'pdf'):
    fig.savefig('../BolmontScaling.pdf', bbox_inches="tight")
if (sys.argv[1] == 'png'):
    fig.savefig('../BolmontScaling.png', bbox_inches="tight")

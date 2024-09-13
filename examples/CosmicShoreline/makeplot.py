import os
import pathlib
import subprocess
import sys

import matplotlib as mpl
import matplotlib.pyplot as plt
import numpy as np
import vplot as vpl
from matplotlib.ticker import MaxNLocator
from mpl_toolkits.axes_grid1 import make_axes_locatable

import vplanet

path = pathlib.Path(__file__).parents[0].absolute()
sys.path.insert(1, str(path.parents[0]))

output = vplanet.run(units = False)

# Plot!
fig = plt.figure(figsize=(8.5, 6))

fxuv_earth = output.log.final.Earth.CumulativeXUVFlux

fxuv = []
fxuv.append(output.log.final.Mercury.CumulativeXUVFlux/fxuv_earth)
fxuv.append(output.log.final.Venus.CumulativeXUVFlux/fxuv_earth)
fxuv.append(output.log.final.Earth.CumulativeXUVFlux/fxuv_earth)
fxuv.append(output.log.final.Mars.CumulativeXUVFlux/fxuv_earth)
fxuv.append(output.log.final.Jupiter.CumulativeXUVFlux/fxuv_earth)
fxuv.append(output.log.final.Saturn.CumulativeXUVFlux/fxuv_earth)
fxuv.append(output.log.final.George.CumulativeXUVFlux/fxuv_earth)
fxuv.append(output.log.final.Neptune.CumulativeXUVFlux/fxuv_earth)

escvel = []
escvel.append(output.log.final.Mercury.EscapeVelocity/1e3)
escvel.append(output.log.final.Venus.EscapeVelocity/1e3)
escvel.append(output.log.final.Earth.EscapeVelocity/1e3)
escvel.append(output.log.final.Mars.EscapeVelocity/1e3)
escvel.append(output.log.final.Jupiter.EscapeVelocity/1e3)
escvel.append(output.log.final.Saturn.EscapeVelocity/1e3)
escvel.append(output.log.final.George.EscapeVelocity/1e3)
escvel.append(output.log.final.Neptune.EscapeVelocity/1e3)

shorelinex = []
shorelinex.append(0.2)
shorelinex.append(60)

shoreliney = []
shoreliney.append(1e-6)
shoreliney.append(1e4)

plt.xlabel('Escape Velocity [km/s]')
plt.ylabel('Normalized Cumulative XUV Flux')
plt.plot(shorelinex,shoreliney,color=vpl.colors.pale_blue)
plt.plot(escvel,fxuv,'o',color='k')
plt.xscale('log')
plt.yscale('log')
plt.ylim(1e-6,1e4)
plt.xlim(0.1,200)

plt.annotate('Mercury',(2.7,9))
plt.annotate('Venus',(10.2,2.6))
plt.annotate('Earth',(11.5,0.5))
plt.annotate('Mars',(5,0.2))
plt.annotate('Jupiter',(60,0.05))
plt.annotate('Saturn',(37,0.014))
plt.annotate('Uranus',(22,0.0034))
plt.annotate('Neptune',(24,0.0006))


# Save figure
fig.savefig(path / f"CosmicShoreline.png", bbox_inches="tight", dpi=200)

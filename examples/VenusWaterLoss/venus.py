from __future__ import division, print_function, absolute_import, \
    unicode_literals
import matplotlib.pyplot as pl
import numpy as np
import vplot as vpl

output = vpl.GetOutput()
time = output.sun.Age
planets = output.bodies[1:]
N = len(planets)
water = np.array([planet.SurfWaterMass for planet in planets])
oxygen = np.array([planet.OxygenMantleMass for planet in planets])

# Plot
fig, ax = pl.subplots(1, 2, figsize=(12, 3))
fig.subplots_adjust(wspace=0.2)
lines = [None for i in range(N)]
for i in range(N):
    lines[i], = ax[0].plot(time, water[i])
    ax[1].plot(time, oxygen[i])

# Legend
leg = ax[0].legend(lines, ('1 TO', '3 TO', '5 TO'),
                   loc=('upper right'),
                   handletextpad=0.1,
                   title="Initial water")
pl.setp(leg.get_title(), fontsize=12, fontweight='bold')

# Watson et al (1981) result
ax[0].axvline(280e6, color='k', lw=1, ls='--')
ax[0].annotate("Watson+81 timescale", xy=(280e6, 1), xycoords="data",
               textcoords=("offset points"), xytext=(5, 0),
               ha="left", va="center", fontsize=12, color='k')

# Tweaks
ax[0].set_xlabel('Time (years)')
ax[1].set_xlabel('Time (years)')
ax[0].set_ylabel('Surface Water (TO)')
ax[1].set_yscale('log')
ax[1].set_ylabel('Absorbed O$_2$ (bar)')
ax[0].set_xlim(5e7, 4.6e9)
ax[1].set_xlim(5e7, 4.6e9)
for axis in ax.flatten():
    axis.set_xscale('log')

fig.savefig('venus.pdf', bbox_inches='tight')

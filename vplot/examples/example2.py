#!/usr/bin/env python
# -*- coding: utf-8 -*-
'''
example2.py
-----------

'''

from __future__ import division, print_function, absolute_import, unicode_literals
import vplot as vpl
import matplotlib.pyplot as plt

# Grab the output from a run
output = vpl.GetOutput('solarsys')

# Set up a matplotlib plot as usual
fig, ax = plt.subplots(2, 2, figsize = (12, 8))

# Now use ``vpl.plot`` instead of ``plt.plot`` to do the
# plotting to get customized VPLOT plots. You can specify
# keyword arguments in the same way you would for ``plt.plot``.
for planet in [output.Earth, output.Jupiter]:
  vpl.plot(ax[0,0], 100 * planet.Time, planet.Eccentricity)
  vpl.plot(ax[0,1], 100 * planet.Time, planet.Inc)


vpl.plot(ax[1,0], output.Earth.Latitude[0], output.Earth.TempLat[0])
vpl.plot(ax[1,1], output.Earth.Time, output.Earth.IceHeight[:,0])

# Add legends
ax[0,0].legend(); ax[0,0].set_ylim(0, 0.08)
ax[0,1].legend(loc = 'upper left')
ax[1,0].legend()
ax[1,1].legend(loc = 'lower right')

# Add a title
plt.suptitle('VPLOT Example 2')

# Show it
vpl.show()
#!/usr/bin/env python
# -*- coding: utf-8 -*-
'''
example1.py
-----------

'''

from __future__ import division, print_function, absolute_import, unicode_literals
import vplot as vpl
import matplotlib.pyplot as plt

# Grab the output from a run
output = vpl.GetOutput('gl581')

# Set up a matplotlib plot as usual
fig, ax = plt.subplots(2, 2, figsize = (12, 8))

# Now use ``vpl.plot`` instead of ``plt.plot`` to do the
# plotting to get customized VPLOT plots. You can specify
# keyword arguments in the same way you would for ``plt.plot``.
for planet in [output.c, output.d]:
  vpl.plot(ax[0,0], output.star.Time, planet.Eccentricity)
  vpl.plot(ax[0,1], output.star.Time, planet.Obliquity)

vpl.plot(ax[1,0], output.c.Eccentricity, output.c.Obliquity)
vpl.plot(ax[1,1], output.d.Eccentricity, output.d.Obliquity)

# Add legends
ax[0,0].legend(); ax[0,0].set_ylim(0, 0.6)
ax[0,1].legend()

# Add a title
plt.suptitle('VPLOT Example 1')

# Show it
vpl.show()
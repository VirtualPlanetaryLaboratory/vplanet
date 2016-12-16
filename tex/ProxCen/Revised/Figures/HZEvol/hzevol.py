#!/usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import division, print_function, absolute_import, unicode_literals
import matplotlib.pyplot as plt
import vplot as vpl
from pylab import *

# Grab the output from a run
output = vpl.GetOutput('/Users/rory/Exoplanets/systems/ProxCen/HZEvol')
#fig = plt.figure()
#ax = fig.add_subplot(111)

#for axis in ['top','bottom','left','right']:
#  ax.spines[axis].set_linewidth(5)

rc('axes',linewidth=2)
plt.rcParams['xtick.major.pad']='8'
plt.rcParams['ytick.major.pad']='5'
plt.rcParams['xtick.major.size'] = 10
plt.rcParams['xtick.major.width'] = 2
plt.rcParams['xtick.minor.size'] = 5
plt.rcParams['xtick.minor.width'] = 1
plt.rcParams['ytick.major.size'] = 10
plt.rcParams['ytick.major.width'] = 2
plt.rcParams['ytick.minor.size'] = 5
plt.rcParams['ytick.minor.width'] = 1

plt.figure(figsize=(7,6), dpi=300)

# Make font size smaller
plt.rcParams.update({'font.size': 18})

# Now use ``vpl.plot`` instead of ``plt.plot`` to do the
# plotting to get customized VPLOT plots. You can specify
# keyword arguments in the same way you would for ``plt.plot``.
vpl.plot(plt.gca(),output.proxima.Time,output.proxima.HZLimMoistGreenhouse,color=vpl.colors.pale_blue,label='')
vpl.plot(plt.gca(),output.proxima.Time,output.proxima.HZLimMaxGreenhouse,color=vpl.colors.pale_blue,label='')

fbk = {'lw':0.0, 'edgecolor':None}
plt.fill_between(output.proxima.Time,output.proxima.HZLimMoistGreenhouse,output.proxima.HZLimMaxGreenhouse,facecolor=vpl.colors.pale_blue,**fbk)

output = vpl.GetOutput('/Users/rory/Exoplanets/systems/ProxCen/DryRunaway')

vpl.plot(plt.gca(),output.a0.Time,output.a0.HZLimitDryRunaway,color=vpl.colors.red,linestyle='dotted',label='A=0')
vpl.plot(plt.gca(),output.a0.Time,output.a05.HZLimitDryRunaway,color=vpl.colors.red,linestyle='dashed',label='A=0.5')
vpl.plot(plt.gca(),output.a0.Time,output.a08.HZLimitDryRunaway,color=vpl.colors.red,label='A=0.8')

# Fuss with figures
plt.ylabel('Orbital Distance (AU)')
#plt.setp(ax.spines.values(), linewidth=5)
plt.plot([0,1e9],[0.387,0.387],linestyle='dashed',color='k',lw=2)
plt.plot([0,1e9],[0.0485,0.0485],linestyle='solid',color='k',lw=2)
plt.text(1.1e6,0.027,'Orbit of Proxima b',color='k',fontsize=15)
plt.text(1e8,0.365,'Orbit of Mercury',color='k',fontsize=15)
plt.text(2e6,0.28,'Habitable Zone',color='w',rotation=320)
plt.text(2e6,0.14,'Dry Limits',color=vpl.colors.red,rotation=335,fontsize=15)
plt.legend(loc='best',fontsize=15)

plt.ylim(0,0.5)
plt.xlim(1e6,1e9)
plt.xscale('log')
plt.xlabel('Time (years)')

# Show it
#vpl.show()

plt.tight_layout()
plt.savefig('hzevol.pdf')

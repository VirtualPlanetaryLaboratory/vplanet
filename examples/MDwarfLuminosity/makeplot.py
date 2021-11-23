"""
This script produces the Figures 5 from Amaral+2021, the
Stellar luminosity evolution of M dwarf stars between 0.2 and 0.6 Msun,
 using VPLANET's STELLAR and FLARE modules.

Laura N. R. do Amaral, Universidad Nacional Autónoma de México, 2021
Date:  July 26th 2021
"""
import matplotlib.pyplot as plt
import matplotlib as mpl
import numpy as np
import sys
import os
import subprocess
from matplotlib.patches import Patch
from matplotlib.lines import Line2D
import pathlib
import vplanet

path = pathlib.Path(__file__).parents[0].absolute()
sys.path.insert(1, str(path.parents[0]))
from get_args import get_args
mpl.style.use('classic')


run = ['./LumEvolFlareFFD/vpl.in','./LumEvolStellar/vpl.in']

for i in range(0,2):
           vplanet.run(path / run[i], units=False)


# Check correct number of arguments
'''if (len(sys.argv) != 2):
    print('ERROR: Incorrect number of arguments.')
    print('Usage: '+sys.argv[0]+' <pdf | png>')
    exit(1)
if (sys.argv[1] != 'pdf' and sys.argv[1] != 'png'):
    print('ERROR: Unknown file format: '+sys.argv[1])
    print('Options are: pdf, png')
    exit(1)'''

#Typical plot parameters that make for pretty plot
mpl.rcParams['figure.figsize'] = (11,11)
mpl.rcParams['font.size'] = 15.0


mpl.rcParams['xtick.minor.size'] = 5
#mpl.rcParams['xtick.minor.width'] = 1
mpl.rcParams['ytick.minor.size'] = 5
#mpl.rcParams['ytick.minor.width'] = 1

mpl.rcParams['xtick.direction'] = 'in'
mpl.rcParams['ytick.direction'] = 'in'
mpl.rcParams['xtick.top'] = True
mpl.rcParams['xtick.bottom'] = True
mpl.rcParams['ytick.right'] = True



# Run the simulations

path = ["./LumEvol_flare_FFD/", "./LumEvol_Stellar/"]

# Return to top-level directory

flare = ['./LumEvolFlareFFD/FlareEvol.fstar_02.forward','./LumEvolFlareFFD/FlareEvol.fstar_03.forward','./LumEvolFlareFFD/FlareEvol.fstar_04.forward',
         './LumEvolFlareFFD/FlareEvol.fstar_05.forward','./LumEvolFlareFFD/FlareEvol.fstar_06.forward']
stellar = ['./LumEvolStellar/StellarEvol.sstar_02.forward','./LumEvolStellar/StellarEvol.sstar_03.forward','./LumEvolStellar/StellarEvol.sstar_04.forward',
         './LumEvolStellar/StellarEvol.sstar_05.forward','./LumEvolStellar/StellarEvol.sstar_06.forward']


ftime = []
fage = []
flumbol = []
flumxuv = []
flumtot = []              
flumflare = []   

stime = []
sage = []
slumbol = []
slumxuv = []
slumtot = []              
slumflare = []   

for i in range(0,5):
    ftime.append(np.genfromtxt(flare[i],usecols = (0),unpack=True))
    fage.append(np.genfromtxt(flare[i],usecols = (8),unpack=True))
    flumbol.append(np.genfromtxt(flare[i],usecols = (1),unpack=True))
    flumxuv.append(np.genfromtxt(flare[i],usecols = (6),unpack=True))
    flumtot.append(np.genfromtxt(flare[i],usecols = (5),unpack=True))
    flumflare.append(np.genfromtxt(flare[i],usecols = (7),unpack=True))

for i in range(0,5):
    stime.append(np.genfromtxt(stellar[i],usecols = (0),unpack=True))
    sage.append(np.genfromtxt(stellar[i],usecols = (7),unpack=True))
    slumbol.append(np.genfromtxt(stellar[i],usecols = (1),unpack=True))
    slumxuv.append(np.genfromtxt(stellar[i],usecols = (6),unpack=True))
    slumtot.append(np.genfromtxt(stellar[i],usecols = (5),unpack=True))


# Plot
fig, axes = plt.subplots(nrows=3, ncols=2, sharex=True, figsize=(10,12))
fig.subplots_adjust(right = 1.0, left = 0.05, bottom = 0.05, top = 0.91, wspace = 0.03, hspace =0.2 )

time02s = sage[0]
time03s = sage[1]
time04s = sage[2]
time05s = sage[3]
time06s = sage[4]


time02f = fage[0]
time03f = fage[1]
time04f = fage[2]
time05f = fage[3]
time06f = fage[4]

import seaborn as sns
cmap = sns.color_palette("inferno", n_colors=5)

## Upper left: Bolometric Luminosity #
axes[0,0].plot(time02s, slumbol[0], color = cmap[0], alpha = 0.6, lw = 3)
axes[0,0].plot(time03s, slumbol[1], color = cmap[1], alpha = 0.6, lw = 3)
axes[0,0].plot(time04s, slumbol[2], color = cmap[2], alpha = 0.6, lw = 3)
axes[0,0].plot(time05s, slumbol[3], color = cmap[3], alpha = 0.6, lw = 3)
axes[0,0].plot(time06s, slumbol[4], color = cmap[4], alpha = 0.6, lw = 3)

axes[0,0].set_xscale('log')
axes[0,0].set_yscale("log")
axes[0,0].set_ylabel(r"Bolometric Luminosity ($L_\odot$)", fontsize = 18)
axes[0,0].set_title('a',fontsize=30, loc="left")

# center left: XUV luminosity by flares
axes[1,0].plot(time02f, flumflare[0],color = cmap[0], alpha = 0.6, lw = 3)
axes[1,0].plot(time03f, flumflare[1],color = cmap[1], alpha = 0.6, lw = 3)
axes[1,0].plot(time04f, flumflare[2],color = cmap[2], alpha = 0.6, lw = 3)
axes[1,0].plot(time05f, flumflare[3],color = cmap[3], alpha = 0.6, lw = 3)
axes[1,0].plot(time06f, flumflare[4],color = cmap[4], alpha = 0.6, lw = 3)

axes[1,0].set_yscale("log")
axes[1,0].set_ylabel(r"XUV Flare Luminosity  ($L_\odot$)", fontsize = 18)
axes[1,0].set_title('c',fontsize=30, loc="left")

# Down left: XUV luminosity by flares / XUV quiescent luminosity
axes[2,0].plot(time02s, flumflare[0]/flumbol[0], color = cmap[0], alpha = 0.6, lw = 3)#, label = r'0.2M$_{\odot}$ STELLAR')
axes[2,0].plot(time03s, flumflare[1]/flumbol[1], color = cmap[1], alpha = 0.6, lw = 3)#, label = r'0.3M$_{\odot}$ STELLAR')
axes[2,0].plot(time04s, flumflare[2]/flumbol[2], color = cmap[2], alpha = 0.6, lw = 3)#, label = r'0.4M$_{\odot}$ STELLAR')
axes[2,0].plot(time05s, flumflare[3]/flumbol[3], color = cmap[3], alpha = 0.6, lw = 3)#, label = r'0.5M$_{\odot}$ STELLAR')
axes[2,0].plot(time06s, flumflare[4]/flumbol[4], color = cmap[4], alpha = 0.6, lw = 3)#, label = r'0.6M$_{\odot}$ STELLAR')

axes[2,0].set_title('e',fontsize=30, loc="left")
axes[2,0].set_yscale("log")
axes[2,0].set_xscale("log")
axes[2,0].set_xlabel("Stellar Age (year)", fontsize=22)
axes[2,0].set_ylabel(r'L$_{XUV,flare}$/L$_{bol}$ ($L_\odot$)', fontsize = 22)

# Upper right: XUV quiescent luminosity
axes[0,1].plot(time02s, slumxuv[0], color = cmap[0], alpha = 0.6, lw = 3, label = r'0.2M$_{\odot}$')
axes[0,1].plot(time03s, slumxuv[1], color = cmap[1], alpha = 0.6, lw = 3, label = r'0.3M$_{\odot}$')
axes[0,1].plot(time04s, slumxuv[2], color = cmap[2], alpha = 0.6, lw = 3, label = r'0.4M$_{\odot}$')
axes[0,1].plot(time05s, slumxuv[3], color = cmap[3], alpha = 0.6, lw = 3, label = r'0.5M$_{\odot}$')
axes[0,1].plot(time06s, slumxuv[4], color = cmap[4], alpha = 0.6, lw = 3, label = r'0.6M$_{\odot}$')

axes[0,1].legend(loc='lower left', fontsize = 13, ncol=1)
axes[0,1].set_yscale("log")
axes[0,1].set_xscale("log")
axes[0,1].set_ylabel(r'L$_{XUV,quiescent}$ ($L_\odot$)', fontsize = 22)
axes[0,1].set_title('b',fontsize=30, loc="left")
axes[0,1].set_xlabel('  ')

## center right: Total XUV luminosity (flares + quiescent) ##
axes[1,1].plot(time02s, slumtot[0], color = cmap[0], alpha = 0.6, lw = 4)
axes[1,1].plot(time03s, slumtot[1], color = cmap[1], alpha = 0.6, lw = 4)
axes[1,1].plot(time04s, slumtot[2], color = cmap[2], alpha = 0.6, lw = 4)
axes[1,1].plot(time05s, slumtot[3], color = cmap[3], alpha = 0.6, lw = 4)
axes[1,1].plot(time06s, slumtot[4], color = cmap[4], alpha = 0.6, lw = 4)

axes[1,1].plot(time02f, flumtot[0], '--',color = cmap[0], alpha = 0.99)
axes[1,1].plot(time03f, flumtot[1], '--',color = cmap[1], alpha = 0.99)#, label = r'0.3M$_{\odot}$')
axes[1,1].plot(time04f, flumtot[2], '--',color = cmap[2], alpha = 0.99)#, label = r'0.4M$_{\odot}$')
axes[1,1].plot(time05f, flumtot[3], '--',color = cmap[3], alpha = 0.99)#, label = r'0.5M$_{\odot}$')
axes[1,1].plot(time06f, flumtot[4], '--',color = cmap[4], alpha = 0.99)#, label = r'0.6M$_{\odot}$')

axes[1,1].set_yscale("log")
axes[1,1].set_ylabel(r"XUV Luminosity  ($L_\odot$)", fontsize=18)
axes[1,1].set_title('d',fontsize=30, loc="left")


## Down left:LXUVTot ##
axes[2,1].plot(time02s, flumflare[0]/flumxuv[0],color = cmap[0], alpha = 0.6, lw = 3)#,label = r'0.2M$_{\odot}$ STELLAR')
axes[2,1].plot(time03s, flumflare[1]/flumxuv[1], color = cmap[1], alpha = 0.6, lw = 3)#, label = r'0.3M$_{\odot}$ STELLAR')
axes[2,1].plot(time04s, flumflare[2]/flumxuv[2], color = cmap[2], alpha = 0.6, lw = 3)#, label = r'0.4M$_{\odot}$ STELLAR')
axes[2,1].plot(time05s, flumflare[3]/flumxuv[3], color = cmap[3], alpha = 0.6, lw = 3)#,label = r'0.5M$_{\odot}$ STELLAR')
axes[2,1].plot(time06s, flumflare[4]/flumxuv[4], color = cmap[4], alpha = 0.6, lw = 3)#, label = r'0.6M$_{\odot}$ STELLAR')

axes[2,1].set_yscale("log")
axes[2,1].set_xscale("log")
axes[2,1].set_ylabel(r'L$_{XUV,flare}$/L$_{XUV,quiescent}$ ($L_\odot$)', fontsize = 18)
axes[2,1].set_title('f',fontsize=30, loc="left")
axes[2,1].set_xlabel('  ')
axes[2,1].set_xlabel("Stellar Age (year)", fontsize=22)



legend_elements = [Line2D([0], [0], ls='-', color='black',lw=2, label=r'L$_{XUV,quiescent}$'),
                   Line2D([0], [0], ls='--', color='black', lw=2,label=r'L$_{XUV,flare}$ + L$_{XUV,quiescent}$'),

]

plt.legend(handles=legend_elements,bbox_to_anchor=(0.4,1.18), loc="lower center", ncol=1, fontsize = 12)

# Format all axes
for ax in axes.flatten():

    # Format x axis
    ax.set_xlim(time02s[1],time02s.max())
    ax.set_xscale("log")
    # Set rasterization
    ax.set_rasterization_zorder(0)
    for tick in ax.xaxis.get_major_ticks() + ax.yaxis.get_major_ticks():
        tick.label.set_fontsize(23)

fig.tight_layout()
fig.subplots_adjust(wspace=0.41)
fig.subplots_adjust(hspace=0.18)



if (sys.argv[1] == 'pdf'):
    fig.savefig('MDwarfLuminosity.pdf', bbox_inches="tight", dpi=600)
if (sys.argv[1] == 'png'):
    fig.savefig('MDwarfLuminosity.png', bbox_inches="tight", dpi=600)

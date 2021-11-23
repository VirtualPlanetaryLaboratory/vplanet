"""
This script produces a version of Figure 10 from Davenport+2019, the
flare frequency distribution for Proxima Centauri, using VPLANET's STELLAR and FLARE modules.

Laura N.  R. do Amaral, Universidad Nacional Autónoma de México, 2021
date: Sep. 2021
"""
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.colors as colors
from math import exp, expm1, log10, log
import matplotlib.style
import seaborn as sns
from scipy.interpolate import interp1d
import matplotlib as mpl
import sys
import os
import subprocess
import pathlib
import vplanet

path = pathlib.Path(__file__).parents[0].absolute()
sys.path.insert(1, str(path.parents[0]))
from get_args import get_args
mpl.style.use('classic')


run = ['./1Myr/vpl.in','./10Myr/vpl.in','./100Myr/vpl.in','./1000Myr/vpl.in','./lacy/vpl.in']

for i in range(0,5):
           vplanet.run(path / run[i], units=False)

M = np.loadtxt("./FfdLacy.lacy.forward")

MD = np.loadtxt("./FfdDaven1.daven1Myr.forward")
ND = np.loadtxt("./FfdDaven10.daven10Myr.forward")
OD = np.loadtxt("./FfdDaven100.daven100Myr.forward")
PD = np.loadtxt("./FfdDaven1000.daven1000Myr.forward")


try:
    import vplot as vpl
except:
    print('Cannot import vplot. Please install vplot.')

# Check correct number of arguments
if (len(sys.argv) != 2):
    print('ERROR: Incorrect number of arguments.')
    print('Usage: '+sys.argv[0]+' <pdf | png>')
    exit(1)
if (sys.argv[1] != 'pdf' and sys.argv[1] != 'png'):
    print('ERROR: Unknown file format: '+sys.argv[1])
    print('Options are: pdf, png')
    exit(1)

#Typical plot parameters that make for pretty plot
mpl.rcParams['figure.figsize'] = (7,10)
mpl.rcParams['font.size'] = 18.0

mpl.rcParams['xtick.major.size'] = 7
mpl.rcParams['xtick.major.width'] = 2
mpl.rcParams['ytick.major.size'] = 7
mpl.rcParams['ytick.major.width'] = 2

mpl.rcParams['xtick.minor.size'] = 5
mpl.rcParams['xtick.minor.width'] = 2
mpl.rcParams['ytick.minor.size'] = 5
mpl.rcParams['ytick.minor.width'] = 2

mpl.rcParams['xtick.direction'] = 'in'
mpl.rcParams['ytick.direction'] = 'in'
mpl.rcParams['xtick.top'] = True
mpl.rcParams['xtick.bottom'] = True
mpl.rcParams['ytick.right'] = True


# Run the simulations
Mdata = []

MdataD = []
NdataD = []
OdataD = []
PdataD = []


for i in range(0,9):
          Mdata.append(M[0,i])
          MdataD.append(MD[0,i])
          NdataD.append(ND[0,i])
          OdataD.append(OD[0,i])
          PdataD.append(PD[0,i])
          
          
fig= plt.figure(figsize=(13,10))

cmap = sns.color_palette("plasma", n_colors=4)

fp = [1,1,1,1]
np.interp(Mdata[5:9], Mdata[1:5], fp)
plt.plot(Mdata[5:9], Mdata[1:5], '-', color =  "Orange", linewidth=2.5)

np.interp(MdataD[5:9], MdataD[1:5], fp)
plt.plot(MdataD[5:9], MdataD[1:5], '-', color = "cornflowerblue", linewidth=2.5)

np.interp(NdataD[5:9], NdataD[1:5], fp)
plt.plot(NdataD[5:9], NdataD[1:5], '-', color = "darkviolet", linewidth=2.5)

np.interp(OdataD[5:9], OdataD[1:5], fp)
plt.plot(OdataD[5:9], OdataD[1:5], '-', color = "darkred", linewidth=2.5)

np.interp(PdataD[5:9], PdataD[1:5], fp)
plt.plot(PdataD[5:9], PdataD[1:5], '-', color = "red", linewidth=2.5)




for i in range(1,5):
           plt.plot(Mdata[i+4], Mdata[i], color = "cornflowerblue", linewidth=2, label='t')               
           plt.plot(MdataD[i+4], MdataD[i], color = cmap[0], linewidth=2, label='t')
           plt.plot(NdataD[i+4], NdataD[i], color = cmap[1], linewidth=2, label='t')
           plt.plot(OdataD[i+4], OdataD[i], color = cmap[2], linewidth=2, label='t')
           plt.plot(PdataD[i+4], PdataD[i], color = cmap[3], linewidth=2, label='t')                            
                      
plt.yscale('log')

from matplotlib.patches import Patch
from matplotlib.lines import Line2D

legend_elements = [Line2D([0], [0], ls='-', color='cornflowerblue', lw=3,label='Davenport mode, 1 Myr'),
                   Line2D([0], [0], ls='-', color='darkviolet',lw=3, label='Davenport mode, 10 Myr'),
                   Line2D([0], [0], ls='-', color='darkred', lw=3,label='Davenport mode, 100 Myr'),
                   Line2D([0], [0], ls='-', color='red', lw=3,label='Davenport mode, 1 Gyr'),
                   Line2D([0], [0],color = cmap[3], lw=3, label='Lacy mode')
]

plt.legend(handles=legend_elements, ncol=1,loc='lower left', fontsize = 24)



plt.title('Flare Frequency Distribution', fontsize=37)  #Define el título de la gráfica 
plt.xlabel("log Flare Energy (erg)",fontsize=40)         #Define el título y el tamaño del título en X
plt.ylabel("Cumulative Flare Freq (#/day)",fontsize=40)        #Define el título y el tamaño del título en Y
plt.xticks(fontsize=40)                         #Define el tamaño de los números en el eje X
plt.yticks(fontsize=40)                         #Define el tamaño de los números en el eje Y


#plt.ylim(10**(-4.02),10**(-0.5))
#plt.xlim(32.85,36.15)

if (sys.argv[1] == 'pdf'):
    fig.savefig('FfdReproduced.pdf', bbox_inches="tight", dpi=600)
if (sys.argv[1] == 'png'):
    fig.savefig('FfdReproduced.png', bbox_inches="tight", dpi=600)

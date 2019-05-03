import numpy as np
import matplotlib as mpl
import matplotlib.pyplot as plt
from scipy.misc import imread
import seaborn as sns
from time import time
clock = int(time())

# sns.set_style("whitegrid")
plt.close('all')

cmap=plt.get_cmap('nipy_spectral')
mpl.rcParams['lines.linewidth'] = 2
mpl.rcParams['lines.color'] = cmap(0)
mpl.rcParams['axes.labelsize'] = 13
mpl.rcParams['axes.titlesize'] = 14
mpl.rcParams['xtick.labelsize'] = 12
mpl.rcParams['ytick.labelsize'] = 12
mpl.rcParams['legend.fontsize'] = 13
mpl.rcParams['axes.titleweight'] = 'bold'

N_Planet = 5
M_ini    = 10

if N_Planet == 4:
    # TRAPPIST-1 e #
    Name_Planet = 'Trappist-1 e'
    Name_Folder = 'Trappist-1_e'

    TSol      = [[17.011,15.9374,13.521],[  5.7996,5.7743,5.6935]]
    WaterLeft = [[  2.41,   2.60,3.0755],[    5.24, 5.254, 5.287]]
    OxyPress  = [[    14,  11.20,  5.75],[0.000593,     0,     0]]
elif N_Planet == 5:
    # TRAPPIST-1 f #
    Name_Planet = 'Trappist-1 f'
    Name_Folder = 'Trappist-1_f'
    TSol      = [[15.2732,15.0551,14.4199],[  5.2412,5.2351,5.2207]]
    WaterLeft = [[   4.67,  4.704,  4.798],[   6.725,6.7267, 6.731]]
    OxyPress  = [[     11,   10.5,    9.0],[0.000775,     0,     0]]
elif N_Planet == 6:
    # TRAPPIST-1 g #
    Name_Planet = 'Trappist-1 g'
    Name_Folder = 'Trappist-1_g'
    TSol      = [[15.4677,15.4109,15.2079],[5.1383,  5.1375,  5.1332]]
    WaterLeft = [[   6.04,  6.043,  6.066],[ 7.647,  7.6474,   7.648]]
    OxyPress  = [[   12.6,   12.4,   11.9],[     0,0.000761,0.001016]]

Ecc = [1e-3, 5e-3, 1e-2]
K40 = [1,10]

waterfile = open('water.txt')
water = waterfile.readlines()
waterfile.close()

eccfile = open('ecc.txt')
ecc = eccfile.readlines()
eccfile.close()

radfile = open('rad.txt')
rad = radfile.readlines()
radfile.close()

planets = ['e','f','g']
masses  = [0.766,0.926,1.14]

for p in range(len(planets)):

    fig = plt.figure(num=None, figsize=(10, 9), dpi=300, facecolor='w', edgecolor='k')
    fig.suptitle(''+str(Name_Planet)+' at magma ocean solidification', fontsize=16, fontweight='bold')

    ax1 = fig.add_subplot(221)
    for w in range(len(water)):
        solid_time        = np.genfromtxt('Results_TR1_'+planets[p]+'_'+water[w][:-1]+'_solid_time.dat')
        # solid_waterlocked = np.genfromtxt('Results_TR1_'+planets[p]+'_'+water[w][:-1]+'_solid_waterlocked.dat')
        solid_watertot    = np.genfromtxt('Results_TR1_'+planets[p]+'_'+water[w][:-1]+'_solid_watertot.dat')
        solid_presswater  = np.genfromtxt('Results_TR1_'+planets[p]+'_'+water[w][:-1]+'_solid_presswater.dat')
        solid_pressoxy    = np.genfromtxt('Results_TR1_'+planets[p]+'_'+water[w][:-1]+'_solid_pressoxy.dat')

        CS_1 = ax1.contour(ecc, rad, solid_time, [8, 12, 16], colors=cmap(np.log10(water[w])*110), linestyles=(':','--','-'))

    labels = ['8','12','16']
    for i in range(len(labels)):
        CS_1.collections[i].set_label(labels[i])
    ax1.legend(loc='best', frameon=True)
    ax1.set_title('(a) Solidification Time (Myr)')
    ax1.set_xscale('log')
    ax1.set_yscale('log')
    # ax1.set_xlabel('Eccentricity')
    ax1.set_ylabel('$^{40}K$ Abundance / Earth')

ax2 = fig.add_subplot(222)
if (N_Planet==4):
    CS_1 = ax2.contour(Ecc, K40, TSol, [8, 12, 16], colors='k', linestyles=(':','--','-'))
    labels = ['8','12','16']
elif (N_Planet==5):
    CS_1 = ax2.contour(Ecc, K40, TSol, [6, 10, 14], colors='k', linestyles=(':','--','-'))
    labels = ['6', '10','14']
elif (N_Planet==6):
    CS_1 = ax2.contour(Ecc, K40, TSol, [6, 10, 14], colors='k', linestyles=(':','--','-'))
    labels = ['6', '10','14']
for i in range(len(labels)):
    CS_1.collections[i].set_label(labels[i])
ax2.legend(loc='best', frameon=True)
ax2.set_title('(b) Desiccation Time (Myr)')
ax2.set_xscale('log')
ax2.set_yscale('log')
# ax2.set_xlabel('Eccentricity')
# ax2.set_ylabel('$^{40}K$ Abundance / Earth')

ax3 = fig.add_subplot(223)
if (N_Planet==4):
    CS_3 = ax3.contour(Ecc, K40, WaterLeft, [3, 4, 5], colors='k', linestyles=(':','--','-'))
    labels = ['3', '4','5']
elif (N_Planet==5):
    CS_3 = ax3.contour(Ecc, K40, WaterLeft, [5, 5.75, 6.5], colors='k', linestyles=(':','--','-'))
    labels = ['5', '5.75','6.5']
elif (N_Planet==6):
    CS_3 = ax3.contour(Ecc, K40, WaterLeft, [6.5, 7, 7.5], colors='k', linestyles=(':','--','-'))
    labels = ['6.5', '7','7.5']
for i in range(len(labels)):
    CS_3.collections[i].set_label(labels[i])
ax3.legend(loc='best', frameon=True)
ax3.set_title('(c) Water left at solidification (TO)')
ax3.set_xscale('log')
ax3.set_yscale('log')
ax3.set_xlabel('Eccentricity')
ax3.set_ylabel('$^{40}K$ Abundance')

ax4 = fig.add_subplot(224)
if (N_Planet==4):
    CS_4 = ax4.contour(Ecc, K40, OxyPress, [0.1, 1, 10], colors='k', linestyles=(':','--','-'))
    labels = ['0.1','1','10']
elif (N_Planet==5):
    CS_4 = ax4.contour(Ecc, K40, OxyPress, [0.1, 1, 10], colors='k', linestyles=(':','--','-'))
    labels = ['0.1','1','10']
elif (N_Planet==6):
    CS_4 = ax4.contour(Ecc, K40, OxyPress, [0.1, 1, 10], colors='k', linestyles=(':','--','-'))
    labels = ['0.1', '1','10']
for i in range(len(labels)):
    CS_4.collections[i].set_label(labels[i])
# ax2.clabel(CS_3, inline=1, fontsize=12, fmt='%1.0f', inline_spacing=1)
ax4.legend(loc='best', frameon=True)
ax4.set_title('(d) Oxygen pressure (bar)')
ax4.set_xscale('log')
ax4.set_yscale('log')
ax4.set_xlabel('Eccentricity')
# ax4.set_ylabel('$^{40}K$ Abundance / Earth')

plt.subplots_adjust(left=0.1, right=0.97, top=0.9, bottom=0.07)
plt.savefig('../Results_VPlanet/Trappist-1/'+str(Name_Folder)+'/'+str(clock)+'_'+str(Name_Folder)+'_'+str(M_ini)+'TO_CountourPlot.png')
# plt.show()

# ax1.clabel(CS, inline=1, fontsize=12, fmt='%1.0f', inline_spacing=1)

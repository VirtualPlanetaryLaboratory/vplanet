import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
import matplotlib as mpl

sns.set_style("whitegrid")
plt.close('all')
# Set style for plot #
mpl.rcParams['lines.linewidth'] = 2
mpl.rcParams['axes.labelsize'] = 13
mpl.rcParams['xtick.labelsize'] = 12
mpl.rcParams['ytick.labelsize'] = 12
mpl.rcParams['legend.fontsize'] = 13
######################
cmap=plt.get_cmap('nipy_spectral')

# Points along curve
# x = [-1,2,3,4]
# y = [1420, 1420, 1420, 1420]
#
# xx = [1.5]
# yy = [1640]

data = np.loadtxt("greynewviscGJ1132.GJ1132b.forward")
data_petit = np.loadtxt("petitnewviscGJ1132.GJ1132b.forward")

time        = data[:,0]  # time (yr)
Tpot        = data[:,1]  # Potential temp magma ocean (K)
Tsurf       = data[:,2]

time_petit        = data_petit[:,0]  # time (yr)
Tpot_petit        = data_petit[:,1]  # Potential temp magma ocean (K)
Tsurf_petit       = data_petit[:,2]  # Surface temp (K)

LogT = np.zeros(len(time))
for i in range(len(time)):
    LogT[i] = np.log10(time[i]) - 6

LogT_petit = np.zeros(len(time_petit))
for i in range(len(time_petit)):
    LogT_petit[i] = np.log10(time_petit[i]) - 6

fig = plt.figure(num=None, figsize=(14, 4), dpi=300, facecolor='w', edgecolor='k')

img = plt.imread('Results_Schaefer_a.png')
plt.imshow(img, zorder=0, extent=[-6, 3.7, 280, 4000], aspect='auto')

plt.plot(LogT, Tpot, label='grey', color=cmap(0), linewidth=3.0)
plt.plot(LogT, Tsurf, linestyle='--', color=cmap(0), linewidth=3.0)

plt.plot(LogT_petit, Tpot_petit, label='petit',color=cmap(220), linewidth=3.0)
plt.plot(LogT_petit, Tsurf_petit, linestyle='--', color=cmap(220), linewidth=3.0)


# plt.plot(x, y, label='Solidus (1420 K)')
# plt.plot(xx, yy, 'ko', label='1640K', color='red')
# plt.plot(T,F)
# plt.plot(T,Fa)
plt.legend(loc='best', frameon=True, fontsize=14)
plt.xlim([-6,3.7])
plt.ylim([280,4000])
plt.ylabel('Temperature (K)', fontsize=14, fontweight='bold')
plt.xlabel('$log_{10}(t)$ (Myr)', fontsize=14, fontweight='bold')
# plt.show()
plt.tight_layout()
# plt.subplots_adjust(left=0.15, right=0.95, top=0.95, bottom=0.12)
plt.savefig('Schaefer_Grey_Petit_TeqCons.png')

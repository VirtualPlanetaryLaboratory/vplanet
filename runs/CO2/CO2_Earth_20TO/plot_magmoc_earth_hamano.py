import numpy as np
import matplotlib as mpl
import matplotlib.pyplot  as plt
import matplotlib.gridspec as gridspec
import seaborn as sns

# sns.set_style("whitegrid")
plt.close('all')
# Set style for plot #
mpl.rcParams['lines.linewidth'] = 3
mpl.rcParams['axes.labelsize'] = 13
mpl.rcParams['xtick.labelsize'] = 12
mpl.rcParams['ytick.labelsize'] = 12
mpl.rcParams['legend.fontsize'] = 15
mpl.rcParams['axes.labelweight'] = 'bold'
######################

cmap=plt.get_cmap('nipy_spectral')
A = 0

all_plots = 1
# 1 - all, 2 - r_sol, 3 - water mass, 4 - Fe2O3 mass fraction

# 1: plot individual graphs, 0: plot all graphs together
individual = 0
# 1: plot time axis log scale, 0: plot time axis linear
log_plot = 1
Initial_water = 1
# read data
# data = np.loadtxt("Solarsystem.Earth.forward")
data = np.loadtxt("Solarsystem.Earth.forward")

time        = data[:,0]  # time (yr)
Tpot        = data[:,1]  # Potential temp magma ocean (K)
Tsurf       = data[:,2]  # Surface temp (K)
r_sol       = data[:,3]  # solidification radius (R_earth)
M_water_mo  = data[:,4] # water mass in magma ocean + atmosphere (TO)
M_water_sol = data[:,5] # water mass in solid mantle (kg)
M_O_mo      = data[:,6] # mass of oxygen in magma ocean + atmosphere (kg)
M_O_sol     = data[:,7] # mass of oxygen in solid mantle (kg)
Press_H2O   = data[:,8] # partial pressure water in atmopshere (bar)
Press_O     = data[:,9] # partial pressure oxygen in atmosphere (bar)
M_H_Space   = data[:,10] # partial pressure oxygen in atmosphere (bar)
M_O_Space   = data[:,11] # partial pressure oxygen in atmosphere (bar)
Frac_Fe2O3  = data[:,12] # partial pressure oxygen in atmosphere (bar)
NetFluxAtmo = data[:,13] # atmospheric net flux (W/m^2)
Melt_frac   = data[:,14] # atmospheric net flux (W/m^2)

n_time = len(time)
i_end  = n_time-1

M_water_atm = np.zeros(n_time)
M_O_atm     = np.zeros(n_time)

N_H_sol = np.zeros(n_time) # number of H atoms in solid mantle
N_H_space = np.zeros(n_time) # number of H atoms in solid mantle
N_H_mo  = np.zeros(n_time) # number of H atoms in liquid mantle
N_H_atm = np.zeros(n_time) # number of H atoms in atmosphere
N_O_sol = np.zeros(n_time) # number of O atoms in solid mantle
N_O_mo  = np.zeros(n_time) # number of O atoms in liquid mantle
N_O_atm = np.zeros(n_time) # number of O atoms in atmosphere
N_O_space = np.zeros(n_time) # number of H atoms in solid mantle

N_H_tot = np.zeros(n_time) # number of O atoms in atmosphere
N_O_tot = np.zeros(n_time) # number of O atoms in atmosphere

round = 1e45

TO        = 1.39e21         # mass of 1 Terr. Ocean [kg]
AVOGADROCONST = 6.022e23

REARTH = 6.3781e6        # m
MEARTH = 5.972186e24     # kg
BIGG   = 6.67428e-11     # m**3/kg/s**2
r_p    = REARTH
r_core = 3.4e6/r_p
m_p    = MEARTH
g      = (BIGG * m_p) / (r_p ** 2)

for i in range(n_time):
    M_water_atm[i] = Press_H2O[i]*1e5 * 4 * np.pi * r_p**2 / g
    M_O_atm[i]     = Press_O[i]*1e5 * 4 * np.pi * r_p**2 / g

    N_H_space[i] = M_H_Space[i] * AVOGADROCONST / (0.001 * round)
    N_H_sol[i] = 2 * M_water_sol[i]*TO * AVOGADROCONST / (0.018 * round)
    N_H_mo[i]  = 2 * (M_water_mo[i]*TO - M_water_atm[i]) * AVOGADROCONST / (0.018 * round)
    N_H_atm[i] = 2 * M_water_atm[i] * AVOGADROCONST / (0.018 * round)
    N_H_tot[i] = N_H_sol[i] + N_H_mo[i] + N_H_atm[i] + N_H_space[i]

    N_O_space[i] = M_O_Space[i] * AVOGADROCONST / (0.016 * round)
    N_O_sol[i] = M_water_sol[i]*TO * AVOGADROCONST / (0.018 * round) \
                 + M_O_sol[i] * AVOGADROCONST / (0.016 * round)
    N_O_mo[i]  = (M_water_mo[i]*TO - M_water_atm[i]) * AVOGADROCONST / (0.018 * round) \
                 + (M_O_mo[i] - M_O_atm[i]) * AVOGADROCONST / (0.016 * round)
    N_O_atm[i] = M_water_atm[i] * AVOGADROCONST / (0.018 * round) \
                 + M_O_atm[i] * AVOGADROCONST / (0.016 * round)
    N_O_tot[i] = N_O_sol[i] + N_O_mo[i] + N_O_atm[i] + N_O_space[i]

print('Solidification Time           = ',time[n_time-1]*1e-6,  ' Myr')
print('Water mass locked in mantle   = ',M_water_sol[n_time-1], ' TO')
print('Oxygen mass locked in mantle  = ',M_O_sol[n_time-1],     ' kg')
print('Water pressure in atmosphere  = ',Press_H2O[n_time-1],  ' bar')
print('Oxygen pressure in atmosphere = ',Press_O[n_time-1],    ' bar')
print('Fe2O3 mass frac in mantle     = ',Frac_Fe2O3[n_time-1])



V_sol = np.zeros(len(r_sol))
for i in range(len(V_sol)):
    V_sol[i] = (r_sol[i]**3 - r_core**3)/((r_p/REARTH)**3 - r_core**3)
### Plot ###

fig = plt.figure(num=None, figsize=(10, 10), dpi=300, facecolor='w', edgecolor='k')
fig.suptitle('Earth: Initial water content '+str(M_water_mo[0])+' terrestrial oceans', fontsize=16, fontweight='bold')

gs = fig.add_gridspec(6, 1)
ax11 = fig.add_subplot(gs[1, 0])
ax3 = fig.add_subplot(gs[2:-2, 0], sharex=ax11)
ax4 = fig.add_subplot(gs[0, 0], sharex=ax11)
ax5 = fig.add_subplot(gs[4:, 0], sharex=ax11)

### multiple
# ax1.legend(loc='best', frameon=True)
ax1 = ax11.twinx()

ax1.set_ylabel('Temperature (K)')
ax1.set_xscale('log')
ax1.set_xlim([0.001,13])
ax1.set_ylim([1300,3200])
ax11.fill_between(time*10**-6, V_sol, color=cmap(0), alpha=0.8)
ax11.fill_between(time*10**-6, 1, V_sol, color=cmap(0), alpha=0.2)

# ax11.plot(time*10**-6, r_sol, color=cmap(0))
ax11.set_ylim([0,1])
# ax11.legend(loc='best', frameon=True)
ax11.set_ylabel('Solid radius ($r_E$)')
# ax1.plot(time*10**-6, Tpot, label='$T_p$', color='white', linewidth=5)
ax1.plot(time*10**-6, Tsurf, label='$T_{surf}$', color='white', linewidth=5)


ax3.plot(time*10**-6, M_water_mo+M_water_sol, label='Total', color=cmap(0))
ax3.plot(time*10**-6, M_water_atm/TO, label='Atmosphere', color=cmap(220))
ax3.plot(time*10**-6, M_water_mo-M_water_atm/TO, label='Magma ocean', color=cmap(120))
ax3.plot(time*10**-6, M_water_sol, label='Solid mantle', color=cmap(60))
ax3.set_ylim([0.001,10])
# ax3.legend(loc='best', frameon=True)
ax3.set_ylabel('Water Mass (TO)')
ax3.set_yscale('log')

# ax4.plot(time*10**-6, Press_H2O) #, label='partial pressure H2O')
# ax4.plot(time*10**-6, Press_O, label='partial pressure O')
# ax4.legend(loc='best', frameon=True)
ax4.fill_between(time*10**-6, Press_H2O, color=cmap(0), alpha=0.5)

ax4.set_ylabel('Atmospheric pressure (bar)')
ax4.set_ylim([0,1500])

ax5.plot(time*10**-6, NetFluxAtmo, color=cmap(0))
# ax5.legend(loc='best', frameon=True)
ax5.set_ylabel('Net flux $W m^{-2}$')
ax5.set_yscale('log')
ax5.set_ylim([100,1e5])
ax5.set_xlabel('Time (Myr)')

ax1.tick_params(axis='both', which='both', direction='in')
ax11.tick_params(axis='both', which='both', direction='in')
ax5.tick_params(axis='both', which='both', direction='in', left='on', right='on')
ax3.tick_params(axis='both', which='both', direction='in', left='on', right='on')
ax4.tick_params(axis='both', which='both', direction='in', left='on', right='on')
ax1.set_xticklabels([])
ax11.set_xticklabels([])
ax3.set_xticklabels([])
ax4.set_xticklabels([])
# ax5.set_xticklabels([])

plt.subplots_adjust(left=0.2, right=0.7, top=0.9, bottom=0.16, hspace=0)
plt.savefig('07Earth_5TO_Hamano.png')


# ax6 = fig.add_subplot(336, sharex=ax1)
# ax6.plot(time*10**-6, M_O_atm, label='atmosphere')
# ax6.plot(time*10**-6, M_O_mo-M_O_atm, label='magma ocean')
# ax6.plot(time*10**-6, M_O_sol, label='solid')
# ax6.legend(loc='best', frameon=True)
# ax6.set_ylabel('Oxygen Mass (kg)')
# ax6.set_yscale('log')
# # ax6.set_ylim([1e16,1e21])
#
# ax7 = fig.add_subplot(337, sharex=ax1)
# ax7.plot(time*10**-6, N_H_space, label='space')
# ax7.plot(time*10**-6, N_H_atm, label='atmosphere')
# ax7.plot(time*10**-6, N_H_mo, label='magma ocean')
# ax7.plot(time*10**-6, N_H_sol, label='solid mantle')
# ax7.plot(time*10**-6, N_H_tot, label='total', linestyle=':')
# ax7.legend(loc='best', frameon=True)
# ax7.set_xlabel('Time (Myrs)')
# ax7.set_ylabel('Number of H atoms ($10^{45}$)')
# ax7.set_yscale('log')
# ax7.set_ylim([1e-1,1e3])
#
# ax8 = fig.add_subplot(338, sharex=ax1)
# ax8.plot(time*10**-6, N_O_space, label='space')
# ax8.plot(time*10**-6, N_O_atm, label='atmosphere')
# ax8.plot(time*10**-6, N_O_mo, label='magma ocean')
# ax8.plot(time*10**-6, N_O_sol, label='solid mantle')
# ax8.plot(time*10**-6, N_O_tot, label='total', linestyle=':')
# ax8.legend(loc='best', frameon=True)
# ax8.set_xlabel('Time (Myrs)')
# ax8.set_ylabel('Number of O atoms ($10^{45}$)')
# ax8.set_yscale('log')
# ax8.set_ylim([1e-1,1e3])
#
# ax9 = fig.add_subplot(339, sharex=ax1)
# ax9.plot(time*10**-6, N_H_tot/N_H_tot[0]-1, label='H')
# ax9.plot(time*10**-6, N_O_tot/N_O_tot[0]-1, label='O', linestyle='--')
# ax9.plot(time*10**-6, (N_O_tot/N_O_tot[0]-1)-(N_H_tot/N_H_tot[0]-1), label='O-H', linestyle=':')
# ax9.legend(loc='best', frameon=True)
# ax9.set_xlabel('Time (Myrs)')
# ax9.set_ylabel('Ratio of atoms lost')
# ax8.set_yscale('log')
#
# ax9 = fig.add_subplot(339, sharex=ax1)
# ax9.plot(time*10**-6, N_H_tot-N_H_tot[0], label='H')
# ax9.plot(time*10**-6, N_O_tot-N_O_tot[0], label='O')
# ax9.plot(time*10**-6, (N_O_tot-N_O_tot[0])-(N_H_tot-N_H_tot[0])/2., label='O-H/2', linestyle='--')
# ax9.legend(loc='best', frameon=True)
# ax9.set_xlabel('Time (Myrs)')
# ax9.set_ylabel('Number of atoms lost ($10^{45}$)')

import numpy as np
import matplotlib.pyplot  as plt
import seaborn as sns

sns.set_style("whitegrid")
plt.close('all')

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
M_CO2_mo    = data[:,15] # CO2 Mass mo + atm [kg]
M_CO2_sol   = data[:,16] # CO2 Mass sol [kg]
Press_CO2   = data[:,17] # pressure of CO2 in atmosphere [bar]
Frac_CO2    = data[:,18] # mass frac of CO2 in magma ocean
Frac_H2O    = data[:,19] # Water fraction in magma ocean

n_time = len(time)
i_end  = n_time-1

M_water_atm = np.zeros(n_time)
M_O_atm     = np.zeros(n_time)
M_CO2_atm   = np.zeros(n_time)

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
m_p    = MEARTH
g      = (BIGG * m_p) / (r_p ** 2)

for i in range(n_time):
    M_water_atm[i] = Press_H2O[i]*1e5 * 4 * np.pi * r_p**2 / g
    M_O_atm[i]     = Press_O[i]*1e5 * 4 * np.pi * r_p**2 / g
    M_CO2_atm[i]   = Press_CO2[i]*1e5 * 4 * np.pi * r_p**2 / g

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



### Plot ###

fig = plt.figure()
fig.suptitle('Earth: Initial water content '+str(M_water_mo[0])+' terrestrial oceans', fontsize=16, fontweight='bold')

### multiple
if all_plots == 1:
    ax1 = fig.add_subplot(331)
    ax1.plot(time*10**-6, Tpot, label='$T_p$')
    ax1.plot(time*10**-6, Tsurf, label='$T_{surf}$', linestyle='--')
    ax1.legend(loc='best', frameon=True)
    ax1.set_ylabel('Temperature (K)')
    ax1.set_xscale('log')
    # ax1.set_xlim([1e-6,time[i_end]*1e-6])

    ax2 = fig.add_subplot(332, sharex=ax1)
    ax2.plot(time*10**-6, r_sol, label='$r_s$', color=cmap(0))
    ax2.set_ylim([0.5,1])
    ax2.legend(loc='best', frameon=True)
    ax2.set_ylabel('Solidification radius ($r_p$)')

    ax3 = fig.add_subplot(333, sharex=ax1)
    ax3.plot(time*10**-6, M_water_atm/TO, label='atmosphere')
    ax3.plot(time*10**-6, M_water_mo-M_water_atm/TO, label='magma ocean')
    ax3.plot(time*10**-6, M_water_sol, label='solid mantle')
    ax3.set_ylim([0.01,10.50])
    ax3.legend(loc='best', frameon=True)
    ax3.set_ylabel('Water Mass (TO)')
    ax3.set_yscale('log')

    ax4 = fig.add_subplot(334, sharex=ax1)
    ax4.plot(time*10**-6, Press_H2O, label='$H_2O$', color=cmap(0))
    ax4.plot(time*10**-6, Press_O, label='$O$', color=cmap(220))
    ax4.plot(time*10**-6, Press_CO2, label='$CO_2$', color=cmap(100))
    # ax4.axvline(x=T_Solid,linestyle='--', color=cmap(20))
    # ax4.axvline(x=T_Desicc,linestyle='--', color=cmap(140))
    ax4.legend(loc='best', frameon=True)
    ax4.set_ylabel('Atmospheric pressure (bar)')
    ax4.set_yscale('log')


    # --- Mass fractions magmoc --- #
    ax5 = fig.add_subplot(335, sharex=ax1)
    ax5.plot(time*10**-6, Frac_H2O, label='$H_2O$', color=cmap(0))
    ax5.plot(time*10**-6, Frac_Fe2O3, label='$Fe_2O_3$', color=cmap(220))
    ax5.plot(time*10**-6, Frac_CO2, label='$CO_2$', color=cmap(100))
    # ax5.axvline(x=T_Solid,linestyle='--', color=cmap(20))
    # ax5.axvline(x=T_Desicc,linestyle='--', color=cmap(140))
    ax5.legend(loc='best', frameon=True)
    ax5.set_ylabel('Mass frac in magma ocean')
    # ax5.set_yscale('log')
    ax5.set_ylim([0,0.1])

    ax6 = fig.add_subplot(336, sharex=ax1)
    ax6.plot(time*10**-6, M_O_atm, label='atmosphere')
    ax6.plot(time*10**-6, M_O_mo-M_O_atm, label='magma ocean')
    ax6.plot(time*10**-6, M_O_sol, label='solid')
    ax6.legend(loc='best', frameon=True)
    ax6.set_ylabel('Oxygen Mass (kg)')
    ax6.set_yscale('log')
    # ax6.set_ylim([1e16,1e21])

    # --- Atmosphere Net FLux --- #
    ax7 = fig.add_subplot(337, sharex=ax1)
    ax7.plot(time*10**-6, NetFluxAtmo, color=cmap(0))
    ax7.set_ylabel('Atmospheric net flux ($W/m^2$)')
    # ax7.axvline(x=T_Solid,linestyle='--', color=cmap(20))
    # ax7.axvline(x=T_Desicc,linestyle='--', color=cmap(140))
    ax7.set_yscale('log')
    ax7.set_xlabel('Time (Myrs)')

    # --- Mantle Heating --- #
    ax8 = fig.add_subplot(338, sharex=ax1)
    # ax8.plot(time*10**-6, RadioHeat, color=cmap(0), label='Radiogenic')
    # ax8.plot(time*10**-6, TidalHeat, color=cmap(220), label='Tidal')
    # ax8.axvline(x=T_Solid,linestyle='--', color=cmap(20))
    # ax8.axvline(x=T_Desicc,linestyle='--', color=cmap(140))
    ax8.legend(loc='best', frameon=True)
    ax8.set_ylabel('Mantle Heating Power (TW)')
    ax8.set_yscale('log')
    ax8.set_xlabel('Time (Myrs)')

    # --- CO2 mass --- #
    ax9 = fig.add_subplot(339, sharex=ax1)
    ax9.plot(time*10**-6, M_CO2_mo-M_CO2_atm, label='magma ocean', color=cmap(0))
    ax9.plot(time*10**-6, M_CO2_atm, label='atmosphere', color=cmap(220))
    ax9.plot(time*10**-6, M_CO2_sol, label='solid', color=cmap(70))
    # ax9.plot(time*10**-6, M_CO2_sol+M_CO2_mo, label='total', color=cmap(100))
    # ax9.axvline(x=T_Solid,linestyle='--', color=cmap(20))
    # ax9.axvline(x=T_Desicc,linestyle='--', color=cmap(140))
    ax9.legend(loc='best', frameon=True)
    ax9.set_ylabel('$CO_2$ Mass (kg)')
    ax9.set_yscale('log')
    # xup = max(M_CO2_atm[i_end],M_CO2_sol[i_end],M_CO2_mo[i_end]-M_CO2_atm[i_end])
    ax9.set_ylim([1e19,1e23])

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

else:
    if (all_plots == 2):
        ax = fig.subplots()

        ax.plot(time*10**-6, r_sol, color=cmap(0), linewidth=3.0)

        ax.set_ylim([0.5,1])
        ax.set_ylabel('Solidification radius ($r_{Earth}$)', fontsize=14, fontweight='bold')


    elif all_plots == 3:
        ax = fig.subplots()

        ax.plot(time*10**-6, M_water_atm/TO, label='atmosphere', linewidth=3.0)
        ax.plot(time*10**-6, M_water_mo-M_water_atm/TO, label='magma ocean', linewidth=3.0)
        ax.plot(time*10**-6, M_water_sol, label='solid mantle', linewidth=3.0)

        ax.legend(loc='best', frameon=True, fontsize=13)
        ax.set_ylim([0.01,10.50])
        ax.set_ylabel('Water Mass (TO)', fontsize=14, fontweight='bold')
        ax.set_yscale('log')

    elif all_plots == 4:
        ax = fig.subplots()

        ax.plot(time*10**-6, Frac_Fe2O3, linewidth=3.0, color=cmap(0))

        ax.set_ylabel('$Fe_2O_3$ mass frac in magma ocean', fontsize=14, fontweight='bold')

    ax.set_xscale('log')
    ax.set_xlabel('Time (Myrs)', fontsize=14, fontweight='bold')
    ax.tick_params(labelsize=13)

plt.subplots_adjust(left=0.05, right=0.99, top=0.93, bottom=0.07)
# plt.savefig('plot.png')
plt.show()

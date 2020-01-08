#################################################
#   Plot results for TRAPPIST-1 from VPLanet    #
# Modules used: MagmOc, AtmEsc, RadHeat, EqTide #
#################################################

import numpy as np
import matplotlib as mpl
import matplotlib.pyplot  as plt
# import seaborn as sns
from time import time

# sns.set_style("whitegrid")
plt.close('all')

clock = int(time())

# Set style for plot #
mpl.rcParams['lines.linewidth'] = 2
mpl.rcParams['axes.labelsize'] = 13
mpl.rcParams['xtick.labelsize'] = 12
mpl.rcParams['ytick.labelsize'] = 12
mpl.rcParams['legend.fontsize'] = 13
######################
cmap=plt.get_cmap('nipy_spectral')

# Which planet to plot:
# 1-b, 2-c, 3-d, 4-e, 5-f, 6-g, 7-h
N_Planet = 4

# 40-K abundance (in Earth abundances)
K40 = 1

# read data
if N_Planet == 4:
    # TRAPPIST-1 e #
    data = np.loadtxt("Trappist1.e.forward")
    R_N_Planet = 0.913
    M_N_Planet = 0.766
    Ecc = 0.005
    Name_Planet = 'Trappist-1 e'
    Name_Folder = 'Trappist-1_e'
elif N_Planet == 5:
    # TRAPPIST-1 f #
    data = np.loadtxt("Trappist1.f.forward")
    R_N_Planet = 1.05
    M_N_Planet = 0.926
    Ecc = 0.01
    Name_Planet = 'Trappist-1 f'
    Name_Folder = 'Trappist-1_f'
elif N_Planet == 6:
    # TRAPPIST-1 g #
    data = np.loadtxt("Trappist1.g.forward")
    R_N_Planet = 1.15
    M_N_Planet = 1.14
    Ecc = 0.002
    Name_Planet = 'Trappist-1 g'
    Name_Folder = 'Trappist-1_g'

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
Frac_H2O    = data[:,14] # Water fraction in magma ocean
RadioHeat   = data[:,15] # Radiogenic Heating Power (TW)
TidalHeat   = data[:,16] # Tidal Heating Power (TW)
SemiMajor   = data[:,17] # Semi Major Axis (AU)
HZInnerEdge = data[:,18] # Inner Edge of the HZ (AU)
M_CO2_mo    = data[:,19] # CO2 Mass mo + atm [kg]
M_CO2_sol   = data[:,20] # CO2 Mass sol [kg]
Press_CO2   = data[:,21] # pressure of CO2 in atmosphere [bar]
Frac_CO2    = data[:,22] # mass frac of CO2 in magma ocean


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
r_p    = R_N_Planet * REARTH
m_p    = M_N_Planet * MEARTH
g      = (BIGG * m_p) / (r_p ** 2)
rho_m  = 4000

man_sol   = 0 # Mantle solidified?
esc_stop  = 0 # Escape stopped? (Inner edge HZ)
atm_des   = 0 # Atmosphere desiccated?
quasi_sol = 0 # Atm desiccated & T_surf below 1000K but not solid?

for i in range(n_time):

    if (atm_des == 0) and (Press_H2O[i] <= 1e-2):
        atm_des  = 1
        n_t_desicc = i

    if (man_sol == 0) and ((r_sol[i] >= 0.9999*R_N_Planet) or (Tpot[i]<=1660)):
        man_sol = 1
        n_t_solid = i

    if (esc_stop == 0) and (SemiMajor[i] >= HZInnerEdge[i]):
        esc_stop = 1
        n_t_habit  = i

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

# print('-----------------------'+str(Name_Planet)+'-----------------------')
# print('Solidification Time               = ',time[n_t_solid]*1e-6,  ' Myr')
# print('Water mass locked in mantle       = ',M_water_sol[n_t_solid], ' TO')
# print('Oxygen mass locked in mantle      = ',M_O_sol[n_t_solid],     ' kg')
# print('Total Water mass left in system   = ',M_water_sol[n_t_solid]+M_water_atm[n_t_solid]/TO, ' TO')
# print('Water pressure in atmosphere      = ',Press_H2O[n_t_solid],  ' bar')
# print('Oxygen pressure in atmosphere     = ',Press_O[n_t_solid],    ' bar')
# print('Fe2O3 mass frac in mantle         = ',Frac_Fe2O3[n_t_solid])
#
# print('All outputs in one column:')
# print(time[n_t_solid]*1e-6)
# print(M_water_sol[n_t_solid])
# print(M_O_sol[n_t_solid])
# print(M_water_sol[n_t_solid]+M_water_atm[n_t_solid]/TO)
# print(Press_H2O[n_t_solid])
# print(Press_O[n_t_solid])
# print(Frac_Fe2O3[n_t_solid])
#
# print('-------------------------------------------------------------------')
#
# print('Desiccation Time                  = ',time[n_t_desicc]*1e-6,  ' Myr')
# print('Oxygen pressure in atmosphere     = ',Press_O[n_t_desicc],    ' bar')
#
# print('All outputs in one column:')
# print(time[n_t_desicc]*1e-6)
# print(Press_O[n_t_desicc])
#
# print('-------------------------------------------------------------------')
# print('-------------------------------------------------------------------')

if (atm_des == 1) and (man_sol == 0):
    T_Solid = time[n_t_desicc]/1e6
    T_Desicc = time[n_t_desicc]/1e6
elif (atm_des == 0) and (man_sol == 0) and (esc_stop == 0):
    T_Solid = time[n_time-1]/1e6
    T_Desicc = time[n_time-1]/1e6
else:
    T_Solid = time[n_t_solid]/1e6
    if (atm_des==1):
        T_Desicc = time[n_t_desicc]/1e6
    elif (esc_stop==1):
        T_Desicc = time[n_t_habit]/1e6
    else:
        T_Desicc = time[n_time-1]/1e6
### Plot ###

fig = plt.figure(num=None, figsize=(15, 9), dpi=300, facecolor='w', edgecolor='k')
fig.suptitle(''+str(Name_Planet)+': $M^{ini}_{H_2O} = $ '+str(M_water_mo[0])+' TO, $e = $'+str(Ecc)+', Abundance of $^{40}K =$'+str(K40)+' $\\times$ Earth', fontsize=16, fontweight='bold')

# --- Temperature --- #
ax1 = fig.add_subplot(331)
ax1.plot(time*10**-6, Tpot, label='$T_p$', color=cmap(0))
ax1.plot(time*10**-6, Tsurf, label='$T_{surf}$', linestyle='--', color=cmap(220))
ax1.axvline(x=T_Solid,linestyle='--', color=cmap(20))
ax1.axvline(x=T_Desicc,linestyle='--', color=cmap(140))
ax1.legend(loc='best', frameon=True)
ax1.set_ylabel('Temperature (K)')
ax1.set_xscale('log')
# ax1.set_xlim([1e-1, 3e-1])

# --- Solidification Radius --- #
ax2 = fig.add_subplot(332, sharex=ax1)
ax2.plot(time*10**-6, r_sol/R_N_Planet, label='$r_s$', color=cmap(0))
ax2.axvline(x=T_Solid,linestyle='--', color=cmap(20))
ax2.axvline(x=T_Desicc,linestyle='--', color=cmap(140))
ax2.set_ylim([0.5,1])
ax2.set_ylabel('Solidification radius ($r_p$)')

# --- Water Mass --- #
ax3 = fig.add_subplot(333, sharex=ax1)
ax3.plot(time*10**-6, M_water_mo-M_water_atm/TO, label='magma ocean', color=cmap(0))
ax3.plot(time*10**-6, M_water_atm/TO, label='atmosphere', color=cmap(220))
ax3.plot(time*10**-6, M_water_sol, label='solid', color=cmap(70))
ax3.axvline(x=T_Solid,linestyle='--', color=cmap(20))
ax3.axvline(x=T_Desicc,linestyle='--', color=cmap(140))
ax3.set_ylim([0.001*M_water_mo[0],M_water_mo[0]])
ax3.legend(loc='best', frameon=True)
ax3.set_ylabel('Water Mass (TO)')
ax3.set_yscale('log')

# --- Atmospheric pressures --- #
ax4 = fig.add_subplot(334, sharex=ax1)
ax4.plot(time*10**-6, Press_H2O, label='$H_2O$', color=cmap(0))
ax4.plot(time*10**-6, Press_O, label='$O$', color=cmap(220))
ax4.plot(time*10**-6, Press_CO2, label='$CO_2$', color=cmap(100))
ax4.axvline(x=T_Solid,linestyle='--', color=cmap(20))
ax4.axvline(x=T_Desicc,linestyle='--', color=cmap(140))
ax4.legend(loc='best', frameon=True)
ax4.set_ylabel('Atmospheric pressure (bar)')
ax4.set_yscale('log')

# --- Mass fractions magmoc --- #
ax5 = fig.add_subplot(335, sharex=ax1)
ax5.plot(time*10**-6, Frac_H2O, label='$H_2O$', color=cmap(0))
ax5.plot(time*10**-6, Frac_Fe2O3, label='$Fe_2O_3$', color=cmap(220))
ax5.plot(time*10**-6, Frac_CO2, label='$CO_2$', color=cmap(100))
ax5.axvline(x=T_Solid,linestyle='--', color=cmap(20))
ax5.axvline(x=T_Desicc,linestyle='--', color=cmap(140))
ax5.legend(loc='best', frameon=True)
ax5.set_ylabel('Mass frac in magma ocean')
# ax5.set_yscale('log')
ax5.set_ylim([0,0.1])

# --- Oxygen mass --- #
ax6 = fig.add_subplot(336, sharex=ax1)
ax6.plot(time*10**-6, M_O_mo-M_O_atm, label='magma ocean', color=cmap(0))
ax6.plot(time*10**-6, M_O_atm, label='atmosphere', color=cmap(220))
ax6.plot(time*10**-6, M_O_sol, label='solid', color=cmap(70))
ax6.axvline(x=T_Solid,linestyle='--', color=cmap(20))
ax6.axvline(x=T_Desicc,linestyle='--', color=cmap(140))
ax6.legend(loc='best', frameon=True)
ax6.set_ylabel('Oxygen Mass (kg)')
ax6.set_yscale('log')
xup = max(M_O_atm[i_end],M_O_sol[i_end],M_O_mo[i_end]-M_O_atm[i_end])
ax6.set_ylim([1e-3*xup,xup])

# --- Atmosphere Net FLux --- #
ax7 = fig.add_subplot(337, sharex=ax1)
ax7.plot(time*10**-6, NetFluxAtmo, color=cmap(0))
ax7.set_ylabel('Atmospheric net flux ($W/m^2$)')
ax7.axvline(x=T_Solid,linestyle='--', color=cmap(20))
ax7.axvline(x=T_Desicc,linestyle='--', color=cmap(140))
ax7.set_yscale('log')
ax7.set_xlabel('Time (Myrs)')

# --- Mantle Heating --- #
ax8 = fig.add_subplot(338, sharex=ax1)
ax8.plot(time*10**-6, RadioHeat, color=cmap(0), label='Radiogenic')
ax8.plot(time*10**-6, TidalHeat, color=cmap(220), label='Tidal')
ax8.axvline(x=T_Solid,linestyle='--', color=cmap(20))
ax8.axvline(x=T_Desicc,linestyle='--', color=cmap(140))
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
ax9.axvline(x=T_Solid,linestyle='--', color=cmap(20))
ax9.axvline(x=T_Desicc,linestyle='--', color=cmap(140))
ax9.legend(loc='best', frameon=True)
ax9.set_ylabel('$CO_2$ Mass (kg)')
ax9.set_yscale('log')
xup = max(M_CO2_atm[i_end],M_CO2_sol[i_end],M_CO2_mo[i_end]-M_CO2_atm[i_end])
ax9.set_ylim([1e19,1e23])
ax9.set_xlabel('Time (Myrs)')
# ax8.set_ylim([292,300])
# ax8.set_yticks([292,294,296,298,300])
# ax8.get_yaxis().set_major_formatter(matplotlib.ticker.FuncFormatter(lambda x, p: format(int(x))))

plt.subplots_adjust(left=0.05, right=0.99, top=0.93, bottom=0.07)
plt.savefig('plot.png')
# plt.savefig('../../../../../../Uni/Masterarbeit/Results_VPlanet/Trappist-1/'+str(Name_Folder)+'_final/'+str(clock)+'_'+str(Name_Folder)+'_'+str(M_water_mo[0])+'TO.png')
# plt.savefig('../../../../../../Uni/Masterarbeit/Results_VPlanet/Trappist-1/'+str(Name_Folder)+'_final/'+str(clock)+'_'+str(Name_Folder)+'_'+str(M_water_mo[0])+'TO_ecc_'+str(Ecc)+'_40K_'+str(K40)+'.png')

# plt.show()

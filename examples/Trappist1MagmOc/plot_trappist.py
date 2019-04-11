#################################################
#   Plot results for TRAPPIST-1 from VPLanet    #
# Modules used: MagmOc, AtmEsc, RadHeat, EqTide #
#################################################

import numpy as np
import matplotlib as mpl
import matplotlib.pyplot  as plt
import seaborn as sns

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
A = 0
# 1: plot individual graphs, 0: plot all graphs together
individual = 0
# 1: plot time axis log scale, 0: plot time axis linear
log_plot = 1
Initial_water = 1

# Which planet to plot:
# 1-b, 2-c, 3-d, 4-e, 5-f, 6-g, 7-h
N_Planet = 5
# Eccentricity
Ecc = 0.001
# 40-K abundance (in Earth abundances)
K40 = 1.
# Solidification time:
T_Solid = [0,0,0,13.6188,8.1224,17.1552]

# read data
if N_Planet == 4:
    # TRAPPIST-1 e #
    data = np.loadtxt("Trappist1.Trappist1e.forward")
    R_N_Planet = 0.918
    M_N_Planet = 0.62
    Name_Planet = 'Trappist-1 e'
elif N_Planet == 5:
    # TRAPPIST-1 f #
    data = np.loadtxt("Trappist1.Trappist1f.forward")
    R_N_Planet = 1.045
    M_N_Planet = 0.68
    Name_Planet = 'Trappist-1 f'
elif N_Planet == 6:
    # TRAPPIST-1 g #
    data = np.loadtxt("Trappist1.Trappist1g.forward")
    R_N_Planet = 1.127
    M_N_Planet = 1.341
    Name_Planet = 'Trappist-1 g'

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
r_p    = R_N_Planet * REARTH
m_p    = M_N_Planet * MEARTH
g      = (BIGG * m_p) / (r_p ** 2)
rho_m  = 4000

diff_old = 1e9
for i in range(n_time):

    diff = abs(time[i] - T_Solid[N_Planet-1]*1e6)
    if (diff < diff_old):
        n_t_solid = i
    diff_old = diff

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

print('Solidification Time           = ',time[n_t_solid]*1e-6,  ' Myr')
print('Water mass locked in mantle   = ',M_water_sol[n_t_solid], ' TO')
print('Oxygen mass locked in mantle  = ',M_O_sol[n_t_solid],     ' kg')
print('Water pressure in atmosphere  = ',Press_H2O[n_t_solid],  ' bar')
print('Oxygen pressure in atmosphere = ',Press_O[n_t_solid],    ' bar')
print('Fe2O3 mass frac in mantle     = ',Frac_Fe2O3[n_t_solid])

# integrate volume of magma ocean over time:
MagmOc_Vol_Time = 0
for i in range(1,n_time):
    MagmOc_Vol_Time = MagmOc_Vol_Time + 4./3 * np.pi * (r_p**3 - r_sol[i]**3) * (time[i]-time[i-1])

print('Time integrated magma ocean volume = ', MagmOc_Vol_Time, ' m^3*yr')

# calculate total amount of Fe2O3 in mantle:
Fe2O3_rad = 0
for i in range(n_time):
    if r_sol[i] > r_sol[0]:
        Fe2O3_rad = Frac_Fe2O3[i] * 4./3 * np.pi * (r_sol[i]**3 - r_sol[i-1]**3)

M_O_sol_tot = Fe2O3_rad * rho_m * 0.5 * 15.999/79.844
print('Oxygen mass in mantle in Fe2O3 = ', M_O_sol_tot, ' kg')

### Plot ###

fig = plt.figure()
fig.suptitle(''+str(Name_Planet)+': $M^{ini}_{H_2O} = $ '+str(M_water_mo[0])+' TO, $e = $'+str(Ecc)+', Abundance of $^{40}K =$'+str(K40)+' $\\times$ Earth', fontsize=16, fontweight='bold')

# --- Temperature --- #
ax1 = fig.add_subplot(331)
ax1.plot(time*10**-6, Tpot, label='$T_p$', color=cmap(0))
ax1.plot(time*10**-6, Tsurf, label='$T_{surf}$', linestyle='--', color=cmap(220))
ax1.axvline(x=T_Solid[N_Planet-1],linestyle='--', color=cmap(20))
ax1.legend(loc='best', frameon=True)
ax1.set_ylabel('Temperature (K)')
ax1.set_xscale('log')

# --- Solidification Radius --- #
ax2 = fig.add_subplot(332, sharex=ax1)
ax2.plot(time*10**-6, r_sol/R_N_Planet, label='$r_s$', color=cmap(0))
ax2.axvline(x=T_Solid[N_Planet-1],linestyle='--', color=cmap(20))
ax2.set_ylim([0.5,1])
ax2.set_ylabel('Solidification radius ($r_p$)')

# --- Water Mass --- #
ax3 = fig.add_subplot(333, sharex=ax1)
ax3.plot(time*10**-6, M_water_mo-M_water_atm/TO, label='magma ocean', color=cmap(0))
ax3.plot(time*10**-6, M_water_atm/TO, label='atmosphere', color=cmap(220))
ax3.plot(time*10**-6, M_water_sol, label='solid', color=cmap(70))
ax3.axvline(x=T_Solid[N_Planet-1],linestyle='--', color=cmap(20))
# ax3.set_ylim([0.01,1050])
ax3.legend(loc='best', frameon=True)
ax3.set_ylabel('Water Mass (TO)')
ax3.set_yscale('log')

# --- Atmospheric pressures --- #
ax4 = fig.add_subplot(334, sharex=ax1)
ax4.plot(time*10**-6, Press_H2O, label='$H_2O$', color=cmap(0))
ax4.plot(time*10**-6, Press_O, label='$O$', color=cmap(220))
ax4.axvline(x=T_Solid[N_Planet-1],linestyle='--', color=cmap(20))
ax4.legend(loc='best', frameon=True)
ax4.set_ylabel('Atmospheric pressure (bar)')
ax4.set_yscale('log')

# --- Mass fractions magmoc --- #
ax5 = fig.add_subplot(335, sharex=ax1)
ax5.plot(time*10**-6, Frac_H2O, label='$H_2O$', color=cmap(0))
ax5.plot(time*10**-6, Frac_Fe2O3, label='$Fe_2O_3$', color=cmap(220))
ax5.axvline(x=T_Solid[N_Planet-1],linestyle='--', color=cmap(20))
ax5.legend(loc='best', frameon=True)
ax5.set_ylabel('Mass frac in magma ocean')
# ax5.set_yscale('log')
# ax5.set_ylim([1e16,5e21])

# --- Oxygen mass --- #
ax6 = fig.add_subplot(336, sharex=ax1)
ax6.plot(time*10**-6, M_O_mo-M_O_atm, label='magma ocean', color=cmap(0))
ax6.plot(time*10**-6, M_O_atm, label='atmosphere', color=cmap(220))
ax6.plot(time*10**-6, M_O_sol, label='solid', color=cmap(70))
ax6.axvline(x=T_Solid[N_Planet-1],linestyle='--', color=cmap(20))
ax6.legend(loc='best', frameon=True)
ax6.set_ylabel('Oxygen Mass (kg)')
ax6.set_yscale('log')
# ax6.set_ylim([1e16,1e21])

# --- Atmosphere Net FLux --- #
ax7 = fig.add_subplot(337, sharex=ax1)
ax7.plot(time*10**-6, NetFluxAtmo, color=cmap(0))
ax7.set_ylabel('Atmospheric net flux ($W/m^2$)')
ax7.axvline(x=T_Solid[N_Planet-1],linestyle='--', color=cmap(20))
# ax7.set_yscale('log')
ax7.set_xlabel('Time (Myrs)')

# --- Mantle Heating --- #
ax8 = fig.add_subplot(338, sharex=ax1)
ax8.plot(time*10**-6, RadioHeat, color=cmap(0), label='Radiogenic')
ax8.plot(time*10**-6, TidalHeat, color=cmap(220), label='Tidal')
ax8.axvline(x=T_Solid[N_Planet-1],linestyle='--', color=cmap(20))
ax8.legend(loc='best', frameon=True)
ax8.set_ylabel('Mantle Heating Power (TW)')
ax8.set_yscale('log')
ax8.set_xlabel('Time (Myrs)')

# ax8.set_ylim([292,300])
# ax8.set_yticks([292,294,296,298,300])
# ax8.get_yaxis().set_major_formatter(matplotlib.ticker.FuncFormatter(lambda x, p: format(int(x))))

###
### Constency checks
###

# ax7 = fig.add_subplot(337, sharex=ax1)
# ax7.plot(time*10**-6, N_H_space, label='space')
# ax7.plot(time*10**-6, N_H_atm, label='atmosphere')
# ax7.plot(time*10**-6, N_H_mo, label='magma ocean')
# ax7.plot(time*10**-6, N_H_sol, label='solid')
# ax7.plot(time*10**-6, N_H_tot, label='total', linestyle=':')
# ax7.legend(loc='best', frameon=True)
# ax7.set_xlabel('Time (Myrs)')
# ax7.set_ylabel('Number of H atoms ($10^{45}$)')
# ax7.set_yscale('log')
# ax7.set_ylim([1e-1,1e5])
#
# ax8 = fig.add_subplot(338, sharex=ax1)
# ax8.plot(time*10**-6, N_O_space, label='space')
# ax8.plot(time*10**-6, N_O_atm, label='atmosphere')
# ax8.plot(time*10**-6, N_O_mo, label='magma ocean')
# ax8.plot(time*10**-6, N_O_sol, label='solid')
# ax8.plot(time*10**-6, N_O_tot, label='total', linestyle=':')
# ax8.legend(loc='best', frameon=True)
# ax8.set_xlabel('Time (Myrs)')
# ax8.set_ylabel('Number of O atoms ($10^{45}$)')
# ax8.set_yscale('log')
# ax8.set_ylim([1e-1,1e5])

# --- Consistency --- #
ax9 = fig.add_subplot(339, sharex=ax1)
ax9.plot(time*10**-6, 100*(N_H_tot/N_H_tot[0]-1), label='H', color=cmap(0))
ax9.plot(time*10**-6, 100*(N_O_tot/N_O_tot[0]-1), label='O', linestyle='--', color=cmap(0))
ax9.plot(time*10**-6, 100*((N_O_tot/N_O_tot[0]-1)-(N_H_tot/N_H_tot[0]-1)), label='O-H', linestyle=':', color=cmap(0))
ax9.axvline(x=T_Solid[N_Planet-1],linestyle='--', color=cmap(20))
ax9.legend(loc='best', frameon=True)
ax9.set_xlabel('Time (Myrs)')
ax9.set_ylabel('Ratio of atoms gained (%)')
# ax8.set_yscale('log')
#
# ax9 = fig.add_subplot(339, sharex=ax1)
# ax9.plot(time*10**-6, N_H_tot-N_H_tot[0], label='H')
# ax9.plot(time*10**-6, N_O_tot-N_O_tot[0], label='O')
# ax9.plot(time*10**-6, (N_O_tot-N_O_tot[0])-(N_H_tot-N_H_tot[0])/2., label='O-H/2', linestyle='--')
# ax9.legend(loc='best', frameon=True)
# ax9.set_xlabel('Time (Myrs)')
# ax9.set_ylabel('Number of atoms lost ($10^{45}$)')

plt.show()

# plt.figure()
# plt.title('GJ1132b: $M_{water}^{ini} = $'+str(M_water_mo[0])+' TO', fontsize=18, fontweight='bold')
# # plt.plot(time*10**-6, Tpot, label='T_p', linewidth=3.0)
# # plt.plot(time*10**-6, Tsurf, label='T_surf', linestyle='--', linewidth=3.0)
# plt.plot(time*10**-6, Tpot, label='T_p', linewidth=3.0, color=cmap(0))
# plt.plot(time*10**-6, Tsurf, label='T_surf', linestyle='--', linewidth=3.0, color=cmap(0))
# plt.xlim([1e-6,time[i_end+1]*1e-6])
# # ax1.set_ylim([0,4100])
# plt.legend(loc='best', frameon=True, fontsize=16)
# plt.xlabel('Time (Myrs)', fontsize=18, fontweight='bold')
# plt.ylabel('Temperature (K)', fontsize=18, fontweight='bold')
# if log_plot == 1:
#     plt.xscale('log')
# plt.xlim([1e-6,time[i_end+1]*1e-6])
# plt.tick_params(labelsize=14)
# plt.show()

# r_core = np.zeros(n_time)+r_c
#
# if individual == 1:
#     ## plot individual figures
#
#     plt.figure()
#     plt.title('GJ1132b: $M_{water}^{ini} = $'+str(Initial_water)+' TO', fontsize=18, fontweight='bold')
#     # plt.plot(time*10**-6, Tpot, label='T_p', linewidth=3.0)
#     # plt.plot(time*10**-6, Tsurf, label='T_surf', linestyle='--', linewidth=3.0)
#     plt.plot(time*10**-6, Tpot, label='T_p', linewidth=3.0, color=cmap(0))
#     plt.plot(time*10**-6, Tsurf, label='T_surf', linestyle='--', linewidth=3.0, color=cmap(0))
#     plt.xlim([1e-6,time[i_end+1]*1e-6])
#     # ax1.set_ylim([0,4100])
#     plt.legend(loc='best', frameon=True, fontsize=16)
#     plt.xlabel('Time (Myrs)', fontsize=18, fontweight='bold')
#     plt.ylabel('Temperature (K)', fontsize=18, fontweight='bold')
#     if log_plot == 1:
#         plt.xscale('log')
#     plt.xlim([1e-6,time[i_end+1]*1e-6])
#     plt.tick_params(labelsize=14)
#     plt.show()
#
#     plt.figure()
#     plt.title('GJ1132b: $M_{water}^{ini} = $'+str(Initial_water)+' TO', fontsize=18, fontweight='bold')
#     # plt.plot(time*10**-6, r_sol/r_p, label='$r_s$', color=cmap(100), linewidth=3.0)
#     # plt.plot(time*10**-6, r_core/r_p, label='$r_c$', color=cmap(220), linestyle='--', linewidth=3.0)
#     plt.plot(time*10**-6, r_sol, label='$r_s$', color=cmap(0), linewidth=3.0)
#     # plt.plot(time*10**-6, r_core, label='$r_c$', color=cmap(0), linestyle='--', linewidth=3.0)
#     plt.ylim([0.5,1])
#     plt.legend(loc='best', frameon=True, fontsize=16)
#     plt.xlabel('Time (Myrs)', fontsize=18, fontweight='bold')
#     plt.ylabel('Solidification radius ($r_p$)', fontsize=18, fontweight='bold')
#     if log_plot == 1:
#         plt.xscale('log')
#     plt.xlim([1e-6,time[i_end+1]*1e-6])
#     plt.tick_params(labelsize=14)
#     plt.show()
#
#     plt.figure()
#     plt.title('GJ1132b: $M_{water}^{ini} = $'+str(Initial_water)+' TO', fontsize=18, fontweight='bold')
#     # plt.plot(time*10**-6, q_m, label='Mantle heat flux', linewidth=3.0)
#     # plt.plot(time*10**-6, Flux_OLR, label='Outgoing longwave radiation', linewidth=3.0)
#     # plt.plot(time*10**-6, Flux_BOL, label='Absorbed stellar radiation', linewidth=3.0)
#     # plt.plot(time*10**-6, Flux_XUV, label='Absorbed stellar XUV flux', linewidth=3.0)
#     plt.plot(time*10**-6, q_m, label='Mantle heat flux', linewidth=3.0, color=cmap(0))
#     plt.plot(time*10**-6, OLR, label='Net Flux Atmosphere', linewidth=3.0, linestyle='--', color=cmap(0))
#     # plt.plot(time*10**-6, ASR, label='Absorbed stellar radiation', linewidth=3.0, linestyle='-.', color=cmap(0))
#     plt.plot(time*10**-6, XUV, label='Absorbed stellar XUV flux', linewidth=3.0, linestyle=':', color=cmap(0))
#     plt.legend(loc='best', frameon=True, fontsize=16)
#     plt.xlabel('Time (Myrs)', fontsize=18, fontweight='bold')
#     plt.ylabel('Flux ($W/m^2$)', fontsize=18, fontweight='bold')
#     plt.yscale('log')
#     if log_plot == 1:
#         plt.xscale('log')
#     plt.xlim([1e-6,time[i_end+1]*1e-6])
#     plt.tick_params(labelsize=14)
#     plt.show()
#
#     plt.figure()
#     plt.title('GJ1132b: $M_{water}^{ini} = $'+str(Initial_water)+' TO', fontsize=18, fontweight='bold')
#     # plt.plot(time*10**-6, M_water_mo/M_water_mo[0], label='magma ocean + atm', linewidth=3.0)
#     # plt.plot(time*10**-6, M_water_atm/M_water_mo[0], label='atmosphere', linewidth=3.0)
#     # plt.plot(time*10**-6, M_water_sol/M_water_mo[0], label='solid', linewidth=3.0)
#     plt.plot(time*10**-6, M_water_mo/M_water_mo[0], label='magma ocean + atm', linewidth=3.0, color=cmap(0))
#     plt.plot(time*10**-6, M_water_atm/M_water_mo[0], label='atmosphere', linewidth=3.0, linestyle='--', color=cmap(0))
#     plt.plot(time*10**-6, M_water_sol/M_water_mo[0], label='solid', linewidth=3.0, linestyle=':', color=cmap(0))
#     plt.ylim([1e-3,1.05])
#     plt.legend(loc='best', frameon=True, fontsize=16)
#     plt.xlabel('Time (Myrs)', fontsize=18, fontweight='bold')
#     plt.ylabel('Water Mass Fraction', fontsize=18, fontweight='bold')
#     plt.yscale('log')
#     if log_plot == 1:
#         plt.xscale('log')
#     plt.xlim([1e-6,time[i_end+1]*1e-6])
#     plt.tick_params(labelsize=14)
#     plt.show()
#
#     plt.figure()
#     plt.title('GJ1132b: $M_{water}^{ini} = $'+str(Initial_water)+' TO', fontsize=18, fontweight='bold')
#     plt.plot(time*10**-6, Press_atm, label='total pressure', linewidth=3.0, color=cmap(0))
#     plt.plot(time*10**-6, Press_H2O, label='partial pressure H2O', linestyle='--', linewidth=3.0, color=cmap(0))
#     plt.plot(time*10**-6, Press_O, label='partial pressure O', linestyle=':', linewidth=3.0, color=cmap(0))
#     plt.legend(loc='best', frameon=True, fontsize=16)
#     plt.xlabel('Time (Myrs)', fontsize=18, fontweight='bold')
#     plt.ylabel('Atmospheric pressure (bar)', fontsize=18, fontweight='bold')
#     if log_plot == 1:
#         plt.xscale('log')
#     plt.xlim([1e-6,time[i_end+1]*1e-6])
#     plt.yscale('log')
#     plt.tick_params(labelsize=14)
#     plt.show()
#
# else:
#     ## plot multiple figures

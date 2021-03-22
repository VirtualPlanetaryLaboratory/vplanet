import numpy as np
import matplotlib as mpl
import matplotlib.pyplot  as plt
import matplotlib.gridspec as gridspec
import sys
import matplotlib.ticker as mticker
import subprocess
try:
    import vplot as vpl
except:
    print('Cannot import vplot. Please install vplot.')

plt.close('all')
# Set style for plot #
mpl.rcParams['lines.linewidth'] = 3
mpl.rcParams['axes.labelsize'] = 13
mpl.rcParams['axes.labelweight'] = 'bold'
mpl.rcParams['xtick.labelsize'] = 12
mpl.rcParams['ytick.labelsize'] = 12
mpl.rcParams['legend.fontsize'] = 15
######################

#Runs VPLANET
subprocess.call(['vplanet', 'vpl.in'])


cmap=plt.get_cmap('nipy_spectral')

# read data
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

# expand all by one more datapoint at 2Myr
n_time = len(time)
t_sol = time[n_time-1]

time        = np.append(time,4e6)
Tpot        = np.append(Tpot,Tpot[n_time-1])
Tsurf       = np.append(Tsurf,Tsurf[n_time-1])
r_sol       = np.append(r_sol,r_sol[n_time-1])
M_water_mo  = np.append(M_water_mo,M_water_mo[n_time-1])
M_water_sol = np.append(M_water_sol,M_water_sol[n_time-1])
M_O_mo      = np.append(M_O_mo,M_O_mo[n_time-1])
M_O_sol     = np.append(M_O_sol,M_O_sol[n_time-1])
Press_H2O   = np.append(Press_H2O,Press_H2O[n_time-1])
Press_O     = np.append(Press_O,Press_O[n_time-1])
M_H_Space   = np.append(M_H_Space,M_H_Space[n_time-1])
M_O_Space   = np.append(M_O_Space,M_O_Space[n_time-1])
Frac_Fe2O3  = np.append(Frac_Fe2O3,Frac_Fe2O3[n_time-1])
NetFluxAtmo = np.append(NetFluxAtmo,NetFluxAtmo[n_time-1])
Melt_frac   = np.append(Melt_frac,Melt_frac[n_time-1])


n_time = len(time)
i_end  = n_time-1

M_water_atm = np.zeros(n_time)
M_O_atm     = np.zeros(n_time)


TO     = 1.39e21                    # mass of 1 Terr. Ocean [kg]
REARTH = 6.3781e6                   # Earth radius [m]
MEARTH = 5.972186e24                # Earth mass [kg]
BIGG   = 6.67428e-11                # Gravitational constant [m**3/kg/s**2]
r_p    = REARTH                     # Planet radius [m]
r_core = 3.4e6/r_p
m_p    = MEARTH                     # Planet mass [kg]
g      = (BIGG * m_p) / (r_p ** 2)  # Surface gravity [m/s**2]

for i in range(n_time):
    M_water_atm[i] = Press_H2O[i]*1e5 * 4 * np.pi * r_p**2 / g
    M_O_atm[i]     = Press_O[i]*1e5 * 4 * np.pi * r_p**2 / g

V_sol = np.zeros(len(r_sol))
for i in range(len(V_sol)):
    V_sol[i] = (r_sol[i]**3 - r_core**3)/((r_p/REARTH)**3 - r_core**3)

### Plot ###

fig = plt.figure(num=None, figsize=(7.5, 6.5), dpi=300, facecolor='w', edgecolor='k')

gs = fig.add_gridspec(4, 1)
ax11 = fig.add_subplot(gs[1, 0])
ax3 = fig.add_subplot(gs[2:, 0])
ax4 = fig.add_subplot(gs[0, 0], sharex=ax11)

ax1 = ax11.twinx()

ax1.set_ylabel('Temperature\n(K)',rotation=270,va='bottom')
ax1.set_xscale('log')
ax1.set_xlim([0.001,2])
ax1.set_ylim([1300,3200])
ax1.plot(time*10**-6, Tpot, label='$T_{surf}$', color='white', linewidth=5)
ax1.axvline(x=t_sol*1e-6,linestyle='--', color='lightgray',linewidth=2)

ax11.fill_between(time*10**-6, V_sol, color=cmap(0))
ax11.fill_between(time*10**-6, 1, V_sol, color='grey')
ax11.set_yticks([0.5,1])
ax11.set_ylim([0,1])
ax11.set_ylabel('Cumulate\nfraction')


ax4.fill_between(time*10**-6, Press_H2O, color='silver')
ax4.plot(time*10**-6, Press_H2O, color='grey')
ax4.axvline(x=t_sol*1e-6,linestyle='--', color='lightgray',linewidth=2)
ax4.set_ylabel('Atmospheric \npressure \n(bar)')
ax4.set_ylim([0,1500])

ax1.tick_params(axis='both', which='both', direction='in')
ax11.tick_params(axis='both', which='both', direction='in')
ax3.tick_params(axis='both', which='both', direction='in', left='on', right='on')
ax4.tick_params(axis='both', which='both', direction='in', left='on', right='on')
ax1.set_xticklabels([])
ax11.set_xticklabels([])

ax3.plot(time*10**-6, M_water_mo+M_water_sol, label='Total', color=cmap(0))
ax3.plot(time*10**-6, M_water_atm/TO, label='Atmosphere', color=cmap(220))
ax3.plot(time*10**-6, M_water_mo-M_water_atm/TO, label='Magma ocean', color=cmap(120))
ax3.plot(time*10**-6, M_water_sol, label='Solid mantle', color=cmap(60))
ax3.axvline(x=t_sol*1e-6,linestyle='--', color='lightgray',linewidth=2)
ax3.set_ylim([0.001,10])
ax3.set_xlim([0.001,2])

label_format = '{:,.0f}'
ax3.xaxis.set_major_locator(mticker.MaxNLocator(3))
ticks_loc = [1,1e-3,1e-2,1e-1,1]
ax3.xaxis.set_major_locator(mticker.FixedLocator(ticks_loc))
ax3.set_xticklabels([label_format.format(x) for x in ticks_loc])

ax3.set_xscale('log')
ax3.set_yscale('log')
ax3.set_xlabel('Time (Myr)')
ax3.set_ylabel('Reservoir size ($M_{EO}$)')

ax3.text(0.0012, 1.5, 'Magma Ocean',  color=cmap(120), fontsize='16')
ax3.text(0.02,   0.01,'Solid Mantle', color=cmap(60),  fontsize='16')
ax3.text(0.0012, 0.03,'Atmosphere',   color=cmap(220), fontsize='16')
ax3.text(2.05, 5,'Total',  va='center', color=cmap(0), fontsize='16')

ax4.text(2, 1550,'MO Solidification', ha='right',  color=cmap(0), fontsize='16')
ax4.text(0.03, 900,'Atmosphere',   color=cmap(0), fontsize='16')
ax4.plot([0.25,0.5],[850,400],linewidth=1,color=cmap(0))

ax11.text(0.02, 0.2,'Temperature',   color='white', fontsize='16')
ax11.text(0.002, 0.5, 'Melts', fontsize='16', bbox={'facecolor':'white', 'pad':5})
ax11.text(0.2, 0.6, 'Cumulates', fontsize='16', bbox={'facecolor':'white', 'pad':5})

bbox_props = dict(boxstyle="rarrow", fc="firebrick", ec="darkred", lw=2)
t = ax4.text(0.25, 150, "ou", ha="center", va="center", rotation=90, color='firebrick',
            size=15,
            bbox=bbox_props)
t2 = ax4.text(0.1, -50, "ou", ha="center", va="center", rotation=90, color='firebrick',
            size=15,
            bbox=bbox_props)

bb = t.get_bbox_patch()
bb.set_boxstyle("rarrow", pad=0.3)
bc = t2.get_bbox_patch()
bc.set_boxstyle("rarrow", pad=0.3)

plt.subplots_adjust(left=0.18, right=0.86, top=0.95, bottom=0.1, hspace=0)

if (sys.argv[1] == 'pdf'):
    plt.savefig('MagmOC_Earth.pdf')
if (sys.argv[1] == 'png'):
    plt.savefig('MagmOC_Earth.png')

# plt.savefig('Earth_5TO_Hamano.png')
# plt.savefig('Earth_5TO_Hamano.eps', format='eps')

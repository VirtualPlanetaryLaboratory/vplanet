import os as os
import numpy as np

# data = np.loadtxt("dir_names.txt", skiprows=1)
data = np.genfromtxt('dir_names2.txt',dtype='str', skip_header=1)
# datafile = open('dir_names.txt')
# data = datafile.readlines()
# datafile.close()

Number  = data[:,0]  # Number of folder
Planet  = data[:,1]  # Planet name (e,f,g)
Water   = data[:,2]  # Initial water mass [TO]
Ecc     = data[:,3]  # Eccentricity
K40Ab   = data[:,4]  # 40K abundance [Earth]

waterfile = open('water.txt')
water_values = waterfile.readlines()
waterfile.close()

eccfile = open('ecc2.txt')
ecc_values = eccfile.readlines()
eccfile.close()

radfile = open('rad.txt')
rad_values = radfile.readlines()
radfile.close()

planets = ['e','f','g']

n = 0

for p in range(len(planets)):
    for w in range(len(water_values)):
        # declare arrays
        solid_time         = np.zeros((len(rad_values),len(ecc_values)))
        solid_waterlocked  = np.zeros((len(rad_values),len(ecc_values)))
        solid_watertot     = np.zeros((len(rad_values),len(ecc_values)))
        solid_presswater   = np.zeros((len(rad_values),len(ecc_values)))
        solid_pressoxy     = np.zeros((len(rad_values),len(ecc_values)))

        atm_desicc         = np.zeros((len(rad_values),len(ecc_values)))
        desicc_time        = np.zeros((len(rad_values),len(ecc_values)))
        desicc_watertot    = np.zeros((len(rad_values),len(ecc_values)))
        desicc_presswater  = np.zeros((len(rad_values),len(ecc_values)))
        desicc_pressoxy    = np.zeros((len(rad_values),len(ecc_values)))

        # schreibe werte in array

        for e in range(len(ecc_values)):
            for r in range(len(rad_values)):
                resultfile = open('TR1_'+str(Number[n])+'/Results.dat')
                results = resultfile.readlines()
                resultfile.close()
                
                print(n,Number[n])
                
                if (n<48) or (n>53):
                    solid_time[r,e]        = results[4]
                    solid_waterlocked[r,e] = results[6]
                    solid_watertot[r,e]    = results[10]
                    solid_presswater[r,e]  = results[12]
                    solid_pressoxy[r,e]    = results[14]

                    if (float(results[2])==0):
                        atm_desicc[r,e]         = results[19]
                        desicc_time[r,e]        = results[21]
                        desicc_watertot[r,e]    = results[23]
                        desicc_presswater[r,e]  = results[25]
                        desicc_pressoxy[r,e]    = results[27]

                n = n + 1

        # schreibe array in file
        np.savetxt('Results_freeecc_TR1_'+planets[p]+'_'+water_values[w][:-1]+'_solid_time.dat',solid_time)
        np.savetxt('Results_freeecc_TR1_'+planets[p]+'_'+water_values[w][:-1]+'_solid_waterlocked.dat',solid_waterlocked)
        np.savetxt('Results_freeecc_TR1_'+planets[p]+'_'+water_values[w][:-1]+'_solid_watertot.dat',solid_watertot)
        np.savetxt('Results_freeecc_TR1_'+planets[p]+'_'+water_values[w][:-1]+'_solid_presswater.dat',solid_presswater)
        np.savetxt('Results_freeecc_TR1_'+planets[p]+'_'+water_values[w][:-1]+'_solid_pressoxy.dat',solid_pressoxy)
        np.savetxt('Results_freeecc_TR1_'+planets[p]+'_'+water_values[w][:-1]+'_atm_desicc.dat',atm_desicc)
        np.savetxt('Results_freeecc_TR1_'+planets[p]+'_'+water_values[w][:-1]+'_desicc_time.dat',desicc_time)
        np.savetxt('Results_freeecc_TR1_'+planets[p]+'_'+water_values[w][:-1]+'_desicc_watertot.dat',desicc_watertot)
        np.savetxt('Results_freeecc_TR1_'+planets[p]+'_'+water_values[w][:-1]+'_desicc_presswater.dat',desicc_presswater)
        np.savetxt('Results_freeecc_TR1_'+planets[p]+'_'+water_values[w][:-1]+'_desicc_pressoxy.dat',desicc_pressoxy)

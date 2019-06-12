import os as os
import numpy as np

# waterfile = open('water.txt')
water = ['1','2','5','10','20','50','100']
# waterfile.close()

planets = ['e','f','g']

num_dir = 2000

for p in range(len(planets)):
    Results_Trappist1_file = open('Results_Trappist1_'+planets[p]+'.txt','w')
    Results_Trappist1_file.write('# Results for Trappist1-'+planets[p]+'\n')
    Results_Trappist1_file.write('# Water mass [TO] \t Solidification time [Myr] \t Desiccation time [Myr] \t Water locked in mantle [TO] \t Total water in system [TO] \t Water pressure [bar] \t Oxygen pressure [bar] \n ')

    for w in range(len(water)):
        # new_directory = 'TR1_'+planets[p]+'_'+water[w]+'TO'
        new_directory = 'TR1_'+str(num_dir)+''

        resultfile = open(''+new_directory+'/Results.dat')
        results = resultfile.readlines()
        resultfile.close()

        solid_time   = float(results[4])
        if (float(results[2])==1):
            water_locked = float(results[6])
            water_tot    = float(results[10])
            press_water  = float(results[12])
            press_oxy    = float(results[14])
            desicc_time  = solid_time
        if (float(results[2])==2):
            water_locked = float(results[6])
            water_tot    = float(results[10])
            press_water  = float(results[12])
            press_oxy    = float(results[14])
            desicc_time  = solid_time
        else:
            water_locked = float(results[23])
            water_tot    = float(results[25])
            press_water  = float(results[27])
            press_oxy    = float(results[29])
            if (float(results[19])==1):
                desicc_time = float(results[21])
            else:
                desicc_time = 7e3


        if (press_water<0.006):
            press_water = 0

        Results_Trappist1_file.write(''+str(water[w])+'\t'+str(solid_time)+'\t'+str(desicc_time)+'\t'+str(water_locked)+'\t'+str(water_tot)+'\t'+str(press_water)+'\t'+str(press_oxy)+'\n')

        num_dir = num_dir + 1

    Results_Trappist1_file.close()

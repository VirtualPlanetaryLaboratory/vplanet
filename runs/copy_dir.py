import os as os
import numpy as np

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
# Copy directories

for w in range(len(water)):
    for e in range(len(ecc)):
        for r in range(len(rad)):
            for p in range(len(planets)):
                new_directory = 'TR1_'+planets[p][:-1]+'_'+water[w][:-1]+'TO_ecc_'+ecc[e][:-1]+'_rad_'+rad[r][:-1]+''
                os.system('cp -r TR1_'+planets[p][:-1]+'_example '+new_directory+'')

                ## Change input files

                # e.in

                inputfile = open(''+new_directory+'/'+planets[p][:-1]+'.in')
                input = inputfile.readlines()
                inputfile.close()

                rad_p = float(rad[r][:-1])*masses[p]

                input[11] = 'dWaterMassAtm		-'+water[w][:-1]+'		  # Initial water mass in the system (neg: terrestrial oceans, default: -1) \n'
                input[38] = 'd40KNumMan    -'+str(rad_p)+'\n'
                input[67] = 'dEcc          '+ecc[e][:-1]+'                         # Eccentricity \n'

                inputfile = open(''+new_directory+'/'+planets[p][:-1]+'.in','w')
                for l in range(len(input)):
                    inputfile.write(input[l])
                inputfile.close()

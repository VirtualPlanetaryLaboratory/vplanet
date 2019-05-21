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

# 1 - 324: ecc locked, tidally locked -- OLD
# 325 - 485: ecc not locked -- OLD
# 487 - 702: ecc locked, tidally locked -- OLD
# 703 - 864: ecc not locked, tidally locked -- OLD
# 865 - 1080: ecc locked, tidally locked
# 1081 - 1242: ecc not locked, tidally locked
dir_number = 865
bFixOrbit  = 1

# File with numbers and input parameters
dir_names_file = open('dir_names5.txt', 'w')
dir_names_file.write('# Number \t Planet \t Water mass [TO] \t Eccentricity \t 40K abundance [Earth] \t bFixOrbit \n ')

for p in range(len(planets)):
    for w in range(len(water)):
        for e in range(len(ecc)):
            for r in range(len(rad)):
                #new_directory = 'TR1_'+planets[p]+'_'+water[w][:-1]+'TO_ecc_'+ecc[e][:-1]+'_rad_'+rad[r][:-1]+''
                new_directory = 'TR1_'+str(dir_number)+''
                os.system('cp -r TR1_'+planets[p]+'_example '+new_directory+'')

                ## Change input files
                inputfile = open(''+new_directory+'/'+planets[p]+'.in')
                input = inputfile.readlines()
                inputfile.close()

                rad_p = float(rad[r][:-1])*masses[p]

                input[12] = 'dWaterMassAtm		-'+water[w][:-1]+'		  # Initial water mass in the system (neg: terrestrial oceans, default: -1) \n'
                input[39] = 'd40KNumMan    -'+str(rad_p)+'\n'
                input[63] = 'bFixOrbit 	'+str(bFixOrbit)+'		   # Keep dEcc constant? \n'
                input[68] = 'dEcc          '+ecc[e][:-1]+'                         # Eccentricity \n'

                inputfile = open(''+new_directory+'/'+planets[p]+'.in','w')
                for l in range(len(input)):
                    inputfile.write(input[l])
                inputfile.close()

                # write input parameters to list of folder numbers & update folder number
                dir_names_file.write(''+str(dir_number)+'\t'+planets[p]+'\t'+water[w][:-1]+'\t'+ecc[e][:-1]+'\t'+rad[r][:-1]+'\t'+str(bFixOrbit)+'\n')
                dir_number = dir_number + 1

dir_names_file.close()

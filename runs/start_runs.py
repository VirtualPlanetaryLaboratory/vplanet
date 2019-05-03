import os as os
import time as time
import PT_free as PT

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

# Copy directories

for w in range(len(water)):
    for e in range(len(ecc)):
        for r in range(len(rad)):
            for p in range(len(planets)):
                new_directory = 'TR1_'+planets[p]+'_'+water[w][:-1]+'TO_ecc_'+ecc[e][:-1]+'_rad_'+rad[r][:-1]+''

                if (not os.path.isfile(new_directory+'/Trappist1.'+planets[p]+'.forward')):
                    while not PT.PT_free():
                        time.sleep(30)

                    #os.system('cd '+fnames[i][:-1]+' ')
                    #print('changed to folder')
                    #qsub -q queue2g64c@bachelor-node04 -N name -t 1:108 -tc 10 -cwd runners.sh
                    os.system('oqsubBA_queue1g1c '+new_directory+' '+new_directory+'/vplanet vpl.in')
                    #print('submitted')
                    #os.system('cd ..')
                    #print('changed back to runs')
                    time.sleep(4)

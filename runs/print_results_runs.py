import os as os
import numpy as np

data = np.genfromtxt('dir_names2.txt',dtype='str', skip_header=1)

Number  = data[:,0]  # Number of folder
Planet  = data[:,1]  # Planet name (e,f,g)

No_results = []

for n in range(len(Number)):
    os.chdir('TR1_'+Number[n]+'')
    file_exists = 0
    for file in os.listdir('./'):
        if file.endswith('.'+Planet[n]+'.forward'):
            os.system('python print_results.py')
            file_exists = 1
    os.chdir('..')

    if (file_exists == 0):
        No_results.append(Number[n])

noresfile = open('no_results_in_dir2.dat','w')
for l in range(len(No_results)):
    noresfile.write(''+No_results[l]+'\n')
noresfile.close()

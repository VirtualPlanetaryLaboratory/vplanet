import os as os
import numpy as np

data = np.genfromtxt('dir_names.txt',dtype='str', skip_header=1)

Number  = data[:,0]  # Number of folder

for n in range(len(Number)):
    os.system('python TR1_'+Number[n]+'/print_results.py')
    

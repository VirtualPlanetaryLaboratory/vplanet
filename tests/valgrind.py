import subprocess
import numpy as np
import os
import sys

subdir = [f.name for f in os.scandir('.') if f.is_dir() ]

# First make debug
sys.stdout.write('Making VPLanet...')
sys.stdout.flush()
os.chdir('..')
subprocess.run(['make debug >& /dev/null'], shell=True)
os.chdir('tests')

print(' done.')
for sub in subdir:
    print(sub)
    os.chdir(sub)
    subprocess.run(['valgrind --track-origins=yes ../../vplanet vpl.in >& valgrind.log'], shell=True)
    os.chdir('..')

print('Done!')

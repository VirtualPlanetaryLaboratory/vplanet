from vplot import GetOutput
import subprocess as sub
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))
import multiprocessing as mp
import warnings

def test_mpstatus():
    #gets the number of cores on the machine
    cores = str(mp.cpu_count())
    if cores == 1:
        warnings.warn("There is only 1 core on the machine",stacklevel=3)
    else:
        #removes checkpoint_file
        cp = cwd+'/.MP_Status'
        sub.run(['rm', cp],cwd=cwd)
        #removes the folders from when vspace is ran
        dir = cwd+'/MP_Status'
        sub.run(['rm', '-rf', dir],cwd=cwd)
        #runs vspace
        sub.run(['python','../../vspace/vspace/vspace.py','vspace.in'],cwd=cwd)
        #runs mp checkpoint
        sub.run(['python', '../../multi-planet/multi-planet.py','vspace.in','-c',cores],cwd=cwd)
        #runs mpstatus
        sub.run(['python', '../../multi-planet/mpstatus.py'],cwd=cwd)

        #gets list of folders
        folders = sorted([f.path for f in os.scandir(dir) if f.is_dir()])

        for i in range(len(folders)):
            os.chdir(folders[i])
            assert os.path.isfile('earth.earth.forward') == True
            os.chdir('../')

if __name__ == "__main__":
    test_mpstatus()

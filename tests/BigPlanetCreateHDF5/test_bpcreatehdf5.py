from vplot import GetOutput
import subprocess as sub
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))
import multiprocessing as mp
import warnings
import bigplanet as bp

def test_bpcreatehdf5():
    sub.run(['python','setup.py','install'],cwd = '../../bigplanet/')
    #gets the number of cores on the machine
    cores = str(mp.cpu_count())
    if cores == 1:
        warnings.warn("There is only 1 core on the machine",stacklevel=3)
    else:
        #removes checkpoint files
        cp = cwd+'/.BP_CreateHDF5'
        sub.run(['rm', cp],cwd=cwd)
        cp_hdf5 = cwd+'/.BP_CreateHDF5_hdf5'
        sub.run(['rm', cp_hdf5],cwd=cwd)
        #removes the folders from when vspace is ran
        dir = cwd+'/BP_CreateHDF5'
        sub.run(['rm', '-rf', dir],cwd=cwd)
        #runs vspace
        sub.run(['python','../../vspace/vspace/vspace.py','vspace.in'],cwd=cwd)
        #runs multi-planet
        sub.run(['python', '../../multi-planet/multi-planet.py','vspace.in','-c',cores],cwd=cwd)
        #runs bigplanet
        sub.run(['python', '../../bigplanet/bigplanet/bigplanet.py','vspace.in','-c',cores],cwd=cwd)

        #gets list of folders
        folders = sorted([f.path for f in os.scandir(dir) if f.is_dir()])
        #checks if the hdf5 files exist
        assert os.path.isfile('BP_CreateHDF5.hdf5') == True
        for i in range(len(folders)):
            os.chdir(folders[i])
            split = folders[i].split('/')[-1]
            assert os.path.isfile(split + '.hdf5') == True
            os.chdir('../')

if __name__ == "__main__":
    test_bpcreatehdf5()

from vplot import GetOutput
import subprocess as sub
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))
import multiprocessing as mp

def test_mp_serial():
    #removes checkpoint_file
    checkpoint = cwd+'/.MP_Serial'
    sub.run(['rm', checkpoint],cwd=cwd)
    #removes the folders from when vspace is ran
    dir = cwd+'/MP_Serial'
    sub.run(['rm', '-rf', dir],cwd=cwd)
    #runs vspace
    sub.run(['python','../../vspace/vspace/vspace.py','vspace.in'],cwd=cwd)
    #runs serial
    sub.run(['python', '../../multi-planet/multi-planet.py','vspace.in','-c','1'],cwd=cwd)

    folders = sorted([f.path for f in os.scandir(dir) if f.is_dir()])

    for i in range(len(folders)):
        os.chdir(folders[i])
        assert os.path.isfile('earth.earth.forward') == True
        os.chdir('../')

if __name__ == "__main__":
    test_mp_serial()

from vplot import GetOutput
import subprocess as sub
import numpy as np
import os
import multiprocessing as mp
import warnings
import sys
import pytest
import shutil


cwd = os.path.dirname(os.path.realpath(__file__))


def test_bpcreatehdf5():
    # gets the number of cores on the machine
    cores = str(mp.cpu_count())
    if cores == 1:
        warnings.warn("There is only 1 core on the machine", stacklevel=3)
    else:
        # removes checkpoint files
        # removes the folders from when vspace is ran
        for path in [
            cwd + "/.BP_CreateHDF5",
            cwd + "/.BP_CreateHDF5_hdf5",
            cwd + "/BP_CreateHDF5",
            cwd + "/BP_CreateHDF5.hdf5",
        ]:
            if os.path.exists(path):
                shutil.rmtree(path)

        # runs vspace
        sub.run(["vspace", "vspace.in"], cwd=cwd)

        # runs multi-planet
        sub.run(
            ["multi-planet", "vspace.in", "-c", cores], cwd=cwd,
        )

        # runs bigplanet
        sub.run(
            ["bigplanet", "vspace.in", "-c", cores,], cwd=cwd,
        )

        # gets list of folders
        folders = sorted([f.path for f in os.scandir(dir) if f.is_dir()])

        # checks if the hdf5 files exist
        assert os.path.isfile((dir + ".hdf5")) == True


if __name__ == "__main__":
    test_bpcreatehdf5()

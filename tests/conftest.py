import glob
import os
import shutil
import subprocess as subp
import sys

import numpy as np
import pytest

import vplanet

# Make the `benchmark` script discoverable by the tests
sys.path.insert(1, os.path.abspath(os.path.dirname(__file__)))

# Set to False to keep .log, .forward, etc files
CLEAN_OUTPUTS = True


@pytest.fixture(scope="module")
def vplanet_output(request):
    path = os.path.abspath(os.path.dirname(request.fspath))
    infile = os.path.join(path, "vpl.in")
    output = vplanet.run(infile, quiet=False, clobber=True, C=True)
    yield output
    if CLEAN_OUTPUTS:
        for file in (
            glob.glob(f"{path}/*.log")
            + glob.glob(f"{path}/*.forward")
            + glob.glob(f"{path}/*.backward")
            + glob.glob(f"{path}/*.Climate")
        ):
            os.remove(file)
        for directory in glob.glob(f"{path}/SeasonalClimateFiles"):
            shutil.rmtree(directory)

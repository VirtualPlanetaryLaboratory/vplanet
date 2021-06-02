import vplanet
import numpy as np
import os
import glob
import pytest
import shutil
import sys

# Make the `benchmark` script discoverable by the tests
sys.path.insert(1, os.path.abspath(os.path.dirname(__file__)))

# Set to False to keep .log, .forward, etc files
CLEAN_OUTPUTS = True


@pytest.fixture(scope="module")
def vplanet_output(request):
    path = os.path.abspath(os.path.dirname(request.fspath))
    infile = os.path.join(path, "vpl.in")
    output = vplanet.run(infile, quiet=True, clobber=True)
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
    return output

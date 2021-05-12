import vplanet
import numpy as np
import os
import pytest


@pytest.fixture(scope="module", autouse=True)
def output(request):
    infile = os.path.join(os.path.abspath(os.path.dirname(request.fspath)), "vpl.in")
    yield vplanet.run(infile, quiet=True, clobber=True)

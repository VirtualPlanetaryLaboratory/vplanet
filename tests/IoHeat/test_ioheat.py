from vplot import GetOutput
import subprocess
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))


def test_stellar():
    """Test the stellar module."""
    # Run vplanet
    subprocess.run(['vplanet', 'vpl.in', '-q'], cwd=cwd)

    # Grab the output
    output = GetOutput(path=cwd)
(SurfEnFluxEqtide) Surface Energy Flux due to Tides in EQTIDE [kg*m^2/sec^2/(m^2*sec)]: 2.243506

    # Check
    assert np.isclose(output.log.initial.io.SurfEnFluxEqtide, 2.243506)

if __name__ == "__main__":
    test_io_heat()

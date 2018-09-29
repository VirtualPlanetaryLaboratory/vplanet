from vplot import GetOutput
import subprocess
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))


def test_VenusWaterLoss():
    """Test the loss of water on Venus."""
    # Run vplanet
    subprocess.run(['vplanet', 'vpl.in', '-q'], cwd=cwd)

    # Grab the output
    output = GetOutput(path=cwd)

    # Check
    assert np.isclose(output.log.final.sun.LXUVStellar, 0.000427)
    assert np.isclose(output.log.final.venus.SurfWaterMass, 0.916121, rtol=1e-4)

if __name__ == "__main__":
    test_VenusWaterLoss()

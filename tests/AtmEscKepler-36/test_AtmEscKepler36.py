from vplot import GetOutput
import subprocess
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))


def test_AtmEscKepler36():
    """Test hydrogen envelope loss."""
    # Remove old log file
    subprocess.run(['rm', 'AtmEscKepler-36.log'], cwd=cwd)
    # Run vplanet
    subprocess.run(['vplanet', 'vpl.in', '-q'], cwd=cwd)

    # Grab the output
    output = GetOutput(path=cwd)

    # Checks
    # Primary Variables
    assert np.isclose(output.log.final.p10.EnvelopeMass, 3.314569)
    assert np.isclose(output.log.final.p10.Radius, 3.376736e+07)

if __name__ == "__main__":
    test_AtmEscKepler36()

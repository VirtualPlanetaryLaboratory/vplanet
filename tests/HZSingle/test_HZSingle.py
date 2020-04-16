from vplot import GetOutput
import subprocess
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))


def test_HZSingle():
    """Test the HZ limits for a single star."""
    # Remove old log file
    subprocess.run(['rm', 'star.log'], cwd=cwd)
    # Run vplanet
    subprocess.run(['vplanet', 'vpl.in', '-q'], cwd=cwd)

    # Grab the output
    output = GetOutput(path=cwd)

    # Check
    assert np.isclose(output.log.initial.a.HZLimRecVenus, 2.879712e+10)
    assert np.isclose(output.log.initial.a.HZLimEarlyMars, 7.969376e+10)
    assert np.isclose(output.log.initial.a.HZLimRunaway, 3.790332e+10)
    assert np.isclose(output.log.initial.a.HZLimMoistGreenhouse, 3.810337e+10)
    assert np.isclose(output.log.initial.a.HZLimMaxGreenhouse, 7.307316e+10)


if __name__ == "__main__":
    test_HZSingle()

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
    assert np.isclose(output.log.final.a.HZLimRecVenus, 6.038467e+09)
    assert np.isclose(output.log.final.a.HZLimEarlyMars, 1.669770e+10)
    assert np.isclose(output.log.final.a.HZLimRunaway, 7.949225e+09)
    assert np.isclose(output.log.final.a.HZLimMoistGreenhouse, 7.989892e+09)
    assert np.isclose(output.log.final.a.HZLimMaxGreenhouse, 1.531054e+10)


if __name__ == "__main__":
    test_HZSingle()

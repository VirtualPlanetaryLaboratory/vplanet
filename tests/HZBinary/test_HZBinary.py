from vplot import GetOutput
import subprocess
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))


def test_HZBinary():
    """Test the HZ limits for a two stars."""
    # Remove old log file
    subprocess.run(['rm', 'HZBinary.log'], cwd=cwd)
    # Run vplanet
    subprocess.run(['vplanet', 'vpl.in', '-q'], cwd=cwd)

    # Grab the output
    output = GetOutput(path=cwd)

    # Check
    assert np.isclose(output.log.final.secondary.HZLimRecVenus, 9.828450e+10)
    assert np.isclose(output.log.final.secondary.HZLimEarlyMars, 2.673083e+11)
    assert np.isclose(output.log.final.secondary.HZLimRunaway, 1.298768e+11)
    assert np.isclose(output.log.final.secondary.HZLimMoistGreenhouse, 1.300464e+11)
    assert np.isclose(output.log.final.secondary.HZLimMaxGreenhouse, 2.450954e+11)


if __name__ == "__main__":
    test_HZBinary()

from vplot import GetOutput
import subprocess
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))


def test_Kepler16():
    """Test module Binary."""
    # Remove old log file
    subprocess.run(['rm', 'kepelr16.log'], cwd=cwd)
    # Run vplanet
    subprocess.run(['vplanet', 'vpl.in', '-q'], cwd=cwd)

    # Grab the output
    output = GetOutput(path=cwd)

    # Run our comparisons
    assert np.isclose(output.log.final.cbp.FreeEcc, 0.03)
    assert np.isclose(output.log.final.cbp.Eccentricity, 0.065653)
    assert np.isclose(output.log.final.cbp.SemiMajorAxis, 1.046162e+11)
    assert np.isclose(output.log.final.secondary.Eccentricity, 0.16048)
    assert np.isclose(output.log.final.secondary.SemiMajorAxis, 3.351740e+10)


if __name__ == "__main__":
    test_Kepler16()

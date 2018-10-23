from vplot import GetOutput
import subprocess
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))


def test_EarthMoonTides():
    """Test module Eqtide and backward integrations."""
    # Remove old log file
    subprocess.run(['rm', 'earthmoon.log'], cwd=cwd)
    # Run vplanet
    subprocess.run(['vplanet', 'vpl.in', '-q'], cwd=cwd)

    # Grab the output
    output = GetOutput(path=cwd)

    # Run our comparisons
    assert np.isclose(output.log.final.moon.Eccentricity, 0.0001839441662174)
    assert np.isclose(output.log.final.moon.SemiMajorAxis, 8.0848150930770254e+06)
    assert np.isclose(output.log.final.earth.RotPer, 204.2777256761116860)
    assert np.isclose(output.log.final.earth.Obliquity, 0.0552580400961436)

if __name__ == "__main__":
    test_EarthMoonTides()

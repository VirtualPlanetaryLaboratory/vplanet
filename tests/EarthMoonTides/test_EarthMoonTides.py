from vplot import GetOutput
import subprocess
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))


def test_EarthMoonTides():
    """Test module Eqtide and backward integrations."""
    # Run vplanet
    subprocess.run(['vplanet', 'vpl.in', '-q'], cwd=cwd)

    # Grab the output
    output = GetOutput(path=cwd)

    # Run our comparisons
    assert np.isclose(output.log.final.moon.Eccentricity, 0.0077708405172700)
    assert np.isclose(output.log.final.moon.SemiMajorAxis, -8.9493380478454471e+07)
    assert np.isclose(output.log.final.earth.RotPer, 158.5020751480991521)
    assert np.isclose(output.log.final.earth.Obliquity, 0.0483032126773969)

if __name__ == "__main__":
    test_EarthMoonTides()

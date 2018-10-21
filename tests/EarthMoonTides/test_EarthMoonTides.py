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
    assert np.isclose(output.log.final.moon.Eccentricity, 0.0136505935724777)
    assert np.isclose(output.log.final.moon.SemiMajorAxis, -60276727.74464643)
    assert np.isclose(output.log.final.earth.RotPer, 222.74591695807214)
    assert np.isclose(output.log.final.earth.Obliquity, 0.0268799502888956)

if __name__ == "__main__":
    test_EarthMoonTides()

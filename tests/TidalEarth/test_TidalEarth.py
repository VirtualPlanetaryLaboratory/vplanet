from vplot import GetOutput
import subprocess
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))


def test_TidalEarth():
    """Test the coupling between radheat, thermint, and eqtide."""
    # Remove old log file
    subprocess.run(['rm', 'tidalearth.log'], cwd=cwd)
    # Run vplanet
    subprocess.run(['vplanet', 'vpl.in', '-q'], cwd=cwd)

    # Grab the output
    output = GetOutput(path=cwd)

    # Check
    assert np.isclose(output.log.final.tidalearth.TMan, 2799.517833)
    assert np.isclose(output.log.final.tidalearth.TCore, 5500.224551)
<<<<<<< HEAD
    assert np.isclose(output.log.final.tidalearth.PowerEqtide, 0.005250)
    assert np.isclose(output.log.final.tidalearth.Eccentricity, 0.498425)
    assert np.isclose(output.log.final.tidalearth.SemiMajorAxis, 7.468143e+09)
=======
    assert np.isclose(output.log.final.tidalearth.PowerEqtide, 0.005337)
    assert np.isclose(output.log.final.tidalearth.Eccentricity, 0.490441)
    assert np.isclose(output.log.final.tidalearth.SemiMajorAxis, 7.409421e+09)
>>>>>>> a45095dc8e0b53eba999c90f3556ce5713085cea


if __name__ == "__main__":
    test_TidalEarth()

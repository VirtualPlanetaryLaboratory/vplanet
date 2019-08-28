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
    assert np.isclose(output.log.final.tidalearth.PowerEqtide, 0.005337)
    assert np.isclose(output.log.final.tidalearth.Eccentricity, 0.490441)
    assert np.isclose(output.log.final.tidalearth.SemiMajorAxis, 7.409421e+09)


if __name__ == "__main__":
    test_TidalEarth()

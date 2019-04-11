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
    assert np.isclose(output.log.final.tidalearth.TMan, 2145.45793)
    assert np.isclose(output.log.final.tidalearth.TCore, 4857.85422)
    assert np.isclose(output.log.final.tidalearth.PowerEqtide, 0.001816)
    assert np.isclose(output.log.final.tidalearth.Eccentricity, 7.256839e-05)
    assert np.isclose(output.log.final.tidalearth.SemiMajorAxis, 5825256000.0)


if __name__ == "__main__":
    test_TidalEarth()

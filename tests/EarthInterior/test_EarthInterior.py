from vplot import GetOutput
import subprocess
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))


def test_EarthInterior():
    """Test the coupling between radheat and thermint."""
    # Remove old log file
    subprocess.run(['rm', 'earth.log'], cwd=cwd)
    # Run vplanet
    subprocess.run(['vplanet', 'vpl.in', '-q'], cwd=cwd)

    # Grab the output
    output = GetOutput(path=cwd)

    # Check
    assert np.isclose(output.earth.TMan[-1], 2257.8509)
    assert np.isclose(output.earth.TCore[-1], 4999.1318)
    assert np.isclose(output.earth.RIC[-1], 1224.7839)
    assert np.isclose(output.earth.RadPowerTotal[-1], 24.3829)
    assert np.isclose(output.earth.MagMom[-1], 1.009593)


if __name__ == "__main__":
    test_EarthInterior()

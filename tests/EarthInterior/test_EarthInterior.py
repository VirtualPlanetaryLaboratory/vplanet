from vplot import GetOutput
import subprocess
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))


def test_EarthInterior():
    """Test the coupling between radheat and thermint."""
    # Run vplanet
    subprocess.run(['vplanet', 'vpl.in', '-q'], cwd=cwd)

    # Grab the output
    output = GetOutput(path=cwd)

    # Check
    assert np.isclose(output.log.final.earth.TMan, 2256.06831)
    assert np.isclose(output.log.final.earth.TCore, 5006.731957)
    assert np.isclose(output.log.final.earth.RIC, 1230030.0)
    assert np.isclose(output.log.final.earth.RadPowerTotal, 2.426301e+13)
    assert np.isclose(output.log.final.earth.MagMom, 0.995468)


if __name__ == "__main__":
    test_EarthInterior()

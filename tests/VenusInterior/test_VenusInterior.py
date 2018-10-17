from vplot import GetOutput
import subprocess
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))


def test_VenusInterior():
    """Test the coupling between radheat and thermint under stagnant lid conditions."""
    # Run vplanet
    subprocess.run(['vplanet', 'vpl.in', '-q'], cwd=cwd)

    # Grab the output
    output = GetOutput(path=cwd)

    # Check
    assert np.isclose(output.log.final.venus.TMan, 2668.703011)
    assert np.isclose(output.log.final.venus.TCore, 6404.180534)
    assert np.isclose(output.log.final.venus.RIC, 0.0)
    assert np.isclose(output.log.final.venus.RadPowerTotal, 3.141604e+13)
    assert np.isclose(output.log.final.venus.BLUMan, 5.573595)


if __name__ == "__main__":
    test_VenusInterior()

from vplot import GetOutput
import subprocess
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))


def test_VenusInterior():
    """Test the coupling between radheat and thermint under stagnant lid conditions."""
    # Remove old log file
    subprocess.run(['rm', 'venus.log'], cwd=cwd)
    # Run vplanet
    subprocess.run(['vplanet', 'vpl.in', '-q'], cwd=cwd)

    # Grab the output
    output = GetOutput(path=cwd)

    # Check
    assert np.isclose(output.log.final.venus.TMan, 2679.27122)
    assert np.isclose(output.log.final.venus.TCore, 6365.71258)
    assert np.isclose(output.log.final.venus.RIC, 0.0)
    assert np.isclose(output.log.final.venus.RadPowerTotal, 31.49126)
    assert np.isclose(output.log.final.venus.MagMom, 0.0)


if __name__ == "__main__":
    test_VenusInterior()

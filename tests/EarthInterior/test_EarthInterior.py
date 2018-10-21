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
    assert np.isclose(output.log.final.earth.TMan, 2256.001803)
    assert np.isclose(output.log.final.earth.TCore, 5006.543944)
    assert np.isclose(output.log.final.earth.RIC, 1.232183e+06)
    assert np.isclose(output.log.final.earth.RadPowerTotal, 2.426567e+13)
    assert np.isclose(output.log.final.earth.MagMom, 0.995060)


if __name__ == "__main__":
    test_EarthInterior()

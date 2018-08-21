from vplot import GetOutput
import subprocess
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))


def test_radheat_thermint():
    """Test the coupling between radheat and thermint."""
    # Run vplanet
    subprocess.run(['vplanet', 'vpl.in', '-q'], cwd=cwd)

    # Grab the output
    output = GetOutput(path=cwd)

    # Check
    assert np.isclose(output.log.final.earth.TMan, 2350.066809)
    assert np.isclose(output.log.final.earth.TCore, 4998.497405)
    assert np.isclose(output.log.final.earth.RadPowerTotal, 22.955414)
    assert np.isclose(output.log.final.earth.MagMom, 0.962911)


if __name__ == "__main__":
    test_radheat_thermint()

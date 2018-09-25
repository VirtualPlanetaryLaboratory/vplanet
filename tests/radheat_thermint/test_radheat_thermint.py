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
    assert np.isclose(output.log.final.earth.TMan, 2256.1804,atol=200)
    assert np.isclose(output.log.final.earth.TCore, 5009.8424,atol=200) # !!!
    assert np.isclose(output.log.final.earth.RIC, 1221e3,atol=30e3) # !!!
    assert np.isclose(output.log.final.earth.RadPowerTotal, 24.2630e12,atol=4e12)
    assert np.isclose(output.log.final.earth.MagMom, 0.994634,atol=0.2)


if __name__ == "__main__":
    test_radheat_thermint()

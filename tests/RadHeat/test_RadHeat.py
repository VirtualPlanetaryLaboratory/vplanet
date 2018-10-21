from vplot import GetOutput
import subprocess
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))


def test_RadHeat():
    """Test RadHeat."""
    # Run vplanet
    subprocess.run(['vplanet', 'vpl.in', '-q'], cwd=cwd)

    # Grab the output
    output = GetOutput(path=cwd)

    # Check
    assert np.isclose(output.log.final.earth._40KPowerCrust, 1.168067)
    assert np.isclose(output.log.final.earth._232ThMassCore, 4.182962e+15)
    assert np.isclose(output.log.final.earth._235UPowerMan,  0.240654)
    assert np.isclose(output.log.final.earth._232ThNumCrust, 2.859381e+41)
    assert np.isclose(output.log.final.earth.SurfEnFluxRadTotal, 0.047491)

if __name__ == "__main__":
    test_RadHeat()

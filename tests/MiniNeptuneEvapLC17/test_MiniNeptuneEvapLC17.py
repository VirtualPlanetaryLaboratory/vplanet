from vplot import GetOutput
import subprocess
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))


def test_LehmerCatling17():
    """Test hydrogen envelope loss with the Lehmer & Catling (2017) model."""
    # Remove old log file
    subprocess.run(['rm', 'lc17.log'], cwd=cwd)
    # Run vplanet
    subprocess.run(['vplanet', 'vpl.in', '-q'], cwd=cwd)

    # Grab the output
    output = GetOutput(path=cwd)

    # Check Primary Variables
    assert np.isclose(output.log.final.planet.EnvelopeMass, 0.029770)
    assert np.isclose(output.log.final.planet.DEnvMassDt, -4.927935e+08)

    # Check other variables
    assert np.isclose(output.log.final.planet.Radius, 2.821863e+07)
    assert np.isclose(output.log.final.planet.Mass, 1.969770)
    assert np.isclose(output.log.final.planet.ScaleHeight, 273.737851)
    assert np.isclose(output.log.final.planet.PresSurf, 3.267509)

if __name__ == "__main__":
    test_LehmerCatling17()

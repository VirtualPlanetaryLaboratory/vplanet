from vplot import GetOutput
import subprocess
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))


def test_MiniNeptuneEvapDyn():
    """Test hydrogen envelope loss with the Lehmer & Catling (2017) model."""
    # Remove old log file
    subprocess.run(['rm', 'dynamic.log'], cwd=cwd)
    # Run vplanet
    subprocess.run(['vplanet', 'vpl.in', '-q'], cwd=cwd)

    # Grab the output
    output = GetOutput(path=cwd)

    # Check Primary Variables
    assert np.isclose(output.log.final.planet.EnvelopeMass, 0.032361)
    assert np.isclose(output.log.final.planet.DEnvMassDt, -4.568102e+09)

    # Check other variables
    assert np.isclose(output.log.final.planet.RadXUV, 29.425994)
    assert np.isclose(output.log.final.planet.RocheRadius, 29.425994)
    assert np.isclose(output.log.final.planet.ScaleHeight, 401.210931)
    assert np.isclose(output.log.final.planet.PresSurf, 3.551891)

if __name__ == "__main__":
    test_MiniNeptuneEvapDyn()

from vplot import GetOutput
import subprocess
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))


def test_eqtide():
    """Test the tidal evolution module."""
    # Run vplanet
    subprocess.run(['vplanet', 'vpl.in', '-q'], cwd=cwd)

    # Grab the output
    output = GetOutput(path=cwd)

    # Check
    assert np.isclose(output.log.final.star.RotPer, 94.197975)
    assert np.isclose(output.log.final.d.RotPer, 43.28407)
    assert np.isclose(output.log.final.d.Obliquity, 3.337686e-07)
    assert np.isclose(output.log.final.d.Eccentricity, 0.380001)
    assert np.isclose(output.log.final.d.SemiMajorAxis, 32682710000.0)


if __name__ == "__main__":
    test_eqtide()

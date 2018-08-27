from vplot import GetOutput
import subprocess
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))


def test_stellar_atmesc():
    """Test the coupling between stellar and atmesc."""
    # Run vplanet
    subprocess.run(['vplanet', 'vpl.in', '-q'], cwd=cwd)

    # Grab the output
    output = GetOutput(path=cwd)

    # Check
    assert np.isclose(output.log.final.star.Luminosity, 0.0008687887)
    assert np.isclose(output.log.final.star.LXUVFrac, 5.8884365536e-05)
    assert np.isclose(output.log.final.a.EnvelopeMass, 5.9695893469e+23)


if __name__ == "__main__":
    test_stellar_atmesc()

from vplot import GetOutput
import subprocess
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))


def test_binary_eqtide_stellar():
    """Test the coupling between binary, eqtide, and stellar."""
    # Run vplanetcle
    subprocess.run(['vplanet', 'vpl.in', '-q'], cwd=cwd)

    # Grab the output
    output = GetOutput(path=cwd)

    # Run our comparisons
    assert np.isclose(output.log.final.cbp.FreeEcc, 0.03)
    assert np.isclose(output.log.final.cbp.Eccentricity, 0.049913)
    assert np.isclose(output.log.final.cbp.SemiMajorAxis, 106419600000.0)
    assert np.isclose(output.log.final.secondary.Eccentricity, 0.16048)
    assert np.isclose(output.log.final.secondary.SemiMajorAxis, 33554800000.0)


if __name__ == "__main__":
    test_binary_eqtide_stellar()

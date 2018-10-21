from vplot import GetOutput
import subprocess
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))


def test_BinaryTides():
    """Test tidal/stellar evolution of tight binary star systems."""
    # Remove old log file
    subprocess.run(['rm', 'Zahn89.log'], cwd=cwd)
    # Run vplanet
    subprocess.run(['vplanet', 'vpl.in', '-q'], cwd=cwd)

    # Grab the output
    output = GetOutput(path=cwd)

    # Run our comparisons
    assert np.isclose(output.log.final.primary.RadGyra, 0.451302)
    assert np.isclose(output.log.final.primary.RotPer, 3.577599, rtol=1e-4)
    assert np.isclose(output.log.final.secondary.RotPer, 3.577599, rtol=1e-4)
    assert np.isclose(output.log.final.secondary.RadGyra, 0.451302)
    assert np.isclose(output.log.final.secondary.Eccentricity, 0.045513, rtol=1e-4)
    assert np.isclose(output.log.final.secondary.SemiMajorAxis, 0.057677, rtol=1e-4)

if __name__ == "__main__":
    test_BinaryTides()

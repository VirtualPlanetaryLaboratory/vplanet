from vplot import GetOutput
import subprocess
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))


def test_GalHabit():
    """Test GalHabit."""
    # Remove old log file
    subprocess.run(['rm', 'kaib.log'], cwd=cwd)
    # Run vplanet
    subprocess.run(['vplanet', 'vpl.in', '-q'], cwd=cwd)

    # Grab the output
    output = GetOutput(path=cwd)

    # Check
    assert np.isclose(output.log.final.comp.SemiMajorAxis, 1.4959787070e+15)
    assert np.isclose(output.log.final.comp.Eccentricity, 0.7052434592)
    assert np.isclose(output.log.final.comp.PeriQ, 4.4094950872e+14)
    assert np.isclose(output.log.final.comp.Inc, 1.3949755231)

if __name__ == "__main__":
    test_GalHabit()

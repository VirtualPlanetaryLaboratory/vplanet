from vplot import GetOutput
import subprocess
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))


def test_GalHabit():
    """Test GalHabit."""
    # Run vplanet
    subprocess.run(['vplanet', 'vpl.in', '-q'], cwd=cwd)

    # Grab the output
    output = GetOutput(path=cwd)

    # Check
    assert np.isclose(output.log.final.comp.SemiMajorAxis, 1.5197682790e+15)
    assert np.isclose(output.log.final.comp.Eccentricity, 0.7369917772)
    assert np.isclose(output.log.final.comp.PeriQ, 3.9971155411e+14)
    assert np.isclose(output.log.final.comp.Inc, 1.3483166684)

if __name__ == "__main__":
    test_GalHabit()

from vplot import GetOutput
import subprocess
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))


def test_distorb_distrot():
    """Test the coupling between distorb and distrot."""
    # Run vplanet
    subprocess.run(['vplanet', 'vpl.in', '-q'], cwd=cwd)

    # Grab the output
    output = GetOutput(path=cwd)

    # Run our comparisons
    assert np.isclose(output.log.final.Mercury.ArgP, 0.772265)
    assert np.isclose(output.log.final.Venus.Eccentricity, 0.006311)
    assert np.isclose(output.log.final.Earth.Obliquity, 0.406847)
    assert np.isclose(output.log.final.Mars.PrecA, 0.340391)
    assert np.isclose(output.log.final.Jupiter.Eccentricity, 0.049628)
    assert np.isclose(output.log.final.Saturn.Inc, 0.016865)
    assert np.isclose(output.log.final.George.ArgP, 3.823707)
    assert np.isclose(output.log.final.Neptune.LongA, 3.056958)


if __name__ == "__main__":
    test_distorb_distrot()

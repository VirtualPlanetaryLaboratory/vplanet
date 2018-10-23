from vplot import GetOutput
import subprocess
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))


def test_SSDistOrbDistRot():
    """Test the coupling between distorb and distrot."""
    # Remove old log file
    subprocess.run(['rm', 'solarsys.log'], cwd=cwd)
    # Run vplanet
    subprocess.run(['vplanet', 'vpl.in', '-q'], cwd=cwd)

    # Grab the output
    output = GetOutput(path=cwd)

    # Run our comparisons
    assert np.isclose(output.log.final.Mercury.ArgP, 1.016312)
    assert np.isclose(output.log.final.Venus.Eccentricity, 0.003504)
    assert np.isclose(output.log.final.Earth.Obliquity, 0.394576)
    assert np.isclose(output.log.final.Mars.PrecA, 0.420712)
    assert np.isclose(output.log.final.Jupiter.Eccentricity, 0.058517)
    assert np.isclose(output.log.final.Saturn.Inc, 0.039880)
    assert np.isclose(output.log.final.George.ArgP, 1.631639)
    assert np.isclose(output.log.final.Neptune.LongA, 2.287481)


if __name__ == "__main__":
    test_SSDistOrbDistRot()

from vplot import GetOutput
import subprocess
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))


def test_CassiniStates():
    """Test the coupling between eqtide, distorb and distrot."""
    # Run vplanet
    subprocess.run(['vplanet', 'vpl.in', '-q'], cwd=cwd)

    # Grab the output
    output = GetOutput(path=cwd)

    # Check
    assert np.isclose(output.log.final.b. Inc, 0.008680)
    assert np.isclose(output.log.final.b.ArgP, 4.733145)
    assert np.isclose(output.log.final.b.LongA, 0.148731)
    assert np.isclose(output.log.final.b.Obliquity, 0.786733)
    assert np.isclose(output.log.final.b.PrecA, 3.068821)
    assert np.isclose(output.log.final.c.Inc, 8.069275e-05)
    assert np.isclose(output.log.final.c.ArgP, 4.653590)
    assert np.isclose(output.log.final.c.LongA, 1.580510)


if __name__ == "__main__":
    test_CassiniStates()

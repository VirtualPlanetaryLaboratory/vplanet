from vplot import GetOutput
import subprocess
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))


def test_Corot7():
    """Test module Eqtide-Distorb coupling."""
    # Run vplanet
    subprocess.run(['vplanet', 'vpl.in', '-q'], cwd=cwd)

    # Grab the output
    output = GetOutput(path=cwd)

    # Run our comparisons
    assert np.isclose(output.log.final.b.Eccentricity, 0.081326)
    assert np.isclose(output.log.final.b.ArgP, 3.693953)
    assert np.isclose(output.log.final.b.SemiMajorAxis, 2.812439e+09)
    assert np.isclose(output.log.final.c.Eccentricity, 0.193811)
    assert np.isclose(output.log.final.c.ArgP, 5.015526)
    assert np.isclose(output.log.final.c.SemiMajorAxis, 6.911428e+09)


if __name__ == "__main__":
    test_Corot7()

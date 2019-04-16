from vplot import GetOutput
import subprocess
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))


def test_SteepCBP():
    """Test STEEP effect on a circumbinary planet (Fleming et al., 2018, ApJ, 858, 86)."""
    # Remove old log file
    subprocess.run(['rm', 'bin_eq_st.log'], cwd=cwd)
    # Run vplanet
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
    test_STEP_CBP()

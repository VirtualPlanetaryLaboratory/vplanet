from vplot import GetOutput
import subprocess
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))


def test_BondiLimited():
    """Test Bondi-limited atmospheric escape"""
    # Remove old log file
    subprocess.run(['rm', 'BondiLimited.log'], cwd=cwd)
    # Run vplanet
    subprocess.run(['vplanet', 'vpl.in', '-q'], cwd=cwd)

    # Grab the output
    output = GetOutput(path=cwd)

    # Make sure the Bondi-limited escape regime has been selected
    assert np.isclose(output.log.initial.bondi.HEscapeRegime, 5.0)

    # Ensure Bondi-limited mass loss rate is correct by comparing it to
    # Eqn. 4 from Owen & Wu (2016), ~1e-2 Earth masses / yr in kg/s
    assert np.isclose(output.bondi.DEnvMassDt[0], -1.753603e+15)

    # Ensure final Roche Lobe and Bondi Radius are correct [Earth radii]
    assert np.isclose(output.log.final.bondi.BondiRadius, 35.824267)
    assert np.isclose(output.log.final.bondi.RocheRadius, 44.88505)

    # Ensure final H envelope and planetary mass are correct
    # That is, the H envelope should have been entirely lost!
    assert np.isclose(output.log.final.bondi.EnvelopeMass, 0.0)
    assert np.isclose(output.log.final.bondi.Mass, 7.0)


if __name__ == "__main__":
    test_BondiLimited()

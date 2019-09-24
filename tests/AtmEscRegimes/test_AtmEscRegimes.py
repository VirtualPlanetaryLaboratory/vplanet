from vplot import GetOutput
import subprocess
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))


def test_AtmEscRegimes():
    """Test Bondi-limited, RR-limited, and auto atmospheric escape"""
    # Remove old log file
    subprocess.run(['rm', 'AtmEscRegimes.log'], cwd=cwd)
    # Run vplanet
    subprocess.run(['vplanet', 'vpl.in', '-q'], cwd=cwd)

    # Grab the output
    output = GetOutput(path=cwd)

    # Check the Bondi-limited escape

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

    # Test RR-limited formalism

    # Make sure RR-limited escape is selected
    assert np.isclose(output.log.initial.rr.HEscapeRegime, 6.0)

    # Check mass loss rate [kg/s] - needs to agree w/ Eqn. 13 from Luger+2015
    assert np.isclose(output.rr.DEnvMassDt[0], -7.111259e+04)

    # Check critical flux [W/m^2] - needs to agree w/ Eqn. A23 from Luger+2015
    # Note - system isn't actual in RR escape - we set bUseRRLimited to force
    # RR-limited escape since dFXUV < the critical flux
    assert(np.isclose(output.log.initial.rr.RRCriticalFlux, 53.688843))


if __name__ == "__main__":
    test_AtmEscRegimes()

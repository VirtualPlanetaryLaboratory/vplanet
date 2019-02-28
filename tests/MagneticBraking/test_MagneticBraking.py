from vplot import GetOutput
import subprocess
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))


def test_MagneticBraking():
    """Test the stellar module."""
    # Remove old log file
    subprocess.run(['rm', 'system.log'], cwd=cwd)
    # Run vplanet
    subprocess.run(['vplanet', 'vpl.in', '-q'], cwd=cwd)

    # Grab the output
    output = GetOutput(path=cwd)

    # Check

    # Confirm
    assert np.isclose(output.log.final.matt.RotPer, 0.252432)
    assert np.isclose(output.log.final.reiners.RotPer, 1.059182)
    assert np.isclose(output.log.final.sk.RotPer, 1.486450)

    # Energy
    Einit = output.log.initial.system.TotEnergy
    Efinal = output.log.initial.system.TotEnergy

    # Angular momentum
    Linit = output.log.initial.system.TotAngMom
    Lfinal = output.log.final.system.TotAngMom

    # Check conservation
    assert np.isclose(Einit, Efinal, rtol=1e-3)
    assert np.isclose(Linit, Lfinal, rtol=1e-3)

if __name__ == "__main__":
    test_MagneticBraking()

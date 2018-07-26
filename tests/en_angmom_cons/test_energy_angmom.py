from vplot import GetOutput
import subprocess
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))


def test_energy_angmom():
    """Test energy and angular momentum conservation."""
    # Run vplanet
    subprocess.run(['vplanet', 'vpl.in', '-q'], cwd=cwd)

    # Grab the output
    output = GetOutput(path=cwd)

    # Energy
    Einit = output.log.initial.system.TotEnergy
    Efinal = output.log.final.system.TotEnergy
    logEinit = np.log10(np.abs(Einit))
    logEfinal = np.log10(np.abs(Efinal))

    # Angular momentum
    Linit = output.log.initial.system.TotAngMom
    Lfinal = output.log.final.system.TotAngMom
    logLinit = np.log10(np.abs(Linit))
    logLfinal = np.log10(np.abs(Lfinal))

    # Check conservation
    # NOTE: Our tolerance is pretty lax here...
    assert np.isclose(logEinit, logEfinal, rtol=0.1)
    assert np.isclose(logLinit, logLfinal, rtol=0.1)


if __name__ == "__main__":
    test_energy_angmom()

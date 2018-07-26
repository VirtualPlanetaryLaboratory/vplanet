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
    Einit = output.star.TotEnergy[1]
    Efinal = output.star.TotEnergy[-1]

    # Angular momentum
    Linit = output.star.TotAngMom[1]
    Lfinal = output.star.TotAngMom[-1]

    # Check conservation
    assert np.isclose(Einit, Efinal, rtol=1e-4)
    assert np.isclose(Linit, Lfinal, rtol=1e-4)


if __name__ == "__main__":
    test_energy_angmom()

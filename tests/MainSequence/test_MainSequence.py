from vplot import GetOutput
import subprocess
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))


def test_MainSequence():
    """Test the stellar module."""
    # Run vplanet
    subprocess.run(['vplanet', 'vpl.in', '-q'], cwd=cwd)

    # Grab the output
    output = GetOutput(path=cwd)

    # Check

    # Star a
    assert np.isclose(output.log.final.a.Luminosity, 0.002445)
    assert np.isclose(output.log.final.a.Temperature, 2992.330343)
    assert np.isclose(output.log.final.a.Radius, 0.184366)
    assert np.isclose(output.log.final.a.LostAngMom, 9.438825e+41, rtol=1.0e-4)
    assert np.isclose(output.log.final.a.RotPer, 0.299133, rtol=1.0e-4)

    # Star b
    assert np.isclose(output.log.final.b.Luminosity, 0.689088, rtol=1.0e-4)
    assert np.isclose(output.log.final.b.Temperature, 5539.190016)
    assert np.isclose(output.log.final.b.Radius, 0.902638, rtol=1.0e-4)
    assert np.isclose(output.log.final.b.LostAngMom, 5.085396e+43, rtol=1.0e-4)
    assert np.isclose(output.log.final.b.RotPer, 3.075721, rtol=1.0e-4)

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
    test_MainSequence()

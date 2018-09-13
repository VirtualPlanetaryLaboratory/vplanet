from vplot import GetOutput
import subprocess
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))


def test_stellar():
    """Test the stellar module."""
    # Run vplanet
    subprocess.run(['vplanet', 'vpl.in', '-q'], cwd=cwd)

    # Grab the output
    output = GetOutput(path=cwd)

    # Check

    # Star a
    assert np.isclose(output.log.final.a.Luminosity, 0.000869)
    assert np.isclose(output.log.final.a.Temperature, 2811.000999)
    assert np.isclose(output.log.final.a.Radius, 0.124)
    assert np.isclose(output.log.final.a.LostAngMom, 1.231953e+40, rtol=1.0e-4)
    assert np.isclose(output.log.final.a.RotPer, 2.130347, rtol=1.0e-4)

    # Star b
    assert np.isclose(output.log.final.b.Luminosity, 1.021926, rtol=1.0e-4)
    assert np.isclose(output.log.final.b.Temperature, 5730.814496)
    assert np.isclose(output.log.final.b.Radius, 1.026419, rtol=1.0e-4)
    assert np.isclose(output.log.final.b.LostAngMom, 4.94742e+42, rtol=1.0e-4)
    assert np.isclose(output.log.final.b.RotPer, 32.262612, rtol=1.0e-4)

    # Energy
    Einit = output.a.TotEnergy[0]
    Efinal = output.a.TotEnergy[-1]

    # Angular momentum
    Linit = output.a.TotAngMom[0]
    Lfinal = output.a.TotAngMom[-1]

    # Check conservation
    assert np.isclose(Einit, Efinal, rtol=1e-4)
    assert np.isclose(Linit, Lfinal, rtol=1e-4)

if __name__ == "__main__":
    test_stellar()

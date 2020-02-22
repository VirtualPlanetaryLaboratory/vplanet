from vplot import GetOutput
import subprocess
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))


def test_Lopez12CPL():
    """Test the loss of hydrogen on planet tidally evolving via the CPL model."""
    # Remove old log file
    subprocess.run(['rm', 'Lopez12CPL.log'], cwd=cwd)
    # Run vplanet
    subprocess.run(['vplanet', 'vpl.in', '-q'], cwd=cwd)

    # Grab the output
    output = GetOutput(path=cwd)

    # Check
    assert np.isclose(output.log.final.auto.OrbPeriod, 9.650538e+05)
    assert np.isclose(output.log.final.auto.RotRate, 6.510710e-06)
    assert np.isclose(output.log.final.auto.Eccentricity, 0.129902)
    assert np.isclose(output.log.final.auto.EnvelopeMass,  0.411359)
    assert np.isclose(output.log.final.auto.PlanetRadius, 25.560564)
    assert np.isclose(output.log.final.auto.DEnvMassDt, -7.802590e+07)

    assert np.isclose(output.log.final.bondi.SemiMajorAxis, 0.100004)
    assert np.isclose(output.log.final.bondi.RotVel, 40.154732)
    assert np.isclose(output.log.final.bondi.KEcc, 0.200007)
    assert np.isclose(output.log.final.bondi.EnvelopeMass,  0.0)
    assert np.isclose(output.log.final.bondi.PlanetRadius, 1.000000)
    assert np.isclose(output.log.final.bondi.DEnvMassDt, 0.0)

    assert np.isclose(output.log.final.el.SemiMajorAxis, 0.096591)
    assert np.isclose(output.log.final.el.RotAngMom, 5.470333e+35)
    assert np.isclose(output.log.final.el.Eccentricity, 0.068456)
    assert np.isclose(output.log.final.el.EnvelopeMass, 0.986370)
    assert np.isclose(output.log.final.el.PlanetRadius, 32.683276)
    assert np.isclose(output.log.final.el.BondiRadius, 7.982897e+08)

    assert np.isclose(output.log.final.rr.MeanMotion, 6.626773e-06)
    assert np.isclose(output.log.final.rr.RotKinEnergy, 1.842803e+30)
    assert np.isclose(output.log.final.rr.Eccentricity, 0.068275)
    assert np.isclose(output.log.final.rr.EnvelopeMass, 0.999399)
    assert np.isclose(output.log.final.rr.PlanetRadius, 32.861293)
    assert np.isclose(output.log.final.rr.RocheRadius, 1.822097e+08)

if __name__ == "__main__":
    test_Lopez12CPL()

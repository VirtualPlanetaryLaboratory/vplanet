from vplot import GetOutput
import subprocess
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))


def test_HabEvapCore():
    """Test hydrogen envelope loss followed by water loss."""
    # Remove old log file
    subprocess.run(['rm', 'proxima.log'], cwd=cwd)
    # Run vplanet
    subprocess.run(['vplanet', 'vpl.in', '-q'], cwd=cwd)

    # Grab the output
    output = GetOutput(path=cwd)

    # Run our comparisons
    assert np.isclose(output.log.final.star.LXUVStellar, 4.988314e+21)

    assert np.isclose(output.log.final.ProxCenELim.EnvelopeMass, 0.000526)
    assert np.isclose(output.log.final.ProxCenELim.PlanetRadius, 1.335673)
    assert np.isclose(output.log.final.ProxCenELim.SurfWaterMass, 1.500000)

    assert np.isclose(output.log.final.ProxCenRRLim.EnvelopeMass, 0.000300)
    assert np.isclose(output.log.final.ProxCenRRLim.PlanetRadius, 1.339401)
    assert np.isclose(output.log.final.ProxCenRRLim.SurfWaterMass, 1.500000)

    assert np.isclose(output.log.final.ProxCenAuto.EnvelopeMass, 0.000526)
    assert np.isclose(output.log.final.ProxCenAuto.PlanetRadius, 1.335673)
    assert np.isclose(output.log.final.ProxCenAuto.SurfWaterMass, 1.500000)

    assert np.isclose(output.log.final.LC17ELim.EnvelopeMass, 0.000000)
    assert np.isclose(output.log.final.LC17ELim.PlanetRadius, 1.063513)
    assert np.isclose(output.log.final.LC17ELim.SurfWaterMass, 1.304141)

    assert np.isclose(output.log.final.LC17RRLim.EnvelopeMass, 0.000000)
    assert np.isclose(output.log.final.LC17RRLim.PlanetRadius, 1.063513)
    assert np.isclose(output.log.final.LC17RRLim.SurfWaterMass, 1.347668)

    assert np.isclose(output.log.final.LC17Auto.EnvelopeMass, 0.000000)
    assert np.isclose(output.log.final.LC17Auto.PlanetRadius, 1.063513)
    assert np.isclose(output.log.final.LC17Auto.SurfWaterMass, 1.347368)


if __name__ == "__main__":
    test_HabEvapCore()

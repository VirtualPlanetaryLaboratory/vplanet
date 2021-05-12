import vplanet
import numpy as np
import astropy.units as u


def test_AbioticO2():
    """Test oxygen build up, HZ limits."""
    # Grab the output
    output = vplanet.run(quiet=True, clobber=True)

    # Star
    assert np.isclose(output.log.final.star.Luminosity, 7.362835e23 * u.W)
    assert np.isclose(output.log.final.star.LXUVStellar, 7.362835e20 * u.W)
    assert np.isclose(output.log.final.star.Radius, 1.186502e08 * u.m)
    assert np.isclose(output.log.final.star.Temperature, 2926.556751 * u.Kelvin)
    assert np.isclose(output.log.final.star.RadGyra, 0.466090)

    # Planet b -- checks high XUV flux environment
    assert np.isclose(output.log.final.b.SurfWaterMass, 4.187987 * u.TO, rtol=1e-4)
    assert np.isclose(output.log.final.b.OxygenMass, 251.127387 * u.bar, rtol=1e-4)

    # Planet e -- checks low XUV flux environment
    assert np.isclose(output.log.final.e.SurfWaterMass, 7.511356 * u.TO, rtol=1e-4)
    assert np.isclose(output.log.final.e.OxygenMass, 420.619083 * u.bar)

    # Other checks
    assert np.isclose(output.log.final.e.FXUV, 3.053257 * u.W / u.m ** 2)
    assert np.isclose(output.log.final.e.AtmXAbsEffH2O, 0.051776)
    assert np.isclose(output.log.final.e.Instellation, 3053.257033 * u.F_F_Earth)


if __name__ == "__main__":
    test_AbioticO2()

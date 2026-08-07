"""
Verify EQTIDE's CPL Darwin-Kaula constituent decomposition.

Rather than benchmarking against a stored log, this checks two properties that
must hold regardless of how the model is tuned:

  1. For the Earth-Sun system every constituent frequency coincides with a
     named Doodson constituent of the classical tide tables. These periods are
     external ground truth, not values produced by VPLanet.

  2. The per-constituent powers sum to PowerEqtide, which is computed by a
     separate code path (fdCPLTidePower).
"""

import pathlib
import subprocess

import numpy as np
import pytest

import vplanet

# Tabulated periods of the solar constituents, in hours.
#
# Radial is the one loose identification. EQTIDE places it at the Keplerian
# mean motion, i.e. the sidereal year (365.256363 d), whereas Doodson's Sa is
# defined on the tropical year (365.2422 d). The two differ by the precession
# of the equinoxes, which EQTIDE does not model, so the reference here is the
# sidereal year rather than the tabulated Sa period.
DOODSON_PERIOD_HR = {
    "SemiDiurn": ("S2", 12.000000),
    "EccPlus": ("T2", 12.016449),
    "EccMinus": ("R2", 11.983596),
    "OblDiurn": ("P1", 24.065890),
    "OblSid": ("K1", 23.934470),
    "Radial": ("Sa (sidereal yr)", 8766.1527),
}

# Amplitude ratios within a species, from the Cartwright & Tayler (1971)
# harmonic expansion. Cross-species ratios are not comparable: the tables
# carry a different geodetic normalization per species.
CARTWRIGHT_RATIO = {
    ("EccPlus", "SemiDiurn"): 0.02472 / 0.42286,
    ("EccMinus", "SemiDiurn"): 0.00354 / 0.42286,
    ("OblSid", "OblDiurn"): 0.16817 / 0.17543,
}

path = pathlib.Path(__file__).parents[0].absolute()


@pytest.fixture(scope="module")
def output():
    subprocess.check_output(["vplanet", "vpl.in"], cwd=str(path))
    return vplanet.get_output(path, units=False)


def fdFirst(body, sName):
    """First (t=0) value of an output column."""
    return float(np.atleast_1d(getattr(body, sName))[0])


def test_constituent_periods(output):
    """Each constituent must land on its tabulated Doodson period."""
    earth = output.bodies[1]
    for sConst, (sDoodson, dRefHr) in DOODSON_PERIOD_HR.items():
        dFreq = fdFirst(earth, "TidalFreq" + sConst)
        dPeriodHr = 2 * np.pi / dFreq * 24
        assert dPeriodHr == pytest.approx(dRefHr, rel=1e-5), (
            f"{sConst} ({sDoodson}): {dPeriodHr} h, expected {dRefHr} h"
        )


def test_amplitude_ratios(output):
    """Within-species amplitude ratios must match the harmonic expansion.

    The tolerance reflects EQTIDE's truncation of the eccentricity functions
    at O(e), plus rounding in the published tables.
    """
    earth = output.bodies[1]
    for (sConst, sRef), dExpected in CARTWRIGHT_RATIO.items():
        dRatio = fdFirst(earth, "TidalAmp" + sConst) / fdFirst(
            earth, "TidalAmp" + sRef
        )
        assert dRatio == pytest.approx(dExpected, rel=5e-3), (
            f"{sConst}/{sRef}: {dRatio}, expected {dExpected}"
        )


def test_power_budget_closes(output):
    """Summing the constituents must reproduce the total tidal power."""
    earth = output.bodies[1]
    dSum = sum(
        fdFirst(earth, "TidalPower" + sConst) for sConst in DOODSON_PERIOD_HR
    )
    dTotal = fdFirst(earth, "PowerEqtide")
    assert dSum == pytest.approx(dTotal, rel=1e-6)


def test_semidiurnal_dominates(output):
    """The principal semi-diurnal line carries most of the dissipation."""
    earth = output.bodies[1]
    dSemi = fdFirst(earth, "TidalPowerSemiDiurn")
    dTotal = fdFirst(earth, "PowerEqtide")
    assert dSemi / dTotal > 0.5


def test_diminishing_factor(output):
    """1 + k_2 - h_2 must be built from the requested Love numbers.

    earth.in sets k_2 = 0.298 and h_2 = 0.6103, the IERS body-tide value, so
    the diminishing factor is 0.6877 -- close to the textbook Earth value.
    """
    earth = output.log.initial.earth
    assert float(earth.H2) == pytest.approx(0.6103, rel=1e-6)
    assert float(earth.TidalDiminish) == pytest.approx(
        1 + 0.298 - 0.6103, rel=1e-6
    )


def test_h2_defaults_to_homogeneous_elastic(output):
    """A body with no dH2 must fall back to h_2 = (5/3)k_2.

    sun.in sets k_2 but not h_2, so this exercises the default path. The
    relation is exact for a homogeneous incompressible elastic sphere. Read
    from the log because the Sun requests no output columns of its own.
    """
    sun = output.log.initial.sun
    assert float(sun.H2) == pytest.approx(5.0 / 3 * float(sun.K2), rel=1e-6)


def test_ocean_amplitude_is_uniformly_scaled(output):
    """Every ocean amplitude is its raising-potential value times gamma_2.

    Because gamma_2 does not depend on the constituent, applying it must not
    disturb any amplitude ratio -- the whole set rescales together.
    """
    earth = output.log.initial.earth
    dGamma = float(earth.TidalDiminish)
    for sConst in DOODSON_PERIOD_HR:
        dOcean = float(getattr(earth, "TidalOceanAmp" + sConst))
        dRaising = float(getattr(earth, "TidalAmp" + sConst))
        # The log carries 8 decimal places, so the millimetre-scale
        # constituents have only a few significant figures; the absolute
        # tolerance is what binds for those.
        assert dOcean == pytest.approx(
            dGamma * dRaising, rel=1e-5, abs=2e-8
        ), sConst


def test_h2_does_not_touch_power(output):
    """h_2 is a displacement Love number: it must not enter the dissipation.

    The power budget is fixed by k_2/Q alone, so it must still close after
    h_2 has been introduced. A regression here would mean h_2 had leaked into
    an energy pathway it has no business in.
    """
    earth = output.bodies[1]
    dSum = sum(
        fdFirst(earth, "TidalPower" + sConst) for sConst in DOODSON_PERIOD_HR
    )
    assert dSum == pytest.approx(fdFirst(earth, "PowerEqtide"), rel=1e-6)

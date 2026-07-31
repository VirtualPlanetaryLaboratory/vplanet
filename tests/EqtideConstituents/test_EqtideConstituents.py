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

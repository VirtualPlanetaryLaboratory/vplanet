"""
Verify the constituent diagnostics degrade safely under a non-CPL model.

The Darwin-Kaula decomposition is a constant-phase-lag construction. Under the
constant-time-lag model there is no discrete set of constituents to report, and
the phase-lag array the power routine reads is never allocated.

Two failure modes are guarded here, both of which were live before this test
existed:

  1. Requesting a per-constituent power under CTL SEGFAULTED. Marking the
     outputs with iNum = 0 does not stop the write function from being called;
     it only zeroes the emitted column count, so the writer still ran and
     dereferenced an unallocated array.

  2. Requesting a frequency or amplitude under CTL silently dropped the
     column. The run completed and the output file simply had fewer columns
     than the user asked for, with no diagnostic.

Both are now handled by returning the not-applicable sentinel -1. That value is
unambiguous because a frequency, an amplitude and a dissipated power are all
non-negative by construction.
"""

import pathlib
import subprocess

import numpy as np
import pytest

import vplanet

CONSTITUENTS = ["SemiDiurn", "EccPlus", "EccMinus", "Radial", "OblDiurn", "OblSid"]

# Every per-constituent family. All are CPL-only.
FAMILIES = ["TidalFreq", "TidalAmp", "TidalOceanAmp", "TidalPower"]

path = pathlib.Path(__file__).parents[0].absolute()


@pytest.fixture(scope="module")
def output():
    # check_output raises on a nonzero exit, so a crash here fails the test
    # rather than silently producing no data.
    subprocess.check_output(["vplanet", "vpl.in"], cwd=str(path))
    return vplanet.get_output(path, units=False)


def fdFirst(body, sName):
    return float(np.atleast_1d(getattr(body, sName))[0])


def test_run_completes_under_ctl(output):
    """The run must finish. This is the segfault regression."""
    assert output.bodies[1].Time is not None


def test_every_constituent_output_is_sentinel(output):
    """All 24 per-constituent outputs must report -1, not a value."""
    d = output.bodies[1]
    for sFamily in FAMILIES:
        for sConst in CONSTITUENTS:
            dValue = fdFirst(d, sFamily + sConst)
            assert dValue == -1, f"{sFamily}{sConst} returned {dValue}"


def test_requested_columns_are_not_dropped(output):
    """Each requested column must be present.

    Reading the attribute at all is the assertion: a dropped column would not
    exist on the body object. This is the silent-truncation regression.
    """
    d = output.bodies[1]
    for sFamily in FAMILIES:
        for sConst in CONSTITUENTS:
            assert hasattr(d, sFamily + sConst), f"{sFamily}{sConst} missing"


def test_diminishing_factor_survives(output):
    """gamma_2 is an elastic property, not a CPL one, so it must still work.

    d.in sets k_2 = 0.3 and no h_2, so h_2 = (5/3)(0.3) = 0.5 and
    gamma_2 = 1 + 0.3 - 0.5 = 0.8. Note this is a case where -1 would have
    been an unsafe sentinel: gamma_2 legitimately reaches 0 for a fluid body.
    """
    d = output.bodies[1]
    assert fdFirst(d, "H2") == pytest.approx(0.5, rel=1e-6)
    assert fdFirst(d, "TidalDiminish") == pytest.approx(0.8, rel=1e-6)

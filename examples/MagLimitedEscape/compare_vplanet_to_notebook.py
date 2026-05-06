#!/usr/bin/env python3
"""Numerical comparison of vplanet's Gunell-mechanism outputs against the
verbatim transcription of the notebook's T1e block. For each point in
the cached vplanet sweep, evaluate the notebook code at the same magnetic
moment and print absolute values + ratio."""

import os
import numpy as np
from astropy import units as u

import notebook_t1e_verbatim as ref

THIS_DIR = os.path.dirname(os.path.abspath(__file__))
PROTON_MASS_KG = 1.67e-27
EARTH_ATM_KG = 5.972e18
GYR_SECONDS = 3.156e16
EARTH_DIPOLE_AM2 = 8.0e22

VPLANET_COLUMNS = {
    "Pickup": 3,
    "CrossField": 4,
    "PolarCap": 5,
    "Cusp": 6,
    "Driscoll": 7,
}


def fdaConvertParticlesPerSecToAtmPerGyr(daRatesParticlesPerSec):
    """Convert column raw values (particles/s) into Earth atm/Gyr."""
    return daRatesParticlesPerSec * PROTON_MASS_KG * GYR_SECONDS / EARTH_ATM_KG


def fnPrintComparisonTable():
    """Print side-by-side vplanet vs notebook reference at every cached M."""
    sCachePath = os.path.join(THIS_DIR, "t1e_sweep_cache.npz")
    dictCache = np.load(sCachePath)
    daFields = dictCache["fields"]
    daVplanet = dictCache["rates"]

    print(f"{'M_E':>10}  {'mech':>10}  {'vplanet':>14}  "
          f"{'notebook':>14}  {'ratio':>10}  units = atm/Gyr")
    print("-" * 80)

    for iIndex, dField in enumerate(daFields):
        M = dField * EARTH_DIPOLE_AM2 * (u.A * u.m ** 2)
        dictRefRates = {
            "Pickup": float(ref.particles_per_sec_to_atm_per_gyr(
                ref.t1e_pickup(M))),
            "CrossField": float(ref.particles_per_sec_to_atm_per_gyr(
                ref.t1e_crossfield(M))),
            "PolarCap": float(ref.particles_per_sec_to_atm_per_gyr(
                ref.t1e_polarcap(M))),
            "Cusp": float(ref.particles_per_sec_to_atm_per_gyr(
                ref.t1e_cusp(M))),
            "Driscoll": ref.driscoll_to_atm_per_gyr(ref.t1e_driscoll(M)),
        }
        for sMech, iCol in VPLANET_COLUMNS.items():
            dVplanetRaw = daVplanet[iIndex, list(VPLANET_COLUMNS.keys()).index(sMech)]
            dVplanetAtm = fdaConvertParticlesPerSecToAtmPerGyr(dVplanetRaw)
            dRefAtm = dictRefRates[sMech]
            dRatio = (dVplanetAtm / dRefAtm) if dRefAtm != 0 else float("inf")
            print(f"{dField:>10.2e}  {sMech:>10}  {dVplanetAtm:>14.4e}  "
                  f"{dRefAtm:>14.4e}  {dRatio:>10.4f}")
        print()


if __name__ == "__main__":
    fnPrintComparisonTable()

"""Trace why the Driscoll13 magnetic-limited rate goes negative.

Reimplements fdDriscollMagLimitedRate (src/atmesc.c) term by term to locate
the sign flip, and reports the exobase density n_L against the stellar-wind
density n_sw over a range of dipole moments.

Usage: python traceDriscollNegativeFlux.py <output.txt>
"""
import sys

# Constants mirrored from src/atmesc.h and src/vplanet.h
KBOLTZ = 1.38064852e-23
BIGG = 6.67428e-11
PI = 3.141592653589793
MAG_PROTON_MASS = 1.67e-27
MAG_VACUUM_PERMEABILITY = 4.0e-7 * PI
MAG_DRISCOLL_TIME = 10.0
MAG_DRISCOLL_MASSWATER = 1.0e21
MAG_DRISCOLL_SIGMA_COLL = 1.0e-17
MAG_DRISCOLL_ENERGY_EFF = 1.0 / 10.6e44
MAG_EARTH_DIPOLE = 8.0e22
MAG_NUM_WELL_MIXED_SPECIES = 5
SECSPERYEAR = 3.15576e7

REARTH = 6.3781e6
MEARTH = 5.972186e24


def fdMagnetopauseStandoff(dMagMoment, dNsw, dVsw, dFormFactor=1.0):
    """Chapman-Ferraro standoff distance (m)."""
    dNum = MAG_VACUUM_PERMEABILITY * dFormFactor**2 * dMagMoment**2
    dDen = 8.0 * PI**2 * dNsw * dVsw**2 * MAG_PROTON_MASS
    return (dNum / dDen) ** (1.0 / 6.0)


def fdDriscollTerms(dMagMoment, dNsw, dVsw, dTexo, dMass, dRadius):
    """Return the intermediate terms of the Driscoll13 rate."""
    dRadMP = fdMagnetopauseStandoff(dMagMoment, dNsw, dVsw, 1.0)
    dRadExo = dRadius + 396000.0
    dHExo = (KBOLTZ * dTexo * dRadExo**2) / (BIGG * dMass * 2.0 * MAG_PROTON_MASS)
    dNExo = 1.0 / (dHExo * MAG_DRISCOLL_SIGMA_COLL)
    dShield = 1.0 - (dRadExo / dRadMP)
    dNL = dNExo * (2.718281828459045 ** ((-dRadExo / dHExo) * dShield))
    dSurface = 4.0 * PI * dRadMP**2
    dFlux = MAG_DRISCOLL_ENERGY_EFF * dHExo * dSurface * (dNL - dNsw) / MAG_DRISCOLL_TIME
    return {
        "dRadMP": dRadMP, "dRadExo": dRadExo, "dHExo": dHExo, "dNExo": dNExo,
        "dShield": dShield, "dNL": dNL, "dNsw": dNsw,
        "dRate": dFlux * MAG_DRISCOLL_MASSWATER,
    }


def main(sOut):
    # Earth at 1 AU, matching examples/MagLimitedEscape/earth_const_driscoll
    dNsw, dVsw, dTexo = 8.0e6, 4.0e5, 1000.0
    dMass, dRadius = MEARTH, REARTH

    listLines = []
    listLines.append("Driscoll13 rate vs dipole moment (Earth, n_sw=8e6 m^-3)")
    listLines.append("")
    listLines.append(f"{'M/M_E':>8} {'r_mp (R_E)':>11} {'shield':>9} "
                     f"{'n_L (m^-3)':>12} {'n_L - n_sw':>13} {'rate (kg/s)':>13}")
    for fFrac in [0.01, 0.05, 0.1, 0.2, 0.3, 0.5, 0.7, 1.0, 1.5, 2.0, 5.0]:
        d = fdDriscollTerms(fFrac * MAG_EARTH_DIPOLE, dNsw, dVsw, dTexo, dMass, dRadius)
        listLines.append(
            f"{fFrac:>8.2f} {d['dRadMP'] / REARTH:>11.4f} {d['dShield']:>9.4f} "
            f"{d['dNL']:>12.4e} {d['dNL'] - d['dNsw']:>13.4e} {d['dRate']:>13.4e}")

    d = fdDriscollTerms(MAG_EARTH_DIPOLE, dNsw, dVsw, dTexo, dMass, dRadius)
    listLines.append("")
    listLines.append("Earth-dipole breakdown:")
    for sKey in ["dRadExo", "dHExo", "dNExo", "dRadMP", "dShield", "dNL", "dNsw", "dRate"]:
        listLines.append(f"  {sKey:<10} = {d[sKey]:.6e}")
    listLines.append("")
    listLines.append(f"  r_exo/h_exo          = {d['dRadExo'] / d['dHExo']:.4f}")
    listLines.append(f"  exponent             = {-d['dRadExo'] / d['dHExo'] * d['dShield']:.4f}")
    listLines.append(f"  n_L/n_sw             = {d['dNL'] / d['dNsw']:.6e}")

    # fdMagLimitedEscapeDtDriscoll13 multiplies the returned rate by the proton
    # mass, as the Gunell path legitimately does for its particles/s rates. But
    # fdDriscollMagLimitedRate has already scaled by MAG_DRISCOLL_MASSWATER.
    dBulk = MAG_NUM_WELL_MIXED_SPECIES * d["dRate"] * MAG_PROTON_MASS
    listLines.append("")
    listLines.append("Bulk rate handed to the derivatives:")
    listLines.append(f"  DriscollRate                 = {d['dRate']:.6e}")
    listLines.append(f"  x N x m_p -> DMagLimitedMassDt = {dBulk:.6e} kg/s")
    listLines.append(f"  mass moved over 4.5 Gyr        = "
                     f"{abs(dBulk) * 4.5e9 * SECSPERYEAR:.4e} kg")
    listLines.append("")
    listLines.append("  (Gunell18 for the same Earth case: DMagLimitedMassDt = 1.41 kg/s)")

    sText = "\n".join(listLines) + "\n"
    with open(sOut, "w") as f:
        f.write(sText)
    print(sText)


if __name__ == "__main__":
    main(sys.argv[1])

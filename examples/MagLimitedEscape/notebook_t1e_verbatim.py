#!/usr/bin/env python3
"""Verbatim transcription of the Trappist-1e block from the source notebook
(Combined for Paper, pages 22-26). Run this to obtain the notebook's
numerical values at any magnetic moment, then compare to vplanet's
output. This is the unit-level reference - if vplanet's numbers don't
match this script's numbers exactly, vplanet is wrong, not the plot."""

import numpy as np
from astropy import units as u
from astropy.constants import G, R_earth, M_earth, k_B, M_sun

# --- Constants (from notebook page 22) ---
m_H_TE = 3.34e-27 * u.kg
magnetic_p_TE = 4 * np.pi * 1e-7 * (u.H / u.m)

T_exo_H_TE = 900 * u.K
R_EE_TE = 0.920 * R_earth.to(u.m)
ree_exo_TE = R_EE_TE + 396000 * u.m
M_EE_TE = 0.692 * M_earth.to(u.kg)

ne_sw_TE = 5.79e3 * (1 / u.cm ** 3)
ve_sw_TE = 604000 * u.m / u.s
prot_mass_TE = 1.67e-27 * u.kg

Omega_pc_E_TE = 0.63
r_exo_E_TE = 6_871_000 * u.m
re_IMB_TE = 7_647_000 * u.m
formfact_o_TE = 1.16
Mag_Earth_TE = 8e22 * (u.A * u.m ** 2)
mass_TE = 1.67e-27 * u.kg

earth_atmosphere_in_kg_TE = 1e-6 * M_earth
seconds_per_gyr_TE = 1e9 * 365.25 * 24 * 3600 * u.s


def t1e_pickup(M):
    """Pickup escape (Q_pu) - notebook page 22-23."""
    Q0_pu_H_TE = 5e26 * (1 / u.s)
    he_H_TE = (k_B * T_exo_H_TE * ree_exo_TE ** 2) / (G * M_EE_TE * m_H_TE)
    R_MP = ((magnetic_p_TE * M ** 2) /
            (8 * np.pi ** 2 * ne_sw_TE * ve_sw_TE ** 2 * prot_mass_TE)) ** (1 / 6)
    if R_MP <= ree_exo_TE:
        Q_pu = Q0_pu_H_TE
    else:
        num = 2 * he_H_TE ** 3 + 2 * he_H_TE ** 2 * R_MP + he_H_TE * R_MP ** 2
        den = (2 * he_H_TE ** 3 + 2 * he_H_TE ** 2 * ree_exo_TE +
               he_H_TE * ree_exo_TE ** 2)
        exp_term = np.exp((ree_exo_TE - R_MP) / he_H_TE)
        Q_pu = Q0_pu_H_TE * (num / den) * exp_term
    return Q_pu


def t1e_crossfield(M):
    """Cross-field ion loss (Q_cf) - notebook page 23."""
    Q0_cf_H_TE = 7.7e25 * (1 / u.s)
    R_MP = ((magnetic_p_TE * M ** 2) /
            (8 * np.pi ** 2 * ne_sw_TE * ve_sw_TE ** 2 * prot_mass_TE)) ** (1 / 6)
    if R_MP >= re_IMB_TE:
        temp = 1 - (ree_exo_TE / R_MP)
        Omega_pc = (4 * np.pi * (1 - np.sqrt(temp))
                    if temp >= 0 else 0)
    else:
        Omega_pc = 0
    Q_cf = Q0_cf_H_TE * ((1 - (Omega_pc / (4 * np.pi))) /
                         (1 - (Omega_pc_E_TE / (4 * np.pi))))
    return Q_cf


def t1e_polarcap(M):
    """Polar cap loss (Q_pc) - notebook page 23-24."""
    Q0_pc_H_TE = 7.8e25 * (1 / u.s)
    R_MP = ((magnetic_p_TE * formfact_o_TE ** 2 * M ** 2) /
            (8 * np.pi ** 2 * ne_sw_TE * ve_sw_TE ** 2 * prot_mass_TE)) ** (1 / 6)
    if R_MP > re_IMB_TE:
        temp = 1 - (r_exo_E_TE / R_MP)
        Omega_pc = (4 * np.pi * (1 - np.sqrt(temp))
                    if temp >= 0 else 0)
    else:
        Omega_pc = 0
    Q_pc = (Q0_pc_H_TE * (Omega_pc / Omega_pc_E_TE) *
            ((ree_exo_TE / r_exo_E_TE) ** 2))
    return Q_pc


def t1e_cusp(M):
    """Cusp escape (Q_cu) - notebook page 24."""
    Q0_cu_H_TE = 5e24 * (1 / u.s)
    Qmax_cu_H_TE = 5e25 * (1 / u.s)
    nsw_E = 1e7 * (1 / u.m ** 3)
    vsw_E = 604000 * (u.m / u.s)
    r_c_E_TE = ((magnetic_p_TE * Mag_Earth_TE ** 2) /
                (8 * np.pi ** 2 * nsw_E * vsw_E ** 2 * prot_mass_TE)) ** (1 / 6)
    R_MP_TE = ((magnetic_p_TE * M ** 2) /
               (8 * np.pi ** 2 * ne_sw_TE * ve_sw_TE ** 2 * prot_mass_TE)) ** (1 / 6)
    r_c_e_TE = R_MP_TE
    if R_MP_TE > re_IMB_TE:
        temp_TE = 1 - (r_exo_E_TE / R_MP_TE)
        Omega_pc_TE = (4 * np.pi * (1 - np.sqrt(temp_TE))
                       if temp_TE >= 0 else 0)
    else:
        Omega_pc_TE = 0
    Q_cu_TE = (min(Q0_cu_H_TE * (r_c_e_TE / r_c_E_TE) ** 2,
                   Qmax_cu_H_TE) *
               (Omega_pc_TE / Omega_pc_E_TE) *
               (ree_exo_TE / r_exo_E_TE) ** 2)
    return Q_cu_TE


def t1e_driscoll(M):
    """Driscoll magnetic-limited (MPU) - notebook page 25."""
    time_TE = 10 * u.s
    mass_water_TE = 1e21 * u.kg
    sigma_coll_TE = 1e-17 * u.m ** 2
    energy_eff_TE = 1 / (10.6e44)
    r_mp = ((magnetic_p_TE * M ** 2) /
            (8 * np.pi ** 2 * ne_sw_TE * ve_sw_TE ** 2 * prot_mass_TE)) ** (1 / 6)
    h_exo = (k_B * T_exo_H_TE * ree_exo_TE ** 2) / (G * M_EE_TE * m_H_TE)
    n_exo = 1 / (h_exo * sigma_coll_TE)
    n_L = n_exo * np.exp((-ree_exo_TE / h_exo) * (1 - (ree_exo_TE / r_mp)))
    surface_a = 4 * np.pi * r_mp ** 2
    f_lm = ((energy_eff_TE * h_exo * surface_a *
             (n_L - ne_sw_TE)) / time_TE) * mass_water_TE
    return f_lm


def particles_per_sec_to_atm_per_gyr(Q):
    """Notebook conversion: particles/s -> Earth atm / Gyr via proton mass."""
    mass_loss = Q * mass_TE
    return ((mass_loss / earth_atmosphere_in_kg_TE) *
            seconds_per_gyr_TE).decompose()


def driscoll_to_atm_per_gyr(f_lm):
    """Notebook conversion for Driscoll: also × proton mass.

    The notebook leaves f_lm with mixed units because energy_eff = 1/(10.6e44)
    is treated as dimensionless. We follow the notebook's habit of taking
    .value to drop the leftover units."""
    mass_loss = f_lm * mass_TE
    val = (mass_loss * seconds_per_gyr_TE / earth_atmosphere_in_kg_TE)
    return val.value


if __name__ == "__main__":
    print("Notebook reference values for T1e at selected magnetic moments\n")
    fields_earth = [1e-4, 1e-3, 1e-2, 1e-1, 0.05, 0.08, 0.1, 1.0, 10.0, 100.0]
    print(f"{'M_E':>10}  {'Pickup':>12}  {'CrossF':>12}  {'PolarC':>12}  "
          f"{'Cusp':>12}  {'Driscl':>12}  (atm/Gyr)")
    for f in fields_earth:
        M = f * Mag_Earth_TE
        Qpu = t1e_pickup(M)
        Qcf = t1e_crossfield(M)
        Qpc = t1e_polarcap(M)
        Qcu = t1e_cusp(M)
        flm = t1e_driscoll(M)
        pu_atm = float(particles_per_sec_to_atm_per_gyr(Qpu))
        cf_atm = float(particles_per_sec_to_atm_per_gyr(Qcf))
        pc_atm = float(particles_per_sec_to_atm_per_gyr(Qpc))
        cu_atm = float(particles_per_sec_to_atm_per_gyr(Qcu))
        ds_atm = driscoll_to_atm_per_gyr(flm)
        print(f"{f:>10.1e}  {pu_atm:>12.4e}  {cf_atm:>12.4e}  "
              f"{pc_atm:>12.4e}  {cu_atm:>12.4e}  {ds_atm:>12.4e}")

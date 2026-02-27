"""
This script summarizes TRAPPIST-1e flarevar diagnostics from VPLanet forward outputs.
It compares smooth, periodic, and random-bin forcing under matched mean XUV energy.
It also reports regime-specific logs and a timestep resolution check against periodic-dt5.
"""

from __future__ import annotations

import argparse
import csv
import math
from pathlib import Path

import numpy as np

COL_TIME_YR = 0
COL_SURF_WATER_TO = 1
COL_OXYGEN_BAR = 2
COL_FXUV_WM2 = 3
COL_FXUV_CRIT_WM2 = 4
COL_WATER_REGIME = 5
COL_OXYGEN_DRAG_ACTIVE = 6
COL_DIFFLIM_ACTIVE = 7
COL_DIFFLIM_FIRST_AGE_MYR = 8
MIN_COLS = 9

RUNS = {
    "baseline": {
        "system": "trappist1e_flarevar_baseline",
        "label": "Smooth baseline",
    },
    "periodic": {
        "system": "trappist1e_flarevar_periodic",
        "label": "Flarevar periodic",
    },
    "random_bin": {
        "system": "trappist1e_flarevar_random",
        "label": "Flarevar random-bin",
    },
    "periodic_dt5": {
        "system": "trappist1e_flarevar_periodic_dt5",
        "label": "Flarevar periodic (dt/5)",
    },
}


def load_forward(case_dir: Path, system_name: str) -> np.ndarray:
    forward_path = case_dir / f"{system_name}.e.forward"
    if not forward_path.exists():
        raise FileNotFoundError(
            f"Missing output file: {forward_path}. Run vplanet in {case_dir}."
        )

    data = np.loadtxt(forward_path)
    if data.ndim == 1:
        data = data.reshape(1, -1)

    if data.shape[1] < MIN_COLS:
        raise ValueError(
            f"{forward_path} has {data.shape[1]} columns; expected at least {MIN_COLS}. "
            "Rerun with the updated saOutputOrder in e.in."
        )

    return data


def time_weighted_fraction(time_yr: np.ndarray, active_flag: np.ndarray) -> float:
    if len(time_yr) < 2:
        return float(active_flag[0] > 0.5)

    dt = np.diff(time_yr)
    total = np.sum(dt)
    if total <= 0:
        return float(np.mean(active_flag > 0.5))

    return float(np.sum(dt * (active_flag[:-1] > 0.5)) / total)


def first_diffusion_age_from_flag_myr(time_yr: np.ndarray, diff_active: np.ndarray) -> float:
    active_idx = np.where(diff_active > 0.5)[0]
    if active_idx.size == 0:
        return math.nan
    return float(time_yr[active_idx[0]] / 1.0e6)


def first_diffusion_age_from_column_myr(diff_first_col_myr: np.ndarray) -> float:
    valid_idx = np.where(diff_first_col_myr >= 0.0)[0]
    if valid_idx.size == 0:
        return math.nan
    return float(diff_first_col_myr[valid_idx[0]])


def summarize_case(data: np.ndarray) -> dict[str, float]:
    time_yr = data[:, COL_TIME_YR]
    water_to = data[:, COL_SURF_WATER_TO]
    oxygen_bar = data[:, COL_OXYGEN_BAR]
    fxuv = data[:, COL_FXUV_WM2]
    fxuv_crit = data[:, COL_FXUV_CRIT_WM2]
    drag_active = data[:, COL_OXYGEN_DRAG_ACTIVE]
    diff_active = data[:, COL_DIFFLIM_ACTIVE]
    diff_first_col = data[:, COL_DIFFLIM_FIRST_AGE_MYR]

    final_water_to = float(water_to[-1])
    total_lost_to = float(water_to[0] - water_to[-1])
    final_o2_bar = float(oxygen_bar[-1])

    diff_switch_from_flag = first_diffusion_age_from_flag_myr(time_yr, diff_active)
    diff_switch_from_col = first_diffusion_age_from_column_myr(diff_first_col)
    diff_switch_age_myr = (
        diff_switch_from_col if not math.isnan(diff_switch_from_col) else diff_switch_from_flag
    )

    drag_fraction = time_weighted_fraction(time_yr, drag_active)
    drag_active_steps_fraction = float(np.mean(drag_active > 0.5))

    return {
        "final_water_to": final_water_to,
        "total_lost_to": total_lost_to,
        "final_o2_bar": final_o2_bar,
        "diff_switch_age_myr": diff_switch_age_myr,
        "drag_fraction": drag_fraction,
        "drag_steps_fraction": drag_active_steps_fraction,
        "initial_o2_bar": float(oxygen_bar[0]),
        "max_o2_bar": float(np.max(oxygen_bar)),
        "fxuv_min": float(np.min(fxuv)),
        "fxuv_max": float(np.max(fxuv)),
        "fxuv_crit_min": float(np.min(fxuv_crit)),
        "fxuv_crit_max": float(np.max(fxuv_crit)),
    }


def format_age(value: float) -> str:
    if math.isnan(value):
        return "NA"
    return f"{value:.4f}"


def format_float(value: float, fmt: str = ".6f") -> str:
    if math.isnan(value):
        return "NA"
    return format(value, fmt)


def write_csv(path: Path, rows: list[tuple[str, dict[str, float]]]) -> None:
    with path.open("w", newline="") as f:
        writer = csv.writer(f)
        writer.writerow(
            [
                "run",
                "final_water_to",
                "total_water_lost_to",
                "final_o2_bar",
                "diffusion_switch_age_myr",
                "fraction_time_fxuv_gt_fxuv_crit",
            ]
        )
        for run_key, metrics in rows:
            writer.writerow(
                [
                    run_key,
                    f"{metrics['final_water_to']:.8f}",
                    f"{metrics['total_lost_to']:.8f}",
                    f"{metrics['final_o2_bar']:.8f}",
                    ""
                    if math.isnan(metrics["diff_switch_age_myr"])
                    else f"{metrics['diff_switch_age_myr']:.8f}",
                    f"{metrics['drag_fraction']:.8f}",
                ]
            )


def write_markdown(
    path: Path,
    comparison_rows: list[tuple[str, dict[str, float]]],
    resolution: dict[str, float],
) -> None:
    lines: list[str] = []
    lines.append("# TRAPPIST-1e flarevar comparison")
    lines.append("")
    lines.append(
        "All three runs below use the same time-averaged stellar XUV energy. "
        "Differences therefore isolate sensitivity to temporal structure in the forcing."
    )
    lines.append("")
    lines.append(
        "| Run | Final water [TO] | Total water lost [TO] | Final O2 [bar] | "
        "Diffusion switch age [Myr] | Fraction time $F_{\\mathrm{XUV}} > F_{\\mathrm{XUV,crit}}$ |"
    )
    lines.append("|---|---:|---:|---:|---:|---:|")

    for run_key, metrics in comparison_rows:
        lines.append(
            "| "
            f"{RUNS[run_key]['label']} | "
            f"{metrics['final_water_to']:.6f} | "
            f"{metrics['total_lost_to']:.6f} | "
            f"{metrics['final_o2_bar']:.6f} | "
            f"{format_age(metrics['diff_switch_age_myr'])} | "
            f"{metrics['drag_fraction']:.6f} |"
        )

    lines.append("")
    lines.append("## Resolution test (periodic vs periodic dt/5)")
    lines.append("")
    lines.append("| Metric | Periodic | Periodic dt/5 | Absolute difference |")
    lines.append("|---|---:|---:|---:|")
    lines.append(
        f"| Final water [TO] | {resolution['periodic_final_water_to']:.6f} | "
        f"{resolution['dt5_final_water_to']:.6f} | {resolution['delta_final_water_to']:.6e} |"
    )
    lines.append(
        f"| Final O2 [bar] | {resolution['periodic_final_o2_bar']:.6f} | "
        f"{resolution['dt5_final_o2_bar']:.6f} | {resolution['delta_final_o2_bar']:.6e} |"
    )
    lines.append(
        f"| Diffusion switch age [Myr] | {format_age(resolution['periodic_diff_switch_age_myr'])} | "
        f"{format_age(resolution['dt5_diff_switch_age_myr'])} | "
        f"{format_float(resolution['delta_diff_switch_age_myr'], '.6e')} |"
    )

    path.write_text("\n".join(lines) + "\n")


def write_regime_log(path: Path, rows: list[tuple[str, dict[str, float]]]) -> None:
    lines: list[str] = []
    lines.append("# TRAPPIST-1e regime log")
    lines.append("")
    lines.append(
        "Requested diagnostics: diffusion-limited transition age, oxygen buildup trajectory proxy "
        "(reported here as initial/final/max $P_{O_2}$), and oxygen-drag activity indicator."
    )
    lines.append("")
    lines.append(
        "| Run | Diffusion-limited transition age [Myr] | $P_{O_2}(0)$ [bar] | "
        "$P_{O_2}(t_{\\mathrm{end}})$ [bar] | max $P_{O_2}(t)$ [bar] | "
        "Drag-active fraction (time-weighted) | Drag-active fraction (step-weighted) |"
    )
    lines.append("|---|---:|---:|---:|---:|---:|---:|")

    for run_key, metrics in rows:
        lines.append(
            "| "
            f"{RUNS[run_key]['label']} | "
            f"{format_age(metrics['diff_switch_age_myr'])} | "
            f"{metrics['initial_o2_bar']:.6f} | "
            f"{metrics['final_o2_bar']:.6f} | "
            f"{metrics['max_o2_bar']:.6f} | "
            f"{metrics['drag_fraction']:.6f} | "
            f"{metrics['drag_steps_fraction']:.6f} |"
        )

    lines.append("")
    lines.append(
        "Time-series curves are plotted in `flarevar_trappist1e_regimes.(png|pdf)` "
        "for $P_{O_2}(t)$, oxygen-drag activity, and diffusion-limited activity."
    )

    path.write_text("\n".join(lines) + "\n")


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Summarize TRAPPIST-1e flarevar diagnostics from forward files."
    )
    parser.add_argument(
        "--root",
        type=Path,
        default=Path(__file__).resolve().parent,
        help="Path to FlareVar_Trappistt-1 directory.",
    )
    args = parser.parse_args()

    root = args.root.resolve()
    figures_dir = root / "figures"
    figures_dir.mkdir(exist_ok=True)

    results: dict[str, dict[str, float]] = {}
    for run_key, meta in RUNS.items():
        case_dir = root / run_key
        data = load_forward(case_dir, meta["system"])
        results[run_key] = summarize_case(data)

    comparison_order = ["baseline", "periodic", "random_bin"]
    comparison_rows = [(run_key, results[run_key]) for run_key in comparison_order]

    periodic = results["periodic"]
    periodic_dt5 = results["periodic_dt5"]
    resolution = {
        "periodic_final_water_to": periodic["final_water_to"],
        "dt5_final_water_to": periodic_dt5["final_water_to"],
        "delta_final_water_to": abs(periodic_dt5["final_water_to"] - periodic["final_water_to"]),
        "periodic_final_o2_bar": periodic["final_o2_bar"],
        "dt5_final_o2_bar": periodic_dt5["final_o2_bar"],
        "delta_final_o2_bar": abs(periodic_dt5["final_o2_bar"] - periodic["final_o2_bar"]),
        "periodic_diff_switch_age_myr": periodic["diff_switch_age_myr"],
        "dt5_diff_switch_age_myr": periodic_dt5["diff_switch_age_myr"],
        "delta_diff_switch_age_myr": abs(
            periodic_dt5["diff_switch_age_myr"] - periodic["diff_switch_age_myr"]
        )
        if not (
            math.isnan(periodic["diff_switch_age_myr"])
            or math.isnan(periodic_dt5["diff_switch_age_myr"])
        )
        else math.nan,
    }

    csv_path = figures_dir / "flarevar_trappist1e_comparison_table.csv"
    md_path = figures_dir / "flarevar_trappist1e_comparison_table.md"
    regime_log_path = figures_dir / "flarevar_trappist1e_regime_log.md"

    write_csv(csv_path, comparison_rows)
    write_markdown(md_path, comparison_rows, resolution)
    write_regime_log(regime_log_path, comparison_rows)

    print(md_path.read_text())
    print(regime_log_path.read_text())
    print(f"CSV saved to: {csv_path}")


if __name__ == "__main__":
    main()

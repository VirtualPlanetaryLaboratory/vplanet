"""
This script generates publication-style figures for the flarevar TRAPPIST-1e example.
It reads baseline, periodic, and random-bin VPLanet outputs and compares forcing and atmospheric response.
It also visualizes escape-regime diagnostics, oxygen buildup, and diffusion-limited transition timing.
"""

from __future__ import annotations

import pathlib
import subprocess
import sys

import matplotlib as mpl
import numpy as np
from matplotlib.ticker import MaxNLocator

mpl.use("Agg")
import matplotlib.pyplot as plt

path = pathlib.Path(__file__).parents[0].absolute()
sys.path.insert(1, str(path.parents[0]))
from get_args import get_args

COL_TIME = 0
COL_O2_BAR = 2
COL_FXUV = 3
COL_FXUV_CRIT_DRAG = 4
COL_OXYGEN_DRAG_ACTIVE = 6
COL_DIFFLIM_ACTIVE = 7
COL_DIFFLIM_FIRST_AGE = 8
MIN_PLANET_COLS = 9

mpl.rcParams.update(
    {
        "text.usetex": True,
        "font.family": "serif",
        "font.serif": ["Latin Modern Roman"],
        "axes.labelsize": 11,
        "axes.titlesize": 12,
        "legend.fontsize": 9,
        "xtick.labelsize": 10,
        "ytick.labelsize": 10,
        "axes.spines.top": False,
        "axes.spines.right": False,
        "axes.facecolor": "white",
        "figure.facecolor": "white",
        "savefig.facecolor": "white",
        "savefig.dpi": 300,
    }
)

RUNS = {
    "baseline": {
        "system": "trappist1e_flarevar_baseline",
        "label": "Baseline (smooth)",
        "color": "#111827",
        "linestyle": "-",
    },
    "periodic": {
        "system": "trappist1e_flarevar_periodic",
        "label": "Flarevar periodic",
        "color": "#1d4ed8",
        "linestyle": "-",
    },
    "random_bin": {
        "system": "trappist1e_flarevar_random",
        "label": "Flarevar random-bin",
        "color": "#c2410c",
        "linestyle": "-",
    },
}


def run_example_case(vplanet_bin: pathlib.Path, case_name: str) -> None:
    case_dir = path / case_name
    subprocess.run([str(vplanet_bin), "vpl.in"], cwd=case_dir, check=True)


def ensure_outputs(args) -> None:
    vplanet_bin = path.parents[1] / "bin" / "vplanet"
    if not vplanet_bin.exists():
        raise FileNotFoundError(
            f"Could not find VPLanet executable at {vplanet_bin}. Run `make legacy` first."
        )

    missing_output = False
    for case_name, meta in RUNS.items():
        star_file = path / case_name / f"{meta['system']}.star.forward"
        planet_file = path / case_name / f"{meta['system']}.e.forward"
        if not star_file.exists() or not planet_file.exists():
            missing_output = True
            break

    if args.rerun or missing_output:
        for case_name in RUNS:
            run_example_case(vplanet_bin, case_name)


def load_run(case_name: str, system_name: str) -> dict[str, np.ndarray]:
    star_file = path / case_name / f"{system_name}.star.forward"
    planet_file = path / case_name / f"{system_name}.e.forward"

    if not star_file.exists() or not planet_file.exists():
        raise FileNotFoundError(
            f"Missing output for {case_name}. Run makeplot with --rerun or run vplanet in {case_name}/."
        )

    star = np.loadtxt(star_file)
    planet = np.loadtxt(planet_file)

    if planet.ndim == 1:
        planet = planet.reshape(1, -1)

    if planet.shape[1] < MIN_PLANET_COLS:
        raise ValueError(
            f"{planet_file} has {planet.shape[1]} columns, expected at least {MIN_PLANET_COLS}. "
            "Rerun simulations with updated e.in output order."
        )

    diff_first_col = planet[:, COL_DIFFLIM_FIRST_AGE]
    valid = diff_first_col >= 0.0
    first_diff_myr = diff_first_col[valid][0] if np.any(valid) else np.nan

    return {
        "time_yr": star[:, 0],
        "time_myr": star[:, 0] / 1.0e6,
        "lxuv_base_lsun": star[:, 3],
        "lxuv_total_lsun": star[:, 4],
        "mult": star[:, 5],
        "fxuv_wm2": planet[:, COL_FXUV],
        "fxuv_crit_drag_wm2": planet[:, COL_FXUV_CRIT_DRAG],
        "o2_bar": planet[:, COL_O2_BAR],
        "drag_active": (planet[:, COL_OXYGEN_DRAG_ACTIVE] > 0.5).astype(float),
        "diff_active": (planet[:, COL_DIFFLIM_ACTIVE] > 0.5).astype(float),
        "surf_water_to": planet[:, 1],
        "diff_first_age_myr": np.array([first_diff_myr]),
    }


def running_time_average(time_yr: np.ndarray, values: np.ndarray) -> np.ndarray:
    if len(time_yr) < 2:
        return np.copy(values)

    elapsed = time_yr - time_yr[0]
    dt = np.diff(time_yr)
    cumulative_int = np.concatenate(([0.0], np.cumsum(values[1:] * dt)))

    avg = np.ones_like(values)
    nonzero = elapsed > 0.0
    avg[nonzero] = cumulative_int[nonzero] / elapsed[nonzero]
    return avg


def style_axes(ax: plt.Axes, x_nbins: int = 6) -> None:
    ax.xaxis.set_major_locator(MaxNLocator(nbins=x_nbins))
    ax.minorticks_off()
    ax.grid(True, which="major", color="#94a3b8", alpha=0.14, linewidth=0.5)


def make_response_figure(data_by_case: dict[str, dict[str, np.ndarray]], output_ext: str) -> None:
    fig, axes = plt.subplots(3, 1, figsize=(7.4, 9.2), sharex=True)

    for case_name, meta in RUNS.items():
        data = data_by_case[case_name]
        stride = 1 if case_name == "baseline" else 8
        alpha = 0.95 if case_name == "baseline" else 0.85

        axes[0].plot(
            data["time_myr"][::stride],
            data["lxuv_total_lsun"][::stride],
            color=meta["color"],
            linestyle=meta["linestyle"],
            linewidth=1.5,
            alpha=alpha,
            label=meta["label"],
        )
        axes[1].plot(
            data["time_myr"][::stride],
            data["fxuv_wm2"][::stride],
            color=meta["color"],
            linestyle=meta["linestyle"],
            linewidth=1.4,
            alpha=alpha,
        )
        axes[2].plot(
            data["time_myr"],
            data["surf_water_to"],
            color=meta["color"],
            linestyle=meta["linestyle"],
            linewidth=1.7,
            alpha=0.95,
        )

    axes[0].set_yscale("log")
    axes[1].set_yscale("log")

    axes[0].set_ylabel(r"$L_{\mathrm{XUV}}$ [$L_{\odot}$]")
    axes[1].set_ylabel(r"$F_{\mathrm{XUV}}$ [W m$^{-2}$]")
    axes[2].set_ylabel(r"$M_{\mathrm{water}}$ [TO]")
    axes[2].set_xlabel("System age [Myr]")

    axes[0].set_title("Stellar XUV forcing")
    axes[1].set_title("Planetary XUV flux at TRAPPIST-1e")
    axes[2].set_title("Atmospheric escape response")

    axes[0].legend(loc="upper right", frameon=False)

    for ax in axes:
        style_axes(ax)

    fig.suptitle(
        "TRAPPIST-1e coarse flarevar variability: forcing and response",
        y=0.985,
        fontsize=13,
    )
    fig.text(
        0.5,
        0.012,
        "All runs share the same time-averaged stellar XUV energy. Small separations in water loss trace nonlinear escape response to episodic forcing.",
        ha="center",
        va="bottom",
        fontsize=9,
    )
    fig.tight_layout(rect=(0.0, 0.045, 1.0, 0.96))

    outdir = path / "figures"
    outdir.mkdir(exist_ok=True)
    fig.savefig(outdir / f"flarevar_trappist1e_response.{output_ext}", bbox_inches="tight")


def make_diagnostics_figure(data_by_case: dict[str, dict[str, np.ndarray]], output_ext: str) -> None:
    baseline = data_by_case["baseline"]
    periodic = data_by_case["periodic"]
    random_bin = data_by_case["random_bin"]

    fig, axes = plt.subplots(2, 1, figsize=(7.4, 6.8), sharex=False)

    zoom_max_myr = 2.0
    periodic_mask = periodic["time_myr"] <= zoom_max_myr
    random_mask = random_bin["time_myr"] <= zoom_max_myr
    baseline_mask = baseline["time_myr"] <= zoom_max_myr

    axes[0].step(
        baseline["time_myr"][baseline_mask],
        baseline["mult"][baseline_mask],
        where="post",
        color=RUNS["baseline"]["color"],
        linewidth=1.4,
        linestyle="--",
        label=RUNS["baseline"]["label"],
    )
    axes[0].step(
        periodic["time_myr"][periodic_mask],
        periodic["mult"][periodic_mask],
        where="post",
        color=RUNS["periodic"]["color"],
        linewidth=1.6,
        label=RUNS["periodic"]["label"],
    )
    axes[0].step(
        random_bin["time_myr"][random_mask],
        random_bin["mult"][random_mask],
        where="post",
        color=RUNS["random_bin"]["color"],
        linewidth=1.4,
        label=RUNS["random_bin"]["label"],
    )
    axes[0].set_xlim(0.0, zoom_max_myr)
    axes[0].set_ylabel(r"$M(t)$ [1]")
    axes[0].set_title("Instantaneous flarevar multiplier (first 2 Myr)")
    axes[0].legend(
        loc="upper left",
        bbox_to_anchor=(1.01, 1.0),
        borderaxespad=0.0,
        frameon=False,
    )

    periodic_avg = running_time_average(periodic["time_yr"], periodic["mult"])
    random_avg = running_time_average(random_bin["time_yr"], random_bin["mult"])

    axes[1].plot(
        periodic["time_myr"],
        100.0 * (periodic_avg - 1.0),
        color=RUNS["periodic"]["color"],
        linewidth=1.6,
        label=RUNS["periodic"]["label"],
    )
    axes[1].plot(
        random_bin["time_myr"],
        100.0 * (random_avg - 1.0),
        color=RUNS["random_bin"]["color"],
        linewidth=1.6,
        label=RUNS["random_bin"]["label"],
    )
    axes[1].axhline(0.0, color="#475569", linewidth=0.9, linestyle="--")
    axes[1].set_xlim(0.2, periodic["time_myr"].max())
    axes[1].set_ylabel(r"$100\,(\langle M\rangle_t - 1)$ [\%]")
    axes[1].set_xlabel("System age [Myr]")
    axes[1].set_title("Running mean multiplier convergence")

    for ax in axes:
        style_axes(ax)

    fig.suptitle("Flarevar mean-preservation diagnostics", y=0.985, fontsize=13)
    fig.text(
        0.5,
        0.012,
        "The top panel shows the imposed two-state scheduling in time. The lower panel confirms convergence of the running mean multiplier toward unity.",
        ha="center",
        va="bottom",
        fontsize=9,
    )
    fig.tight_layout(rect=(0.0, 0.05, 0.83, 0.96))

    outdir = path / "figures"
    outdir.mkdir(exist_ok=True)
    fig.savefig(
        outdir / f"flarevar_trappist1e_diagnostics.{output_ext}",
        bbox_inches="tight",
    )


def make_regime_figure(data_by_case: dict[str, dict[str, np.ndarray]], output_ext: str) -> None:
    fig, axes = plt.subplots(3, 1, figsize=(7.4, 9.0), sharex=True)

    transition_lines: list[str] = []
    for case_name, meta in RUNS.items():
        data = data_by_case[case_name]
        time_myr = data["time_myr"]

        axes[0].plot(
            time_myr,
            data["o2_bar"],
            color=meta["color"],
            linewidth=1.7,
            linestyle=meta["linestyle"],
            label=meta["label"],
        )
        axes[1].step(
            time_myr,
            data["drag_active"],
            where="post",
            color=meta["color"],
            linewidth=1.3,
            linestyle=meta["linestyle"],
        )
        axes[2].step(
            time_myr,
            data["diff_active"],
            where="post",
            color=meta["color"],
            linewidth=1.3,
            linestyle=meta["linestyle"],
        )

        first_diff = float(data["diff_first_age_myr"][0])
        if np.isfinite(first_diff):
            axes[0].axvline(
                first_diff,
                color=meta["color"],
                linewidth=1.0,
                alpha=0.5,
                linestyle=":",
            )
            transition_lines.append(f"{meta['label']}: {first_diff:.3f} Myr")
        else:
            transition_lines.append(f"{meta['label']}: NA")

    axes[0].set_ylabel(r"$P_{\mathrm{O_2}}$ [bar]")
    axes[1].set_ylabel("Oxygen drag active [0/1]")
    axes[2].set_ylabel("Diffusion-limited [0/1]")
    axes[2].set_xlabel("System age [Myr]")

    axes[0].set_title(r"Atmospheric oxygen buildup $P_{\mathrm{O_2}}(t)$")
    axes[1].set_title(r"Oxygen drag indicator: $F_{\mathrm{XUV}} > F_{\mathrm{XUV,crit}}$")
    axes[2].set_title("Diffusion-limited regime indicator")

    axes[1].set_ylim(-0.05, 1.05)
    axes[2].set_ylim(-0.05, 1.05)
    axes[1].set_yticks([0, 1])
    axes[2].set_yticks([0, 1])

    axes[0].legend(loc="upper left", frameon=False)

    for ax in axes:
        style_axes(ax)

    fig.suptitle("TRAPPIST-1e escape-regime diagnostics", y=0.985, fontsize=13)
    fig.text(
        0.5,
        0.012,
        "First diffusion-limited age by run: " + " | ".join(transition_lines),
        ha="center",
        va="bottom",
        fontsize=9,
    )
    fig.tight_layout(rect=(0.0, 0.045, 1.0, 0.96))

    outdir = path / "figures"
    outdir.mkdir(exist_ok=True)
    fig.savefig(outdir / f"flarevar_trappist1e_regimes.{output_ext}", bbox_inches="tight")


def main() -> None:
    args = get_args()
    ensure_outputs(args)

    data_by_case = {
        case_name: load_run(case_name, meta["system"]) for case_name, meta in RUNS.items()
    }

    make_response_figure(data_by_case, args.ext)
    make_diagnostics_figure(data_by_case, args.ext)
    make_regime_figure(data_by_case, args.ext)

    if not args.quiet:
        print("Saved figures to", path / "figures")


if __name__ == "__main__":
    main()

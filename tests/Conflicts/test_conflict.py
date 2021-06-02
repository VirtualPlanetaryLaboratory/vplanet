"""
Raise error if merge conflicts are found.

"""
import pathlib
import subprocess


def test_conflict():
    # Find all files containing this pattern
    pattern = "<<<<<<< HEAD"

    # Path to top level repo directory
    path = pathlib.Path(__file__).absolute().parents[2]

    # Current git branch
    branch = (
        subprocess.check_output(["git", "branch", "--show-current"], cwd=path)
        .decode()
        .replace("\n", "")
    )

    # Get all files tracked by git
    files = (
        subprocess.check_output(
            ["git", "ls-tree", "-r", branch, "--name-only"], cwd=path
        )
        .decode()
        .split("\n")[:-1]
    )

    # Search in all files
    for file in files:

        # Ignore this file!
        if __file__ in str(path / file):
            continue

        try:
            output = subprocess.check_output(
                ["grep", "-nw", "-e", pattern, path / file]
            ).decode()
        except subprocess.CalledProcessError:
            # Error means grep didn't find anything
            pass
        else:
            raise ValueError(f"Merge conflict in {file}, line {output}")

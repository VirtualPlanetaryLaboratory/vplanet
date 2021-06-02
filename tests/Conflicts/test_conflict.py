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

    # Don't search for the pattern in this file or in .git
    this = pathlib.Path(__file__).name
    dotgit = path / ".git"

    try:

        # Call grep
        process = subprocess.check_output(
            [
                "grep",
                "-rnw",
                "-e",
                pattern,
                f"--exclude={this}",
                f"--exclude-dir={dotgit}",
                path,
            ]
        )

    except subprocess.CalledProcessError:

        # Grep didn't find anything: this is good!
        return

    # Grep found something. Let's parse the output
    results = process.decode().split("\n")
    results = [result for result in results if len(result)]

    # Raise AssertionError if pattern is found at least once
    if len(results) > 0:

        # DEBUG
        print(results)

        for i in results:
            info = i.split(" ")[0].rpartition(":")[0]
            file = info.partition(":")[0]
            line = info.partition(":")[-1]
            print("File:", file)
            print("Line Number:", line)
        assert False

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

    # Exclude certain files / folders
    exclude_files = [
        pathlib.Path(__file__).name,  # this file
        "*.pyc",  # temporary python files
    ]
    exclude_dirs = [
        ".git",  # the .git metadata, which is massive
        "__pycache__",  # temporary python files
    ]

    # Format the exclusion rules
    exclude_files = [
        "--exclude={}".format(file.replace("*", "\*")) for file in exclude_files
    ]
    exclude_dirs = [
        "--exclude={}".format(directory.replace("*", "\*"))
        for directory in exclude_dirs
    ]

    try:

        # Call grep
        process = subprocess.check_output(
            ["grep", "-rnw", "-e", pattern] + exclude_files + exclude_dirs + [path]
        )

    except subprocess.CalledProcessError:

        # Grep didn't find anything: this is good!
        return

    # Grep found something. Let's parse the output
    results = process.decode().split("\n")
    results = [result for result in results if len(result)]

    # Raise AssertionError if pattern is found at least once
    if len(results) > 0:

        for i in results:
            info = i.split(" ")[0].rpartition(":")[0]
            file = info.partition(":")[0]
            line = info.partition(":")[-1]
            print("File:", file)
            print("Line Number:", line)
        assert False

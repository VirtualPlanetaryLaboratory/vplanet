import os
import subprocess
import sys

# Run the simulations
dir_path = os.path.dirname(os.path.realpath(__file__))
examples = sorted(next(os.walk("."))[1])

# Run simulations
for dir in examples:
    print("Running " + dir + " example")
    os.chdir(os.path.join(dir_path, dir))

    if sys.argv[1] == "pdf":
        subprocess.call(["python", "makeplot.py", "pdf"])
    if sys.argv[1] == "png":
        subprocess.call(["python", "makeplot.py", "png"])

# Return to top-level directory
os.chdir(dir_path)

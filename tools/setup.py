import subprocess
import os
import sys

# Run the simulations
dir_path = os.path.dirname(os.path.realpath(__file__))
tools = sorted(next(os.walk('.'))[1])

# Run
for dir in tools:
    print ("Installing "+dir+" ...")
    os.chdir(os.path.join(dir_path,dir))

    if tools == "bigplanet" or tools == "vplot":
        subprocess.call(['python', 'setup.py', 'install'])
    else:
        subprocess.call(['python', 'setup.py'])

# Return to top-level directory
os.chdir(dir_path)

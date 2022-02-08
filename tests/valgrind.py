import os
import subprocess
import sys

import numpy as np

subdir = [f.name for f in os.scandir(".") if f.is_dir()]

# First make debug
sys.stdout.write("Making VPLanet...")
sys.stdout.flush()
os.chdir("..")
subprocess.run(["make debug >& /dev/null"], shell=True)
os.chdir("tests")

print("done.")

tot_fail = 0
tot_test = 0
for sub in subdir:
    tot_test += 1
    sys.stdout.write(sub)
    sys.stdout.flush()
    os.chdir(sub)
    fout = sub + ".valgrind"
    cmd = "valgrind --track-origins=yes ../../vplanet vpl.in >& " + fout
    subprocess.run(cmd, shell=True)
    f = open(fout, "r")
    last_line = f.readlines()[-1]
    f.close()
    words = last_line.split()
    n_errors = int(words[3])
    if n_errors > 0:
        tot_fail += 1
    print(": " + repr(n_errors) + " error(s)")
    os.chdir("..")

sys.stdout.write("Done! ")

if tot_fail == 0:
    print("VPLanet is mem-check-clean!")
else:
    print(repr(tot_fail) + "/" + repr(tot_test) + " test(s) failed.")

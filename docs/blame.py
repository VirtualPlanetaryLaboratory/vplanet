"""
Figure out who needs to write their docs!

I'm still working on this :)
"""
import glob
import os

srcfiles = [os.path.basename(x) for x in glob.glob("../src/*.[ch]")]

with open("tmp.txt", "r") as f:
    log = f.read()

print("Documentation errors:")
print("---------------------")
for file in srcfiles:
    counts = log.count(file)
    print("%10s: %d" % (file, counts))

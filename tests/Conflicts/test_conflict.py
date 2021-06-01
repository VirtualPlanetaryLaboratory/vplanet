import os
import subprocess


os.chdir("../../")

process = subprocess.check_output(['grep', '-rnw', '-e', "<<<<<<< HEAD"])

results = process.decode().split("\n")[0:-2]

if len(results) > 0:
    for i in results:
        info = i.split(" ")[0].rpartition(":")[0]
        file = info.partition(":")[0]
        line = info.partition(":")[-1]
        print("File:", file)
        print("Line Number:",line)
    assert False
else:
    assert True

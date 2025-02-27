import os
import subprocess
import sys


# XXX Near duplicates in maketest.py, 02/06/24
def Main():
    BuildVPLanet()
    dir_list = CollectAllTests()

    tot_fail = 0
    tot_test = 0
    for dir in dir_list:
        tot_test += 1
        sys.stdout.write(dir)
        sys.stdout.flush()
        os.chdir(dir)
        subdirs = dir.split("/")
        test = subdirs[1]
        outfile = test + ".sanitize"
        cmd = "../../../bin/vplanet vpl.in"

        with open(outfile, "w+") as f:
            fail = 0
            subprocess.run(cmd, shell=True, stdout=f, stderr=f)
            #subprocess.run(cmd, shell=True)
        f = open(outfile, "r")
        for line in f:
            if ("ABORTING" in line):
                fail = 1
                tot_fail += 1
        if (line != "Simulation completed.\n"):
                fail = 1
                tot_fail += 1
        if fail:
            print(": FAIL",flush=True)
        else:
            print(": pass",flush=True)
        f.close()

        os.chdir("../../")

    print("Done! ")

    if tot_fail == 0:
        print("VPLanet is sanitized!")
        assert True
    else:
        print(repr(tot_fail) + "/" + repr(tot_test) + " test(s) failed.")
        assert False
    exit(0)


def BuildVPLanet():
    sys.stdout.write("Building VPLanet...")
    sys.stdout.flush()
    os.chdir("../")
    subprocess.check_output(["make", "sanitize"])
    print("done.", flush=True)
    os.chdir("tests")


def CollectAllTests():
    top_list = sorted(next(os.walk("."))[1])
    dir_list = []
    for top in top_list:
        subdirs = [
            os.path.join(top, subdir) for subdir in sorted(next(os.walk(top))[1])
        ]
        for subdir in subdirs:
            if "pycache" not in subdir:
                dir_list.append(subdir)
    print(" ", flush=True)

    return dir_list


if __name__ == "__main__":
    Main()

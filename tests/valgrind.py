import os
import subprocess
import sys


# XXX Near duplicates in maketest.py, 02/06/24
def Main():
    BuildVPLanet()
    print("")
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
        outfile = test + ".valgrind"
        cmd = "/usr/bin/valgrind --track-origins=yes ../../../bin/vplanet vpl.in"

        with open(outfile, "w+") as f:
            subprocess.run(cmd, shell=True, stdout=f, stderr=f)
        f = open(outfile, "r")
        last_line = f.readlines()[-1]
        # print(last_line)
        f.close()
        words = last_line.split()
        n_errors = int(words[3])
        if n_errors > 0:
            tot_fail += 1
            print(": FAIL",flush=True)
        else:
            print(": pass",flush=True)
        os.chdir("../../")

    print("Done! ")

    if tot_fail == 0:
        print("VPLanet is mem-check-clean!")
        assert True
    else:
        print(repr(tot_fail) + "/" + repr(tot_test) + " test(s) failed.")
        assert False
    exit(0)


def BuildVPLanet():
    sys.stdout.write("Building VPLanet...")
    sys.stdout.flush()
    os.chdir("../")
    subprocess.check_output(["make", "opt"])
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

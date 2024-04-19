import os
import subprocess
import sys


# XXX Near duplicates in maketest.py, 02/06/24
def Main():
    BuildVPLanet()
    dir_list = CollectAllTests()
    RemoveOldOutputFiles(dir_list)

    print(" ", flush=True)
    tot_fail = 0
    tot_test = 0
    tot_error = 0
    for dir in dir_list:
        tot_test += 1
        sys.stdout.write(dir)
        sys.stdout.write(": ")
        sys.stdout.flush()
        os.chdir(dir)
        subdirs = dir.split("/")
        test = subdirs[1]
        outfile = test + ".floatingpoint"
        cmd = "../../../bin/vplanet vpl.in"

        with open(outfile, "w+") as f:
            subprocess.run(cmd, shell=True, stdout=f, stderr=f)
        f = open(outfile, "r")
        try:
            last_line = f.readlines()[-1]
        except:
            print("Error")
            os.chdir("../../")
            tot_error += 1
            continue
        # print(last_line)
        f.close()
        if last_line != "Simulation completed.\n":
            tot_fail += 1
            print("FAIL", flush=True)
        else:
            print("Pass", flush=True)
        os.chdir("../../")

    print("")

    if tot_fail == 0 and tot_error == 0:
        print("No floating point errors found!")
        assert True
    else:
        print(
            repr(tot_fail)
            + "/"
            + repr(tot_test)
            + " test(s) failed; "
            + repr(tot_error)
            + "/"
            + repr(tot_test)
            + " test(s) errored."
        )
        assert False
    exit(0)


def BuildVPLanet():
    sys.stdout.write("Building VPLanet...")
    sys.stdout.flush()
    os.chdir("../")
    subprocess.check_output(["make", "debug"])
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

    return dir_list


def RemoveOldOutputFiles(dir_list):
    for dir in dir_list:
        os.chdir(dir)
        subdirs = dir.split("/")
        test = subdirs[1]
        outfile = test + ".floatingpoint"
        # print (test,outfile)
        if os.path.exists(outfile):
            os.remove(outfile)
        os.chdir("../../")


if __name__ == "__main__":
    Main()

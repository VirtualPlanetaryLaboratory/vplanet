import os
import shutil
import glob
import random
import multiprocessing as mp
import sys
import fileinput
import subprocess as sub
import numpy as np
import math
import pdb

# --------------------------------------------------------------------


def get_VSPACE(vspace_name):
    vsf = open(vspace_name, 'r')
    vspace_all = vsf.readlines()
    check = vspace_all[3]
    destLine = vspace_all[1]
    destfolder = destLine.strip().split(None, 1)[1]

    if "samplemode" in check:
        typ = "MC"
    else:
        typ = "NM"

    vsf.close()
    return [destfolder, typ]

# --------------------------------------------------------------------


def rand_dist(folder_name, vspace_file):
    vspace = open(vspace_file, "r")

    for line in vspace:
        line.strip().split('/n')

        if line.startswith('seed '):
            seedline = line.split()
            seed = seedline[1]
            np.random.seed(int(seed))

    # get number of files
    files = sub.check_output(
        "find %s -maxdepth 1 -mindepth 1 -type d" % folder_name, shell=True).split()
    for f in files:
        os.chdir(f.decode('UTF-8'))
        earth = open('earth.in', "r+")

        for line in earth:
            line.strip().split('/n')

            if line.startswith('dEcc '):
                eccline = line.split()
                ecc = float(eccline[1])
                rng = 1 - ecc

                if rng < ecc:
                    EccAmp = str(np.random.uniform(
                        low=np.float(0.01), high=np.float(rng)))
                else:
                    EccAmp = str(np.random.uniform(
                        low=np.float(0.01), high=np.float(ecc)))
                texteamp = "dEccAmp      " + EccAmp + "\n"

            if line.startswith('dObliquity'):
                oblline = line.split()
                ObliqAmp = str(np.random.uniform(
                    low=np.float(5), high=np.float(oblline[1])))
                textoblamp = "dObliqAmp      " + ObliqAmp + "\n"

        earth.write(textoblamp + "\n")
        earth.write(texteamp + "\n")
        earth.close()
        os.chdir("../../")

# --------------------------------------------------------------------


def vDirSplit(srcDir, cores=1):
    # cores is 1 no need to merge directories
    if cores == 1:
        return

    if not os.path.exists(srcDir):
        print("Error")
        return

    for i in range(1, cores + 1):
        coreDir = srcDir + "_c" + str(i)
        if os.path.isdir(coreDir) == False:
            os.mkdir(coreDir)

    subDirs = os.listdir(srcDir)
    x = list(range(1, cores + 1)) * ((len(subDirs) // cores) + 1)
    x = x[0:len(subDirs)]

    for i in range(len(subDirs)):
        coreDir = srcDir + "_c" + str(x[i])
        sourceDir = os.path.join(srcDir, subDirs[i])
        destDir = os.path.join(coreDir, subDirs[i])
        shutil.move(sourceDir, destDir)

# --------------------------------------------------------------------


def moveAllFilesinDir(srcDir, dstDir):
    # Check if both the are directories
    if os.path.isdir(srcDir) and os.path.isdir(dstDir):
        # Iterate over all the files in source directory
        for filePath in glob.glob(srcDir + '/*'):
            # Move each file to destination Directory
            shutil.move(filePath, dstDir)
    else:
        print("srcDir & dstDir should be Directories")

# --------------------------------------------------------------------


def vDirMerge(srcDir, cores=1):
    # cores is 1 no need to merge directories
    if cores == 1:
        return

    for i in range(1, cores + 1):
        coreDir = srcDir + "_c" + str(i)
        moveAllFilesinDir(coreDir, srcDir)

        if len(os.listdir(coreDir)) == 0:
            os.rmdir(coreDir)

# --------------------------------------------------------------------


def run_vplanet(folder_name, vspace_name, typ):
    final = '---- FINAL SYSTEM PROPERTIES ----'
    if typ == "MC":
        rand_dist(folder_name, vspace_name)
    # get number of files
    files = sub.check_output("find %s -maxdepth 1 -mindepth 1 -type d" % folder_name, shell=True).split()

    for f in files:
        print(f)
        os.chdir(f.decode('UTF-8'))

        if os.path.exists('tilted.log') == False:
            sub.call("vplanet vpl.in", shell=True)
        else:
            logf = open('tilted.log', 'r')
            lines = logf.readlines()
            logf.close()

            if final in lines:
                pass
            else:
                sub.call("vplanet vpl.in", shell=True)

        os.chdir("../../")

# --------------------------------------------------------------------


def multiProcess(srcDir, cores, vspaceName,typ):
    print(srcDir, cores)

    # Define an output queue
    output = mp.Queue()

    # Setup a list of processes that we want to run
    processes = []
    for i in range(1, cores + 1):
        coreDir = srcDir + "_c" + str(i)
        m = mp.Process(target=run_vplanet, args=(coreDir,vspaceName,typ))
        processes.append(m)

    # Run processes
    for p in processes:
        p.start()

    # Exit the completed processes
    for p in processes:
        p.join()

# --------------------------------------------------------------------


def main():
    vspaceName = sys.argv[1]
    cores = int(sys.argv[2])

    dirName = get_VSPACE(vspaceName)
    dir = dirName[0]
    typ = dirName[1]
    print(dir)
    print(typ)

    vDirSplit(dir, cores)

    if cores > 1:
        multiProcess(dir, cores, vspaceName, typ)
    else:
        run_vplanet(dir, vspaceName, typ)

    vDirMerge(dir, cores)

if __name__ == "__main__":
    main()

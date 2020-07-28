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
    destLine = vspace_all[1]
    destfolder = destLine.strip().split(None, 1)[1]

    vsf.close()
    return destfolder

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


def run_vplanet(folder_name, vspace_name):
    final = '---- FINAL SYSTEM PROPERTIES ----'
    #if typ == "MC":
        #rand_dist(folder_name, vspace_name)
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


def multiProcess(srcDir, cores, vspaceName):
    print(srcDir, cores)

    # Define an output queue
    output = mp.Queue()

    # Setup a list of processes that we want to run
    processes = []
    for i in range(1, cores + 1):
        coreDir = srcDir + "_c" + str(i)
        m = mp.Process(target=run_vplanet, args=(coreDir,vspaceName))
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

    dir = get_VSPACE(vspaceName)

    vDirSplit(dir, cores)

    if cores > 1:
        multiProcess(dir, cores, vspaceName)
    else:
        run_vplanet(dir, vspaceName)

    vDirMerge(dir, cores)

if __name__ == "__main__":
    main()

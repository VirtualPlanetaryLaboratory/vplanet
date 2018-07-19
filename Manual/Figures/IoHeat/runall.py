#!/anaconda3/bin/python

# Script to run all vspace runs
import string
import subprocess as subp

# Number of dimensions
necc=10
nobl=10
ecc=["" for j in range(necc)]
obl=["" for j in range(nobl)]
heat=[["" for j in range(necc)] for k in range(nobl)]

result = subp.run("ls -d data/io*", shell=True, stdout=subp.PIPE).stdout.decode('utf-8')
dirs=result.split()

# Dirs contains array of directories to run

iEcc=0
iObl=0

for dir in dirs:
    if dir != "0":  # WTF?
        cmd = "cd "+dir+"; vplanet vpl.in >& output"
        subp.call(cmd, shell=True)
        # At this point the log file has been generated

        logfile=dir+'/ioheat.log'
        log=open(logfile,"r")

        print(dir)
        # Now search for Io's parameters
        found=0
        for line in log:
            #print(str(len(line)))
            words=line.split()
            if len(words) > 2:
                #print(line)
                #print(words[1],words[2])
                #print("%d %d\n" % (iEcc,iObl))
                if (words[1] == "BODY:") and (words[2] == "io"):
                    found=1
                    #print("Found Io.")
                if (words[0] == "(Eccentricity)") and (found == 1):
                    ecc[iEcc] = words[4]
                    #print("Found e")
                if (words[0] == "(Obliquity)") and (found == 1):
                    obl[iObl] = words[3]
                    #print("Found o")
                if (words[0] == "(SurfEnFluxEqtide)") and (found == 1):
                    heat[iEcc][iObl] = words[10]
                    #print("Found h")
        print(ecc[iEcc],obl[iObl], heat[iEcc][iObl])
        #exit()
        print("%d %d\n" % (iEcc,iObl))
        iObl += 1
        if (iObl == 10):
        # New line in ecc
            iEcc += 1
            iObl = 0

# Arrays ecc,obl,heat now contain the data to make the figure

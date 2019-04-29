#!/usr/local/bin/python3.7

# Script to run all vspace runs
import sys
import string
import subprocess as subp
import matplotlib.pyplot as plt
import vplot as vpl

# Check correct number of arguments
if (len(sys.argv) != 2):
    print('ERROR: Incorrect number of arguments.')
    print('Usage: '+sys.argv[0]+' <pdf | png>')
    exit(1)
if (sys.argv[1] != 'pdf' and sys.argv[1] != 'png'):
    print('ERROR: Unknown file format: '+sys.argv[1])
    print('Options are: pdf, png')
    exit(1)

# Number of dimensions
necc=30
nobl=30
ecc=[0 for j in range(necc)]
obl=[0 for j in range(nobl)]
heat=[[0 for j in range(necc)] for k in range(nobl)]

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
        sys.stdout.flush()
        sys.stderr.flush()
        # Now search for Io's parameters
        found=0
        for line in log:
            words=line.split()
            if len(words) > 2:
                if (words[1] == "BODY:") and (words[2] == "io"):
                    found=1
                if (words[0] == "(Eccentricity)") and (found == 1):
                    ecc[iEcc] = float(words[4])
                if (words[0] == "(Obliquity)") and (found == 1):
                    obl[iObl] = float(words[3])*180/3.1415926535
                if (words[0] == "(SurfEnFluxEqtide)") and (found == 1):
                    heat[iObl][iEcc] = float(words[10])
        iObl += 1
        if (iObl == nobl):
        # New line in ecc
            iEcc += 1
            iObl = 0

# Arrays ecc,obl,heat now contain the data to make the figure

plt.xlabel('Eccentricity',fontsize=20)
plt.ylabel('Obliquity ($^\circ$)',fontsize=20)
plt.tick_params(axis='both', labelsize=20)

#plt.xscale('log')
plt.yscale('log')
plt.xlim(1e-3,0.01)
plt.ylim(1e-3,1)

ContSet = plt.contour(ecc,obl,heat,5,colors='black',linestyles='solid',
                      levels=[0.01,0.1,1,10,100],linewidths=3,origin='lower')
plt.clabel(ContSet,fmt="%.2f",inline=True,fontsize=18)

# Io's heat flux is 1.5-3 W/m^2. After some fussing, this choice of contour matches that range.
plt.contour(ecc,obl,heat,5,colors=vpl.colors.orange,linestyles='solid',
                      levels=[2.1],linewidths=45,origin='lower')

plt.tight_layout()

x=[0.0041,0.0041]
y=[1e-3,1]
plt.plot(x,y,linestyle='dashed',color='black')

x=[1e-3,0.3]
y=[0.0023,0.0023]
plt.plot(x,y,linestyle='dotted',color='black')

if (sys.argv[1] == 'pdf'):
    plt.savefig('IoHeat.pdf')
if (sys.argv[1] == 'png'):
    plt.savefig('IoHeat.png')

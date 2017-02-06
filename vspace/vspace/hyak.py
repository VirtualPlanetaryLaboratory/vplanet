import vspace_hyak
import sys
import os
import pdb

if len(sys.argv) < 2:
  raise IOError("Must enter an input file name")
else:
  inputf = sys.argv[1]

if os.path.isdir(inputf):
  destfolder = inputf
else:
  try:
    f = open(inputf,'r')
  except IOError:
    print ("%s is not a valid file name. Please reenter." %inputf)
  destfolder, trialname, infiles, src = vspace_hyak.parseInput(infile=inputf)

para = "parallel_sql"

# Make command list and .sh files to run the scripts
vspace_hyak.makeCommandList(simdir=destfolder,infile=inputf,para=para)

# Make the submission script
vspace_hyak.makeHyakVPlanetPBS(script="run_vplanet.pbs",taskargs="vplArgs.txt",
                       walltime="00:30:00",para=para,
                       simdir=destfolder,logdir=destfolder)

import vspace_hyak

para = "parallel_sql"

destfolder, trialname, infiles, src = vspace_hyak.parseInput(infile=inputf)
    
# Make command list and .sh files to run the scripts
vspace_hyak.makeCommandList(simdir=destfolder,infile=inputf,para=para)

# Make the submission script
vspace_hyak.makeHyakVPlanetPBS(script="run_vplanet.pbs",taskargs="vplArgs.txt",
                       walltime="00:30:00",para=para,
                       simdir=destfolder,logdir=destfolder)

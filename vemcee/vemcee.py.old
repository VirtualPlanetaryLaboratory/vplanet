#
#	To do:	Move user-editable parameters to an .ini file
#		Fix problem where parameters in list not in .in file are unchanged and no error is thrown (add instead of find and replace?)
#		Write readme file
#		
#

import numpy as np
import emcee
import os
import sys

#
#General Runtime parameters
#
nwalkers = 4 #must be even and more than 2x the number of parameters due to emcee implementation magic. With effort this can be changed.
walker_steps = 1

#
#Output parameter target Values and error scale; used later to determine quality of fit
#
target_list = ['TUMan','TCMB','HflowSurf','HflowCMB','MeltMassFlux','ViscUMan','RIC','MagMom']

ideal_TUMan = 1630; scale_TUMan = 20
ideal_TCMB = 4000; scale_TCMB = 20
ideal_HflowSurf = 39; scale_HflowSurf = 2
ideal_HflowCMB = 14; scale_HflowCMB = 1
ideal_MeltMassFlux = 1.7e6; scale_MeltMassFlux = 3e5
ideal_ViscUMan = 5.5e17; scale_ViscUMan = 4.5e17
ideal_RIC = 1221; scale_RIC = 10
ideal_MagMom = 1; scale_MagMom = 0.25

#
#input parameters and their bounds. Watch out for casing! If you misspell a parameter, you won't get an error till too late!
#
param_list = np.array(['dViscRef'])#,'dViscJumpMan','dTrefLind','dDTChiRef','dMagMomCoef'])

min_dViscRef = 5e6 ; max_dViscRef = 5e8
min_dViscJumpMan = 1 ; max_dViscJumpMan = 10
#min_dshmodref = 1e4 ; max_dshmodref = 1e7 #hardcoded 6e6 default
min_dViscMeltB = 2 ; max_dViscMeltB = 3
min_dViscMeltPhis = 0.64 ; max_dViscMeltPhis = 0.96
min_dViscMeltDelta = 4.8 ; max_dViscMeltDelta = 7.2
min_dViscMeltGamma = 4.8 ; max_dViscMeltGamma = 7.2
min_dViscMeltXi = 4 ;  max_dViscmeltXi = 6
#min_dstiffness = 1e10 ; max_dstiffness = 1e13 #hardcoded default 1e13
#min_dLind = 5e3 ; max_dLind = 8e3 #hardcoded default 7000.0*KM
min_dTrefLind = 5205 ; max_dTrefLind = 6205
min_dDTChiRef = 300 ; max_dDTChiRef = 700
#min_dAdCore = 6e3 ;  max_dAdCore = 7e3 #hardcoded default 6340.0*KM
#min_dAdJumpM2Um = 0.56 ; max_dAdJumpM2Um = 0.84 #hardcoded, check casing default 0.7
#min_dAdJumpM2Lm = 1.04 ; max_dAdJumpM2Lm = 1.56 #hardcoded, casing default 1.3
#min_dAdJumpC2CMB = 0.64 ; max_dAdJumpC2CMB = 0.96 #hardcoded, casing default 0.8
min_dMagMomCoef = 1e-2 ; max_dMagMomCoef = 1
#min_dElecCondCore = 10e5 ; max_dElecCondCore = 20e5 #hardcoded, default 10e5


#
# End of editable parameters. If you're touching anything below here, that means I've made a mistake.
#

#
#Time estimate, build matrix of initial positions for walkers, make paramater list into useful strings
#
mindev = 1e10 #Large number so if statement will work later
print "It will take about "+str((nwalkers * walker_steps)//60)+" minutes and " + str((nwalkers * walker_steps)%60)+" seconds to complete.\n"

ndim = len(param_list)
init_pos = np.ndarray(shape=(nwalkers,ndim), dtype=float, order='F')
for i in range(0,nwalkers*ndim):
	min_param_val = eval('min_'+param_list[i%ndim])
	max_param_val = eval('max_'+param_list[i%ndim])
	init_pos[i//ndim][i%ndim] = np.random.uniform(min_param_val,max_param_val)


sa_outputs = '-Time '
readfile_outputs = 'time,'
for string_iterator in range(0, len(target_list)):
	sa_outputs += '-'+target_list[string_iterator]+' '
	readfile_outputs += target_list[string_iterator]+','

mainloop_iterator = 1

#
# Short Progress bar implementation shamelessly taken from stackoverflow at: http://stackoverflow.com/questions/3173320/text-progress-bar-in-the-console
#

def progress(count, total):
    bar_len = 50
    filled_len = int(round(bar_len * count / float(total)))

    percents = round(100.0 * count / float(total), 1)
    bar = '=' * filled_len + '-' * (bar_len - filled_len)

    sys.stdout.write('[%s] %s%s\r' % (bar, percents, '%'))
    sys.stdout.flush()


#
#Determine how close the test model is to the target, x is a position vector in the n dimensional parameter space
#

def lnfit(x):

	global mindev
	global minpos
	global mainloop_iterator

	progress(mainloop_iterator,(nwalkers*(walker_steps+1)) )
	mainloop_iterator += 1

	#Create the file to be tested and run, replace name
	default = open('earth.in','r')
	default_data = default.read()
	default.close()
	os.system('cp earth.in test.in')
	test_case_1 = default_data.replace('sName		earth','sName test')
	test_case_2 = test_case_1.replace('saOutputOrder','saOutputOrder ' + sa_outputs + '#')

	#For each axis in the parameter space, replace the default input file value with the test value
	for axis in range(0,ndim):

		param = param_list[axis]
		value = str(x[axis])
		#print param, value
		min_param_val = eval('min_'+param_list[axis])
		max_param_val = eval('max_'+param_list[axis])
		
		#Return -infinity for places we don't want the walkers going.
		if (x[axis] < min_param_val) or (x[axis] > max_param_val):
			return -np.inf

		exec('test_case_'+str(axis+3)+' = test_case_'+str(axis+2)+'.replace(param+"   ",param+"   "+value+"#")' ) #2,3 due to previous replacements.
	
	f = open('test.in','w')
	f.write(eval('test_case_'+str(ndim+1)))
	f.close()
	os.system('../vplanet -q vpl.in')

	#load output file, calculate deviance using ideal value and deviation scale from above
	exec(readfile_outputs+"= np.loadtxt('sun.test.forward',dtype=float,delimiter=' ',unpack=True,usecols=(0,1,2,3,4,5,6,7,8) )")
	Dev_Tot = 0
	for target_iterator in range(0,len(target_list)):
		target = str(target_list[target_iterator])
		exec('Dev_'+target+'= ((ideal_'+target+' - '+target+'[4500]) / scale_'+target+')**2')
		Dev_Tot += eval('Dev_'+target)

	#Update the best-found scenario if necessary, this is why mindev was 1e10 to begin with, it makes the if statement work on the first try
	if (Dev_Tot < mindev):
		mindev = Dev_Tot
		minpos = x	

	return np.log(1/Dev_Tot)

#
#The actual implementation, 3 lines of code
#

sampler = emcee.EnsembleSampler(nwalkers, ndim, lnfit)
pos, prob, state = sampler.run_mcmc(init_pos, walker_steps)
sampler.reset()

with open("minima.txt", "a") as minima:
    minima.write( str(mindev)+" at "+str(param_list)+"="+str(minpos)+"\n\n")

print "\n\nThe minimum deviation found was ", mindev, "at", param_list, "=", minpos, "\n\n This has been added to the file minima.txt"

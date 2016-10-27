#
#	To do:	Clean up the earth.in file and make it so parameters are appended rather than replaced
#		Move non-userspace files to resources folder
#		Write readme file
#		Get output params from log file? Use vplot?
#		Add keyboard interrupt so you can log the best file if you stop the program early

import numpy as np
import emcee
import os #redundant with sys I think
import sys
import ConfigParser
import time

#Load config file and extract the needed variables
Config = ConfigParser.ConfigParser()
Config.optionxform = str
Config.read("vemcee.ini")

nwalkers = int(Config.get("Global","nwalkers"))
walker_steps = int(Config.get("Global","walker_steps"))
target_list = Config.options("Output")
param_list = Config.options("Input")

for input_name_iterator in Config.options("Input"):
	#build list of "min_param = x ; max_param = x" definitions
	exec("min_"+input_name_iterator+" = "+Config.get("Input",input_name_iterator).split(" ")[0])
	exec("max_"+input_name_iterator+" = "+Config.get("Input",input_name_iterator).split(" ")[1])

for output_name_iterator in Config.options("Output"):
	#build list of "ideal_param = x ; scale_param = x" definitions
	exec("ideal_"+output_name_iterator+" = "+Config.get("Output",output_name_iterator).split(" ")[0])
	exec("scale_"+output_name_iterator+" = "+Config.get("Output",output_name_iterator).split(" ")[1])
	
#Time estimate, build matrix of initial positions for walkers, make paramater list into useful strings
mindev = 1e10 #Large number so if statement will work later
start_time = time.time()
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



# Progress bar shamelessly taken from: http://stackoverflow.com/questions/3173320/text-progress-bar-in-the-console
def progress(count, total):
	bar_len = 50
	filled_len = int(round(bar_len * count / float(total)))

	percents = round(100.0 * count / float(total), 1)
	bar = '=' * filled_len + '-' * (bar_len - filled_len)
	rate = count / (time.time() - start_time)
	time_left = (total - count)/rate

	sys.stdout.write('[{0}] {1}{2} {3:.3}ops/sec with {4:.2}s left        \r'.format(bar,percents, '% ',rate,time_left))
	sys.stdout.flush()



#Determine how close the test model is to the target, x is a position vector in the n dimensional parameter space
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



#The actual implementation, 3 lines of code
sampler = emcee.EnsembleSampler(nwalkers, ndim, lnfit)
pos, prob, state = sampler.run_mcmc(init_pos, walker_steps)
sampler.reset()



#Write & save output
with open("minima.txt", "a") as minima:
    minima.write( str(mindev)+" at "+str(param_list)+"="+str(minpos)+"\n")

print "\n\nThe minimum deviation found was ", mindev, "at", param_list, "=", minpos, "\n\n This has been added to the file minima.txt"

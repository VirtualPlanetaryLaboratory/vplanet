#	To do:
#		Move non-userspace files to resources folder
#		update readme file
#		Get output params from log file? Use vplot?
#		Add keyboard interrupt so you can log the best file if you stop the program early
#		Add multi-body support, consider needs for other modules
#		Make code not linux-specific

import numpy as np, emcee, os, sys, ConfigParser, time

#How the program exits, don't move since this may be called at any time
def graceful_exit(status):
	if (status == 0 or status == 1):
		with open("minima.txt", "a") as minima:
   			minima.write( str(mindev)+" at "+str(param_list)+"="+str(minpos)+"\n")
		print "\n\nThe minimum deviation found was ", mindev, "at:\n", param_list, "=", minpos, "\n\nThis has been added to the file minima.txt"
	elif (status == 1):
		print "\nThe program exited in an unusual state, probably a keyboard interrupt. If unintentional, something has gone wrong.\n"
	elif (status == 2):
		print "The program ended abnormally and no minima were saved.\n"
	else:
		print "Not only was there an error, the error handler doesn't understand it. Sorry, and good luck.\n"
	sys.exit()



# Progress bar modified from code here: http://stackoverflow.com/questions/3173320/text-progress-bar-in-the-console
# May fail to overwrite properly if the terminal window is too small.
def progress(count, total):
	bar_len = 35
	filled_len = int(round(bar_len * count / float(total)))

	percents = round(100.0 * count / float(total), 1)
	bar = '=' * filled_len + '-' * (bar_len - filled_len)
	rate = count / (time.time() - start_time)
	time_left = (total - count)/rate

	sys.stdout.write('\r[{0}] {1}%;  {2:.2}s remaining     '.format(bar,percents,time_left))
	sys.stdout.flush()



#The main loop, determine how close the test model is to the target, x is a position vector in the n dimensional parameter space
def lnfit(x):
	global mindev
	global minpos
	global mainloop_iterator
		#Create the file to be tested and run, replace name
	in_file_data = open(in_file,'r').read()
	exec("os.system('cp '+in_file+' test.in')")
	if "sName" and "saOutputOrder" in in_file_data:
		test_case_1 = in_file_data.replace('sName','sName test #')
		test_case_2 = test_case_1.replace('saOutputOrder','saOutputOrder ' + sa_outputs + '#')
	else:
		print "\nsName or saOutputOrder missing in .in file.\n"
		graceful_exit(2)

	#Update progress bar
	progress(mainloop_iterator,(nwalkers*(walker_steps+1)) )
	mainloop_iterator += 1

	#For each axis in the parameter space, replace the default input file value with the test value
	for axis in range(0,ndim):
		param = param_list[axis]
		value = str(x[axis])
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
		with open("minima_log.txt", "a") as minima:
   			minima.write( str(mindev)+" at "+str(param_list)+"="+str(minpos)+"\n")

	return np.log(1/Dev_Tot)

###
### End of functions. Beginning of runtime.
###

#Startup tasks, time and command-line parameters
start_time = time.time()
os.system("rm minima_log.txt > /dev/null")
try:
	in_file = sys.argv[1]
except Exception:
	print "Program called incorrectly."
	graceful_exit(2)



#Load config file and extract the needed variables
Config = ConfigParser.ConfigParser()
Config.optionxform = str
Config.read("vemcee.ini")

try:
	nwalkers = int(Config.get("Global","nwalkers"))
	walker_steps = int(Config.get("Global","walker_steps"))
	if (walker_steps == 0):
		print "Walker steps cannot be 0!" #Emcee won't run, but the error it returns makes no sense, so this could save some time
		graceful_exit(2)
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
except Exception:
	print "Error in parsing the .ini file."
	graceful_exit(2)



#Build matrix of initial positions for walkers, make paramater list into useful strings
mindev = 1e10 #Large number so if statement will work later

ndim = len(param_list)
init_pos = np.ndarray(shape=(nwalkers,ndim), dtype=float, order='F')
for i in range(0,nwalkers*ndim):
	min_param_val = eval('min_'+param_list[i%ndim])
	max_param_val = eval('max_'+param_list[i%ndim])
	if (min_param_val > max_param_val):
		print param_list[i%ndim], "is messed up. Min must be less than max."
		graceful_exit(2)
	init_pos[i//ndim][i%ndim] = np.random.uniform(min_param_val,max_param_val)

sa_outputs = '-Time '
readfile_outputs = 'time,'
for string_iterator in range(0, len(target_list)):
	sa_outputs += '-'+target_list[string_iterator]+' '
	readfile_outputs += target_list[string_iterator]+','

mainloop_iterator = 1



#Actually calling emcee
sampler = emcee.EnsembleSampler(nwalkers, ndim, lnfit)
pos, prob, state = sampler.run_mcmc(init_pos, walker_steps)
sampler.reset()

graceful_exit(0)

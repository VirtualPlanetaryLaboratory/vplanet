#def findline(string,bound):
#	logfile = open("sun.log","r")
#	for i, line in enumerate(logfile,1):
#		if string in line and (i>bound):
#			return i
#
#a = findline("FINAL SYSTEM PROPERTIES",0)
#print findline("(RIC)",a)

logfile = open("sun.log","r")
for i, line in enumerate(logfile,1):
	if "FINAL SYSTEM PROPERTIES" in line:
		lower_lim = i
logfile.close()	#Weirdly, absolutely necessary. Can only perform one search per one open/close

logfile = open("sun.log","r")
for j, line2 in enumerate(logfile,1):
	if "(RIC)" in line2:
		param_line = j


print param_line

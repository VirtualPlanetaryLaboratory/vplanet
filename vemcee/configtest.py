import ConfigParser

Config = ConfigParser.ConfigParser()
Config.optionxform = str #Must be before read command
Config.read("vemcee.ini")

#for name in Config.options("Input"):
#	print name

#nwalkers = int(Config.get("Global","nwalkers"))

for name in Config.options("Input"):
	print name, float(Config.get("Input",name).split(" ")[1])

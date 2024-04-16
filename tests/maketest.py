import argparse
import glob
import os
import pathlib
import subprocess
import sys

import pandas as pd
from astropy import units as u

import vplanet
from vplanet import custom_units


def Main(dir, initial=False):
    skip_list = ["Conflicts", "__pycache__", "Help"]
    initial_list = ["HZSingle", "IoHeat"]

    if dir == "all":
        dir_list = CollectAllTests()
    else:
        dir_list = [dir]

    BuildVPLanet()
    for dirname in dir_list:
        if dirname in skip_list:
            continue
        if dirname in initial_list:
            initial = True
        if dirname not in initial_list:
            initial = False

        # Should check for trailing "/" and remove it XXX
        # grab infiles
        infiles = []
        for file in os.listdir(dirname):
            if file.endswith(".in"):
                infiles.append(file)

        # change to Dir and run vplanet
        print("Creating test " + dirname, flush=True)
        os.chdir(dirname)
        RemoveOldFiles()
        vplanet.run("vpl.in", quiet=True, C=True)

        stellar = False  # Assume first, then it can be replaced with GetSNames
        SystemName, BodyName, logfile, outfile, forward, backward, stellar = GetSNames(
            infiles
        )

        # process logfile into a dictonary
        data = {}
        data = ProcessLogFile(logfile, data, forward, backward, initial=initial)

        if initial == False:
            for body in BodyName:
                outputorder = body + ".OutputOrder"
                gridoutputorder = body + ".GridOutputOrder"

                # if output order from the log file isn't empty process it
                if outputorder in data:
                    OutputOrder = {}
                    OutputOrder[outputorder] = data[outputorder]

                    if not outfile:
                        # need to figure out if its forward file or backwards file
                        outputfile = SystemName + "." + body + ".forward"
                        if backward == True:
                            outputfile = SystemName + "." + body + ".backward"
                    else:
                        outfile = outputfile

                    data = ProcessOutputfile(outputfile, data, body, OutputOrder)

                # now process the grid output order (if it exists)
                if gridoutputorder in data:
                    GridOutputOrder = {}
                    GridOutputOrder[gridoutputorder] = data[gridoutputorder]
                    climate_name = SystemName + "." + body + ".Climate"
                    data = ProcessOutputfile(climate_name, data, body, GridOutputOrder)

        ProcessUnits(data)
        WriteTestFile(data, dirname, stellar)
        os.chdir("../../")


def BuildVPLanet():
    sys.stdout.write("Building VPLanet...")
    sys.stdout.flush()
    os.chdir("../")
    subprocess.check_output(["make", "opt"])
    print("done.", flush=True)
    os.chdir("tests")


def RemoveOldFiles():
    for file in glob.glob("*.log"):
        os.remove(file)
    for file in glob.glob("*.forward"):
        os.remove(file)
    for file in glob.glob("*.backward"):
        os.remove(file)


def CollectAllTests():
    reply = None
    question = "WARNING: Confirm that you will overwrite all tests"
    while reply not in ("y", "n"):
        reply = str(input(question + " (y/n): ")).lower().strip()

        if reply[:1] == "y":
            top_list = sorted(next(os.walk("."))[1])
            dir_list = []
            for top in top_list:
                subdirs = [
                    os.path.join(top, subdir)
                    for subdir in sorted(next(os.walk(top))[1])
                ]
                for subdir in subdirs:
                    if "pycache" not in subdir:
                        dir_list.append(subdir)
            print(" ", flush=True)
        elif reply[:1] == "n":
            print("Files NOT overwritten. Exiting.")
            exit()
        else:
            print("User input was not valid. Exiting.")
            exit()
    return dir_list


def GetSNames(bodyfiles):
    # get system and the body names
    body_names = []
    outfile = ""
    forward = False
    backward = False
    stellar = False

    for file in bodyfiles:
        # gets path to infile
        # if the infile is the primary file, then get the system name
        if file == "vpl.in":
            with open(file, "r") as vpl:
                content = [line.strip().split() for line in vpl.readlines()]
                for line in content:
                    if line:
                        if line[0] == "sSystemName":
                            system_name = line[1]
                            prefix = system_name

                            if line[0] == "sLogfile":
                                prefix = line[1]

                            if line[0] == "sOutFile":
                                outfile = line[1]

                            if line[0] == "bDoForward":
                                forward = True
                                backward = False

                            if line[0] == "bDoBackward":
                                backward = True
                                forward = False

            logfile = prefix + ".log"

        # otherwise get the sName from the body files
        else:
            with open(file, "r") as infile:
                content = [line.strip().split() for line in infile.readlines()]
                for line in content:
                    if line:
                        if line[0] == "saModules":
                            if (
                                "stellar".casefold() in line
                                or "stellar,".casefold() in line
                            ):
                                stellar = True
                                print(
                                    "WARNING: Stellar module detected, rtol will be set to 1e-4 for all values.",
                                    flush=True,
                                )
                        if line[0] == "sName":
                            body_names.append(line[1])

    return system_name, body_names, logfile, outfile, forward, backward, stellar


def ProcessLogFile(logfile, data, forward, backward, initial=False):
    prop = ""
    body = "system"
    with open(logfile, "r+", errors="ignore") as log:
        content = [line.strip() for line in log.readlines()]

    for line in content:
        if len(line) == 0:
            continue

        # First we need to get the body names and if its a inital or final value
        if line.startswith("-"):
            tmp_line = line.replace("-", "").replace(":", "").strip().replace(" ", "_")

            if tmp_line.startswith("INITIAL_SYSTEM_PROPERTIES"):
                prop = "initial"

            if tmp_line.startswith("FINAL_SYSTEM_PROPERTIES"):
                prop = "final"
                body = "system"

            if tmp_line.startswith("BODY"):
                body = tmp_line[tmp_line.find("_") + 1 :].strip()

            continue

        # if the line starts with a '(' that means its a variable we need to grab the units
        if line.startswith("("):
            if initial == True and prop == "final":
                continue
            else:
                fv_param = line[1 : line.find(")")].strip()

                # THIS IS FOR VARIABLES THAT START WITH NUMBERS CURRENTLY BUGGED FIX AT LATER DATE
                if fv_param[0].isdigit():
                    continue

                units = line[line.rfind("[") + 1 : line.rfind("]")].strip()

                if not units:
                    units = ""

                fv_value = line[line.find(":") + 1 :].strip()
                key_name = "log." + prop + "." + body + "." + fv_param

                if key_name in data:
                    data[key_name].append(fv_value)
                else:
                    data[key_name] = [units, fv_value]

            if initial == False:
                # if the name starts with output order then its a list of variables
                if line.startswith("Output Order") and len(line[line.find(":") :]) > 1:
                    parm_key = line[: line.find(":")].replace(" ", "")
                    params = line[line.find(":") + 1 :].strip().split("]")
                    key_name = body + "." + parm_key
                    out_params = []

                    for i in params:
                        var = i[: i.find("[")].strip()
                        units = i[i.find("[") + 1 :]

                        if not units:
                            units = ""

                        if var == "":
                            continue

                        out_params.append([var, units])

                        key_name_forward = body + "." + var

                        if key_name_forward not in data:
                            data[key_name_forward] = [units]

                    if key_name not in data:
                        data[key_name] = out_params

                # if the name starts with  grid output order then its a list of variables
                if (
                    line.startswith("Grid Output Order")
                    and len(line[line.find(":") :]) > 1
                ):
                    parm_key = line[: line.find(":")].replace(" ", "")
                    params = line[line.find(":") + 1 :].strip().split("]")
                    key_name = body + "." + parm_key
                    out_params = []

                    for i in params:
                        var = i[: i.find("[")].strip()
                        units = i[i.find("[") + 1 :]

                        if not units:
                            units = ""

                        if var == "":
                            continue

                        out_params.append([var, units])

                        key_name_climate = body + "." + var

                        if key_name_climate not in data:
                            data[key_name_climate] = [units]

                    if key_name not in data:
                        data[key_name] = out_params

    return data


def ProcessOutputfile(file, data, body, Output):
    header = []
    units = []
    for k, v in Output.items():
        for num in v:
            header.append(num[0])
            if num[1] == "":
                units.append("")
            else:
                units.append(num[1])

    sorted = pd.read_csv(file, header=None, delim_whitespace=True)
    sorted = sorted.to_numpy().transpose().tolist()

    for i, row in enumerate(sorted):
        key_name = body + "." + header[i]
        print(key_name)

        if key_name in data:
            data[key_name].append(row[-1])
            print(data[key_name])

        else:
            data[key_name] = [units[i], row[-1]]

    return data


def ProcessUnits(data):
    for k, v in data.items():
        if "Order" in k:
            continue

        units = v[0]
        # v[0] = str(u.Unit(v[0]))
        # print(u.Unit(units))

        ##units that need to be converted
        if units == "kg*m^2/sec^3":
            v[0] = "u.W"

        if units == "kg*m^2/sec^2":
            v[0] = "u.Joule"

        if units == "kg*m^2/sec^2/(m^2*sec)" or units == "kg*m^2/s^2/(m^2*s)":
            v[0] = "u.kg / u.sec ** 3"

        if units == "W/m^2":
            v[0] = "u.W / u.m ** 2"

        if units == "kg/s":
            v[0] = "u.kg / u.sec"

        if units == "kg*m^2/s" or units == "kg*m^2/sec":
            v[0] = "(u.kg * u.m ** 2) / u.sec"

        if units == "/sec":
            v[0] = "1 / u.sec"

        if units == "/sec^2":
            v[0] = "1 / u.sec ** 2"

        if units == "kg/m^3":
            v[0] = "u.kg / u.m ** 3"

        if units == "m/sec" or units == "m/s":
            v[0] = "u.m / u.sec"

        if units == "rad/sec":
            v[0] = "u.rad / u.sec"

        if units == "m^2/s" or units == "m^2/sec":
            v[0] = "u.m ** 2 / u.sec"

        if units == "K/s" or units == "K/sec":
            v[0] = "u.K / u.sec"

        if units == "m^2/s^3":
            v[0] = "u.m ** 2 / u.sec ** 3"

        if units == "m^-2 s^-1":
            v[0] = "1 / u.m ** 2 / u.sec"

        # regular units
        if units == "TO":
            v[0] = "u.TO"

        if units == "bars":
            v[0] = "u.bar"

        if units == "K":
            v[0] = "u.K"

        if units == "Pa":
            v[0] = "u.Pa"

        if units == "W":
            v[0] = "u.W"

        if units == "kg":
            v[0] = "u.kg"

        if units == "km":
            v[0] = "u.km"

        if units == "m":
            v[0] = "u.m"

        if units == "sec":
            v[0] = "u.sec"

        if units == "rad":
            v[0] = "u.rad"

        if units == "TW":
            v[0] = "u.TW"

        if units == "PW":
            v[0] = "u.PW"

        if units == "Gyr":
            v[0] = "u.Gyr"

        if units == "au" or units == "AU":
            v[0] = "u.au"

        if (
            units == "Rearth"
            or units == "Earthradii"
            or units == "earth radii"
            or units == "Earth Radii"
        ):
            v[0] = "u.Rearth"

        if units == "LSUN" or units == "Lsun":
            v[0] = "u.LSUN"

        if units == "Mearth" or units == "Earth Masses" or units == "earthmasses":
            v[0] = "u.Mearth"

        if units == "deg":
            v[0] = "u.deg"

        if units == "GPa":
            v[0] = "u.GPa"

        if units == "Celsius" or units == "C":
            v[0] = "u.deg_C"

        if units == "/day" or units == "/days":
            v[0] = "1 / u.day"

        if units == "day" or units == "days":
            v[0] = "u.day"

        if units == "year" or units == "years":
            v[0] = "u.yr"

        if units == "ergs":
            v[0] = "u.erg"

        if (
            units == "F/F_Earth"
            or units == "Initial Primordial Earth Number"
            or units == "EMAGMOM"
            or units == "EMAGPAUSERAD"
        ):
            v[0] = u.Unit(units)

        if units == "nd":
            v[0] = "u.nd"

    return data


def WriteTestFile(data, dirname, stellar):
    badchars = "- "
    for i in badchars:
        dirname = dirname.replace(i, "")

    # Tests are two subdirs down
    dirs = dirname.split("/")

    test_file = "test_" + dirs[1] + ".py"
    t = open(test_file, "w")
    try:
        with open(test_file, "w") as t:
            t.write("from benchmark import Benchmark, benchmark \n")
            t.write("import astropy.units as u \n")
            t.write(" \n")
            t.write("@benchmark( \n")
            t.write("   { \n")

            for k, v in data.items():
                # print(k)
                if "Order" in k or v[1] == "inf":
                    continue

                # this means its from an output file
                if "log" not in k and v[0] != "":
                    t.write(
                        '       "'
                        + k
                        + '": {"value": '
                        + str(v[1])
                        + ', "unit": '
                        + v[0]
                        + ', "index": -1 }, \n'
                    )
                if "log" not in k and v[0] == "":
                    t.write(
                        '       "'
                        + k
                        + '": {"value": '
                        + str(v[1])
                        + ', "index": [-1] }, \n'
                    )

                if "log" in k and v[0] != "":
                    # print ("log, no v[0]")
                    if "final" in k and stellar == True:
                        # print("final, no v[0]")
                        if "DeltaTime" in k:
                            print("WARNING: Skipping final DeltaTime", flush=True)
                        else:
                            t.write(
                                '       "'
                                + k
                                + '": {"value": '
                                + str(v[1])
                                + ', "unit": '
                                + v[0]
                                + ', "rtol": 1e-4}, \n'
                            )
                    else:
                        # print("not final")
                        t.write(
                            '       "'
                            + k
                            + '": {"value": '
                            + str(v[1])
                            + ', "unit": '
                            + v[0]
                            + "}, \n"
                        )
                if "log" in k and v[0] == "":
                    # print ("log, w/ v[0]")
                    if "final" in k and stellar == True:
                        # print("final, w v[0]")
                        if "DeltaTime" in k:
                            print("WARNING: Skipping final DeltaTime", flush=True)
                        else:
                            t.write(
                                '       "'
                                + k
                                + '": {"value": '
                                + str(v[1])
                                + ', "rtol": 1e-4}, \n'
                            )
                    else:
                        # print("not final")
                        t.write('       "' + k + '": {"value": ' + str(v[1]) + "}, \n")

            t.write("   } \n")
            t.write(")\n")
            t.write("class Test_" + dirs[1] + "(Benchmark): \n")
            t.write("   pass")
            t.write(" \n")
    except IOError:
        print("Unable to create file", test_file)
        exit(1)

    print("Successfuly created new test file: " + dirname + "/" + test_file, flush=True)
    print(" ", flush=True)


def Arguments():
    parser = argparse.ArgumentParser(
        description="Extract data from Vplanet simulations"
    )
    parser.add_argument("dir", help="Name of test directory")
    parser.add_argument(
        "-i",
        "--initial",
        action="store_true",
        help="Make test from initial values only",
    )

    args = parser.parse_args()

    Main(args.dir, args.initial)


if __name__ == "__main__":
    Arguments()

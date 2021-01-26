import os
import multiprocessing as mp
import sys
import subprocess as sub
import mmap
import argparse
import h5py
import numpy as np
from collections import OrderedDict
import csv
import pandas as pd
from scipy import stats


"""
Code for command line call of bigplanet
"""

def GetDir(input_file):
    """ Give it input file and returns name of folder where simulations are located. """
    infiles = []
    # gets the folder name with all the sims
    with open(input_file, 'r') as vpl:
        content = [line.strip().split() for line in vpl.readlines()]
        for line in content:
            if line:
                if line[0] == 'destfolder':
                    folder_name = line[1]

                if line[0] == 'file':
                    infiles.append(line[1])
    if folder_name is None:
        raise IOError("Name of destination folder not provided in file '%s'."
                      "Use syntax 'destfolder <foldername>'"%inputf)


    if os.path.isdir(folder_name) == False:
        print("ERROR: Folder", folder_name, "does not exist in the current directory.")
        exit()

    return folder_name, infiles

def GetSims(folder_name):
    """ Pass it folder name where simulations are and returns list of simulation folders. """
    #gets the list of sims
    sims = sorted([f.path for f in os.scandir(folder_name) if f.is_dir()])
    return sims

def parallel_run_planet(input_file, cores,quiet):
    # gets the folder name with all the sims
    folder_name, infiles = GetDir(input_file)
    #gets the list of sims
    sims = GetSims(folder_name)
    #initalizes the checkpoint file
    checkpoint_file = os.getcwd() + '/' + '.' + folder_name + '_hdf5'

    #checks if the files doesn't exist and if so then it creates it
    if os.path.isfile(checkpoint_file) == False:
        with open(checkpoint_file,'w') as cp:
            cp.write('Vspace File: ' + os.getcwd() + '/' + input_file + '\n')
            cp.write('Total Number of Simulations: '+ str(len(sims)) + '\n')
            for f in range(len(sims)):
                cp.write(sims[f] + " " + "-1 \n")
            cp.write('THE END \n')

    #if it does exist, it checks for any 0's (sims that didn't complete) and
    #changes them to -1 to be re-ran
    else:
        sub.run(['rm', checkpoint_file])
        #restarts the checkpoint file from scratch
        with open(checkpoint_file,'w') as cp:
            cp.write('Vspace File: ' + os.getcwd() + '/' + input_file + '\n')
            cp.write('Total Number of Simulations: '+ str(len(sims)) + '\n')
            for f in range(len(sims)):
                cp.write(sims[f] + " " + "-1 \n")
            cp.write('THE END \n')

        datalist = []

        with open(checkpoint_file, 'r') as f:
            for newline in f:
                datalist.append(newline.strip().split())

        for i in range(len(sims)):
            sim_dir = sims[i]
            vp = os.path.join(sim_dir, 'vpl.in')
            with open(vp, 'r') as vpl:
                content = [line.strip().split() for line in vpl.readlines()]
                for line in content:
                    if line:
                        if line[0] == 'dStopTime':
                            stoptime = line[1]
                        if line[0] == 'dOutputTime':
                            ouputtime = line[1]
            expected_line_num = float(stoptime)/float(ouputtime)

            fd = [f for f in os.listdir(sim_dir) if f.endswith('foward')]
            if len(fd) == 0:
                for l in datalist:
                    if l[0] == sim_dir:
                        l[1] = '-1'
            else:
                fd = os.path.join(sim_dir, fd[0])
                with open(fd, 'r') as f:
                    buf = mmap.mmap(f.fileno(),0)
                    lines = 0
                    readline = buf.readline
                    while readline():
                        lines += 1
                    last = readline[-1]
                    if last == stoptime and lines == expected_line_num:
                        for l in datalist:
                            if l[0] == sim_dir:
                                l[1] = '1'
                    else:
                        for l in datalist:
                            if l[0] == sim_dir:
                                l[1] = '-1'

        with open(checkpoint_file, 'w') as f:
            for newline in datalist:
                f.writelines(' '.join(newline)+'\n')

    #get logfile name
    path_vpl = os.path.join(sims[0],'vpl.in')
    with open(path_vpl, 'r') as vpl:
        content = [line.strip().split() for line in vpl.readlines()]

        for line in content:
            if line:
                if line[0] == 'sSystemName':
                    system_name = line[1]

    logfile = system_name + ".log"

    lock = mp.Lock()
    workers = []
    for i in range(cores):
        workers.append(mp.Process(target=par_worker, args=(checkpoint_file,infiles,system_name,logfile,quiet,lock)))
    for w in workers:
        w.start()
    for w in workers:
        w.join()

    CreateMasterHDF5(folder_name,sims)

def ProcessLogFile(logfile, data):
    prop = ''
    body = 'system'

    with open(logfile, 'r') as log:
        content = [line.strip() for line in log.readlines()]

    for line in content:

        if len(line) == 0:
            continue

        if line.startswith('-'):
            tmp_line = line.replace('-', '').replace(':', '').strip().replace(' ', '_')

            if tmp_line.startswith('INITIAL_SYSTEM_PROPERTIES'):
                prop = 'initial'

            if tmp_line.startswith('FINAL_SYSTEM_PROPERTIES'):
                prop = 'final'
                body = 'system'

            if tmp_line.startswith('BODY'):
                body = tmp_line[tmp_line.find('_')+1:].strip()

            continue

        if line.startswith('('):
            fv_param = line[1:line.find(')')].strip()
            units = line[line.find('[')+1:line.find(']')].strip()

            if not units:
                units = 'nd'

            fv_value = line[line.find(':')+1:].strip()
            key_name = body + '_' +  fv_param + '_' + prop

            if key_name in data:
                data[key_name].append(fv_value)

            else:
                data[key_name] = [units, fv_value]

        if line.startswith('Output Order') and len(line[line.find(':'):]) > 1:
            parm_key = line[:line.find(':')].replace(' ', '')
            params = line[line.find(':') + 1:].strip().split(']')
            key_name = body + '_' + parm_key
            out_params = []

            for i in params:
                var = i[:i.find('[')].strip()
                units = i[i.find('[') + 1:]

                if not units:
                    units = 'nd'

                if var == '':
                    continue
                out_params.append([var, units])

                key_name_forward = body + '_' + var + '_forward'

                if key_name_forward not in data:
                    data[key_name_forward] = [units]

            if key_name not in data:
                data[key_name] = out_params

        if line.startswith('Grid Output Order') and len(line[line.find(':'):]) > 1:
            parm_key = line[:line.find(':')].replace(' ', '')
            params = line[line.find(':') + 1:].strip().split(']')
            key_name = body + '_' + parm_key
            out_params = []

            for i in params:
                var = i[:i.find('[')].strip()
                units = i[i.find('[') + 1:]

                if not units:
                    units = 'nd'

                if var == '':
                    continue

                out_params.append([var, units])

                key_name_climate = body + '_' + var + '_climate'

                if key_name_climate not in data:
                    data[key_name_climate] = [units]

            if key_name not in data:
                data[key_name] = out_params

    return data

def ProcessInfile(infile, file, data):
    with open(infile, 'r') as inf:
        content = [line.strip() for line in inf.readlines()]
        cont = False

        for line in content:

            if len(line) == 0 or line.startswith('#'):
                continue

            tmp_line = line[:line.find('#')].strip()

            if cont:
                fv_value = fv_value.append(tmp_line.split())
                cont = False
                continue

            fv_param = tmp_line.split()[0]
            fv_value = tmp_line.split()[1:]

            if (fv_param == 'saOutputOrder' or fv_param == 'saGridOutput') and fv_value[-1] == '$':
                cont = True

def ProcessForwardfile(forwardfile, data, body, OutputOrder):

    header = []

    for i in OutputOrder:
        header.append([i][0][0])

    with open(forwardfile, 'r') as f:
        content = [line.strip().split() for line in f.readlines()]

        for row in content:
             sorted = [[content[j][i] for j in range(len(content))] for i in range(len(content[0]))]

        for i,row in enumerate(sorted):
            key_name = body + '_' + header[i] + '_forward'
            data[key_name].append(row)

    return data

def ProcessClimatefile(climatefile, data, body, GridOutputOrder):

    header = []

    for i in GridOutputOrder:
        header.append([i][0][0])


    with open(climatefile, 'r') as f:
        content = [line.strip().split() for line in f.readlines()]

        for row in content:
             sorted = [[content[j][i] for j in range(len(content))] for i in range(len(content[0]))]

        for i,row in enumerate(sorted):
            key_name = body + '_' + header[i] + '_climate'
            data[key_name].append(row)

    return data

def ProcessSeasonalClimatefile(prefix, data, body, name):
    file = list(csv.reader(open('SeasonalClimateFiles/' + prefix + '.' + name + '.0')))
    key_name = body + '_' + name
    if key_name in data:
        data[key_name].append(file)
    else:
        data[key_name] = [file]
    return data

def CreateHDF5(data, system_name, infiles, logfile, quiet, h5filename):
    """
    ....
    """
    if quiet == False:
        print('Creating',h5filename)

    data = ProcessLogFile(logfile, data)
    for file in infiles:
        if file == 'vpl.in':
            continue
        else:
            body = file.split('.')[0]
            outputorder = body + "_OutputOrder"
            gridoutputorder = body + "_GridOutputOrder"

            if outputorder in data:
                OutputOrder = data[outputorder]
                forward_name = system_name + '.' + body + '.forward'
                data = ProcessForwardfile(forward_name, data, body, OutputOrder)


            if gridoutputorder in data:
                GridOutputOrder = data[gridoutputorder]
                climate_name = system_name + '.' + body + '.Climate'
                data = ProcessClimatefile(climate_name, data, body, GridOutputOrder)
                prefix = system_name + '.' + body
                name = ['DailyInsol','PlanckB','SeasonalDivF','SeasonalFIn',
                        'SeasonalFMerid','SeasonalFOut','SeasonalIceBalance',
                        'SeasonalTemp']
                for i in range(len(name)):
                    data = ProcessSeasonalClimatefile(prefix,data,body,name[i])

    with h5py.File(h5filename, 'w') as h:
        for k, v in data.items():
            #print("Key:",k)
            #print("Length of Value:",len(v))
            if len(v) == 2:
                v_attr = v[0]
                v_value = [v[1]]

            else:
                v_value = v[0]
                v_attr = ''
            #print("Units:",v_attr)
            #print()
            h.create_dataset(k, data=np.array(v_value,dtype='S'),compression = 'gzip')
            h[k].attrs['Units'] = v_attr

def merge_data(data_list):

    """Merge dictionaries with data.

    Keyword arguments:
    data_list -- the dictionary with data dictionaries
    """

    data = None

    for f in data_list:
        if not data:
            data = data_list[f]
        else:
            for key in data_list[f]:
                data[key] = np.append(data[key], data_list[f][key], axis=0)


    return data

def load(filename):

    """Load hdf5 file to data dictionary and return it.

    Keyword arguments:
    filename -- the full path to hdf5 file
    """

    with h5py.File(filename, 'r') as f:

        data = {}

        for key in f:
            data[key] = f[key][...]

    return data

def save(filename, data):

    """Create hdf5 file with given data.

    Keyword arguments:
    filename -- the full path to hdf5 file
    data -- dictionary with data
    """

    with h5py.File(filename, 'w') as f:

        for key in data:
            f.create_dataset(key, data[key].shape, dtype=data[key].dtype,compression='gzip')[...] = data[key]

## parallel worker to run vplanet ##
def par_worker(checkpoint_file,infiles,system_name,logfile,quiet,lock):

    while True:

        lock.acquire()
        datalist = []
        data = {}

        with open(checkpoint_file, 'r') as f:
            for newline in f:
                datalist.append(newline.strip().split())

        folder = ''

        for l in datalist:
            if l[1] == '-1':
                folder = l[0]
                l[1] = '0'
                break

        if not folder:
            lock.release()
            return

        with open(checkpoint_file, 'w') as f:
            for newline in datalist:
                f.writelines(' '.join(newline)+'\n')

        lock.release()

        os.chdir(folder)

        lock.acquire()
        datalist = []

        with open(checkpoint_file, 'r') as f:
            for newline in f:
                datalist.append(newline.strip().split())

        single_folder = folder.split('/')[-1]
        HDF5_File = single_folder + '.hdf5'

        if os.path.isfile(HDF5_File) == False:

            CreateHDF5(data, system_name, infiles, logfile, quiet, HDF5_File)
            for l in datalist:
                if l[0] == folder:
                    l[1] = '1'
                    break
        else:
            for l in datalist:
                if l[0] == folder:
                    l[1] = '1'
                    break

        with open(checkpoint_file, 'w') as f:
            for newline in datalist:
                f.writelines(' '.join(newline)+'\n')


        lock.release()

        os.chdir("../../")

def CreateMasterHDF5(folder_name, sims):
    master_file = folder_name + '.hdf5'
    filelist = []
    for i in sims:
        single_folder = i.split('/')[-1]
        HDF5_File = single_folder + '.hdf5'
        HDF5_path = i + '/' + HDF5_File
        filelist.append(HDF5_path)

    data = OrderedDict()

    for f in filelist:
        data[f] = load(f)

    save(master_file, merge_data(data))

    i = filelist[0]

    with h5py.File(i,'r') as filename:
        with h5py.File(master_file,'r+') as master_W:
            for k in filename.keys():
                units = filename[k].attrs.get('Units')
                master_W[k].attrs['Units'] = units

    for i in filelist:
        sub.run(['rm', i])
    sub.run(['rm','.' + folder_name + '_hdf5'])


"""
Code for Bigplanet Module
"""

def HDF5File(hf):
    return h5py.File(hf,'r')

def PrintKeys(hf):
    """
    Print all the keys names in the HDF5 file

    Parameters
    ----------
    hf : File
        The HDF5 where the data is stored.
        Example:
            HDF5_File = h5py.File(filename, 'r')

    """

    for k in hf.keys():
        print("Key:", k)

        body = k.split("_")[0]
        variable = k.split("_")[1]

        if variable == "OutputOrder" or variable == "GridOutputOrder":
            continue
        else:
            aggregation = k.split("_")[2]

            if aggregation == 'forward' or aggregation =='climate':
                print("Key: " + body + '_'+ variable + '_min')
                print("Key: " + body + '_'+ variable + '_max')
                print("Key: " + body + '_'+ variable + '_mean')
                print("Key: " + body + '_'+ variable + '_mode')
                print("Key: " + body + '_'+ variable + '_geomean')
                print("Key: " + body + '_'+ variable + '_stddev')

def ExtractColumn(hf,k):
    """
    Returns all the data for a single key (column) in a given HDF5 file.

    Parameters
    ----------
    hf : File
        The HDF5 where the data is stored.
        Example:
            HDF5_File = h5py.File(filename, 'r')
    k : str
        the name of the column that is to be extracted
        Example:
            k = 'earth_Obliquity_final'
        The syntax of the column names is body_variable_aggregation
        the lists of aggregations (and how to call them) is as follows:

            forward file data (forward), initial data (initial),
            final data (final), Output Order List (OutputOrder)

            Note: The following statistics only will work with forward data
            mean (mean), mode (mode), standard deviation (stddev),
            min (min), max (max), gemoetric mean (geomean)

    Returns
    -------
    data : np.array
        A numpy array of the column of table of values


    """

    body = k.split("_")[0]
    var = k.split("_")[1]

    if var == 'OutputOrder' or var == 'GridOutputOrder':
        d = hf.get(k)
        data = [v.decode() for d in hf[k] for v in d]
        shape = hf[k].shape
        data = np.reshape(data,(shape))
    else:
        aggreg = k.split("_")[2]

        if aggreg == 'forward':
            d = hf.get(k)
            data = HFD5Decoder(hf,k)

        elif aggreg == 'mean':
            data = ArgumentParser(hf,k)
            data = np.mean(data, axis=1)

        elif aggreg == 'stddev':
            data = ArgumentParser(hf,k)
            data = np.std(data, axis=1)

        elif aggreg == 'min':
            data = ArgumentParser(hf,k)
            data = np.amin(data,axis=1)

        elif aggreg == 'max':
            data = ArgumentParser(hf,k)
            data = np.amax(data,axis=1)

        elif aggreg == 'mode':
            data = ArgumentParser(hf,k)
            data = stats.mode(data)
            data = data[0]


        elif aggreg == 'geomean':
            data = ArgumentParser(hf,k)
            data = stats.gmean(data, axis=1)


        elif aggreg == 'rms':
            data = ArgumentParser(hf,k)
            #Calculate root mean squared here?

        elif aggreg == 'initial' or aggreg == 'final':
            data = [d.decode() for d in hf[k]]
            data = [float(i) for i in data]

        else:
            print('ERROR: Uknown aggregation option: ', aggreg)
            exit()

    return data

def ExtractUnits(hf,k):
    """
    Returns all the data for a single key (column) in a given HDF5 file.

    Parameters
    ----------
    hf : File
        The HDF5 where the data is stored.
        Example:
            HDF5_File = h5py.File(filename, 'r')
    k : str
        the name of the column that is to be extracted
        Example:
            k = 'earth_Obliquity_final'
        The syntax of the column names is body_variable_aggregation
        the lists of aggregations (and how to call them) is as follows:

            forward file data (forward), initial data (initial),
            final data (final), Output Order List (OutputOrder)

            Note: The following statistics only will work with forward data
            mean (mean), mode (mode), standard deviation (stddev),
            min (min), max (max), gemoetric mean (geomean)

    Returns
    -------
    units : string
        A string value of the units
    """

    return hf[k].attrs.get('Units')

def ArgumentParser(hf,k):
    forward = k.rpartition('_')[0] + '_forward'
    data = HFD5Decoder(hf,forward)
    return(data)

def HFD5Decoder(hf,k):
    data = [float(v.decode()) for d in hf[k] for v in d]
    shape = hf[k].shape
    data = np.reshape(data,(shape))
    return data

def ExtractUniqueValues(hf,k):
    """
    Extracts unique values from a key in an HDF5 file.
    Returns a numpy array of the dataset

    Parameters
    ----------
    HDF5 : File
        The HDF5 where the data is stored
        Example:
            HDF5_File = h5py.File(filename, 'r')
    key : str
        the name of the column that you want unique values from
        Example:
            key = 'earth_Obliquity_final'

    Returns
    -------
    unique : np.array
        A numpy array of the unique values in key


    """

    if len(hf[k].shape) != 1:
        data = HFD5Decoder(hf,k)
        data.flatten()
    else:
        data = [d.decode() for d in hf[k]]
        del data[0]
        data = [float(i) for i in data]

    unique = np.unique(data)
    return unique

def CreateMatrix(xaxis,yaxis,zarray, orientation=1):
    """
    Creates a Matrix for Contour Plotting of Data. Run ExtractUniqueValue()
    prior to CreateMatrix() to get the ticks for the xaxis and yaxis

    Parameters
    ----------
    xaxis : nump array
        the numpy array of unique values of the xaxis
        Example:
            xasis = ExtractUniqueValues(data,'earth_Obliquity_forward')
    yaxis : numpy array
        the numpy array of unique values of the xaxis
        Example:
            yaxis = ExtractUniqueValues(data,'earth_Instellation_final')
    zarray : numpy array
        the numpy array of the values of z for Contour Plotting
        Example:
            zarray = ExtractColumn(data,'earth_IceBeltLand_final')

    Returns
    -------
    zmatrix : numpy array
        zarray in the shape of (xaxis,yaxis)
    """

    xnum = len(xaxis)
    ynum = len(yaxis)

    if xnum * ynum != len(zarray):
        print("ERROR: Cannot reshape",zarray,"into shape("+xnum+","+ynum+")")

    zmatrix = np.reshape(zarray, (ynum, xnum))
    zmatrix = np.flipud(zmatrix)

    for i in range(0,orientation):
        zmatrix = rotate90Clockwise(zmatrix)

    zmatrix = np.flipud(zmatrix)

    return zmatrix

def rotate90Clockwise(A):
    N = len(A[0])
    for i in range(N // 2):
        for j in range(i, N - i - 1):
            temp = A[i][j]
            A[i][j] = A[N - 1 - j][i]
            A[N - 1 - j][i] = A[N - 1 - i][N - 1 - j]
            A[N - 1 - i][N - 1 - j] = A[j][N - 1 - i]
            A[j][N - 1 - i] = temp

    return A

def PrintDictData(data):
    """Proceses through dictionary and prints keys and first 6 values."""

    for k, v in data.items():

        print(k, ':', v[:])
        print()

def WriteOutput(inputfile, columns,file="bigplanet.out",delim=" ",header=False,ulysses=False):
    """
    Writes an Output file in csv format

    Parameters
    ----------
    input file : HDF5 file
        the HDF5 file where the data is stored
        Example:
            HDF5_File = h5py.File(filename, 'r')
    columns : list of strings
        a list of variables that are to be written to the csv file
        Example:
            columns = ['earth_Obliquity_final','earth_Instellation_final']
    file : string
        the name of the output file
        Default is Bigplanet.out
        Example:
            file="bigplanet.out"
    delim : string
        the delimiter for the output file
        Example:
            delim = ","
    header : boolean
        if True, headers are put on the first line of the output
        Default is False
    ulysses : boolean
        True/False boolean determing if the output file will be in VR Ulysses format
        If True, the output file will have headers, and be named 'User.csv'
    """
    export = []
    units = []
    for i in columns:
        export.append(ExtractColumn(inputfile,i))
        units.append(ExtractUnits(inputfile,i))

    export = np.array(export)

    if ulysses == True:
        delim = ','
        header = True
        file = 'User.csv'

    if delim == "":
        print('ERROR: Delimiter cannot be empty')
        exit()

    with open(file, "w", newline="") as f:
        if header == True:
            for count,i in enumerate(columns):
                f.write(i + '[' + units[count] + ']')
                if columns[-1] != i:
                    f.write(delim)

            f.write("\n")

        icol, irow = export.shape
        for i in range(irow):
            for j in range(icol):
                f.write(str(export[j][i]))
                if j < icol - 1:
                    f.write(delim)
            f.write("\n")

def CreateHDF5File(InputFile):
    max_cores = mp.cpu_count()
    parallel_run_planet(InputFile,max_cores)

def main():
    max_cores = mp.cpu_count()
    parser = argparse.ArgumentParser(description="Extract data from Vplanet simulations")
    parser.add_argument("-c","--cores", type=int, default=max_cores, help="Number of processors used")
    parser.add_argument("-q","--quiet", action="store_true", help="no output for bigplanet")
    parser.add_argument("InputFile", help="Name of the vspace input file")

    args = parser.parse_args()

    parallel_run_planet(args.InputFile, args.cores, args.quiet)



if __name__ == "__main__":

    main()

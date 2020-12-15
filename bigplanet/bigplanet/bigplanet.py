import numpy as np
import os
import sys
import pandas as pd
import h5py
import pickle
import multiprocessing as mp
import argparse
from mpi4py import MPI
import hdfdict

__all__ = ["PrintKeys",
           "ExtractColumn",
           "ExtractUniqueValues",
           "CreateMatrix"]

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

    print(infiles)
    print(folder_name)
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

def GetData(sims, cores, infiles):
    """ """

    #get logfile name
    path_vpl = os.path.join(sims[0],'vpl.in')
    with open(path_vpl, 'r') as vpl:
        content = [line.strip().split() for line in vpl.readlines()]

        for line in content:
            if line:
                if line[0] == 'sSystemName':
                    logfile_Name = line[1]

    logfile = logfile_Name + ".log"

    log_data = {}

    print(sims)

    # Process the *.log file
    for i in range(len(sims)):
        path_log = os.path.join(sims[i], logfile)
        log_data = ProcessLogFile(path_log, log_data)

        # Process all the *.in files
        # for file in os.listdir(sims[i]):
        #     if file.endswith('.in'):
        #         path_in = os.path.join(sims[i], file)
        #         ProcessInfile(path_in, file, in_data)
        #         # print(os.path.join(sims[i], file)


        for file in infiles:
            if file == 'vpl.in':
                continue
            else:
                body = file.split('.')[0]
                output = body + "_OutputOrder"
                if output in log_data:
                    OutputOrder = log_data[output]
                    forward_name = logfile_Name + '.' + body + '.forward'
                    path_forward = os.path.join(sims[i],forward_name)
                    print('Forward File: ',path_forward)
                    log_data = ProcessForwardfile(path_forward, log_data, body, OutputOrder)

    PrintDictData(log_data)
    return log_data

def ProcessLogFile(logfile, data):
    print('Log file: ', logfile)
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

        if (line.startswith('Output Order') or line.startswith('Grid Output Order')) and len(line[line.find(':'):]) > 1:
            # print(body)

            parm_key = line[:line.find(':')].replace(' ', '')
            params = line[line.find(':') + 1:].strip().split()
            key_name = body + '_' + parm_key
            out_params = []

            for i in params:
                var = i[:i.find('[')]
                units = i[i.find('[') + 1:i.find(']')].strip()

                if not units:
                    units = 'nd'

                out_params.append([var, units])

            if key_name not in data:
                data[key_name] = out_params

    return data

def PrintDictData(data):
    """Proceses through dictionary and prints keys and first 6 values."""

    for k, v in data.items():

        if 'OutputOrder' in k:
            print(k, ':', v[:])
        else:
            print(k, ':', v[0])
            print(v[1:])

        print()

def ProcessInfile(infile, file, data):
    with open(infile, 'r') as inf:
        content = [line.strip() for line in inf.readlines()]
        cont = False

        for line in content:

            if len(line) == 0 or line.startswith('#'):
                continue

            tmp_line = line[:line.find('#')].strip()
            #print(tmp_line)

            if cont:
                fv_value = fv_value.append(tmp_line.split())
                cont = False
                continue

            fv_param = tmp_line.split()[0]
            fv_value = tmp_line.split()[1:]
            # print(file, fv_param, fv_value)

            if (fv_param == 'saOutputOrder' or fv_param == 'saGridOutput') and fv_value[-1] == '$':
                cont = True

    #get the .in files data
    #if .in files have any overlap ignore duplicates
    #get .foward name
    #get .foward data
    #if .foward files have any overlap ignore orginal

    #return data

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

            if key_name in data:
                data[key_name].append(row)

            else:
                data[key_name] = [row]

    return data

def CreateHDF5(dic, h5filename, cores):
    """
    ....
    """
    with h5py.File(h5filename, 'w') as h5file:
        recursively_save_dict_contents_to_group(h5file, '/', dic)

def recursively_save_dict_contents_to_group(h5file, path, dic):
    """
    ....
    """
    for key, item in dic.items():
        if isinstance(item, dict):
            recursively_save_dict_contents_to_group(h5file, path + key + '/', item)
        else:
            h5file[path + key] = item

"""
Code for Bigplanet Module
"""

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
    print()

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
        The syntax of the column names is body_variable_saggregation
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

    body = k.split('_')[0]
    variable = k.split('_')[1]
    aggreg = k.split('_')[2]

    if aggreg == 'forward':
        d = hf.get(k)
        data = HFD5Decoder(hf,k)

    elif aggreg == 'OutputOrder':
        d = hf.get(k)
        data = [v.decode() for d in hf[k] for v in d]
        shape = hf[k].shape
        data = np.reshape(data,(shape))

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


    elif aggreg == '_mode':
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
        del data[0]
        data = [float(i) for i in data]

    else:
        print('ERROR: Uknown aggregation option: ', aggreg)
        exit()

    return data

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

def CreateMatrix(xaxis,yaxis,zarray):
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

    zmatrix = np.reshape(zarray, (xnum, ynum))
    zmatrix = np.flipud(zmatrix)
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



if __name__ == "__main__":

    max_cores = mp.cpu_count()
    parser = argparse.ArgumentParser(description="Extract data from Vplanet simulations")
    parser.add_argument("inputfile", help="Name of the vspace file")
    parser.add_argument("-c","--cores", type=int,default=max_cores,help="Number of processors used")
    args = parser.parse_args()


    dir, infiles = GetDir(args.inputfile)
    sims = GetSims(dir)
    full_data = GetData(sims, args.cores, infiles)

    h5filename = dir + '.hdf5'

    CreateHDF5(full_data,h5filename,args.cores)

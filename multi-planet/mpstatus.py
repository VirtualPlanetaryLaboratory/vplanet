import os
import sys

def mpstatus(input_file):

    with open(input_file, 'r') as vsf:
        vspace_all = vsf.readlines()
        dest_line = vspace_all[1]
        folder_name = dest_line.strip().split(None, 1)[1]
        if folder_name is None:
            raise IOError("Name of destination folder not provided in file '%s'. Use syntax 'destfolder <foldername>'"%inputf)

    count_done = 0
    count_todo = 0
    count_ip = 0
    checkpoint_file = os.getcwd() + '/' + '.' + folder_name
    if os.path.isfile(checkpoint_file) == False:
        raise Exception("Multi-Planet must be running prior to using mpstatus")
    else:
        with open(checkpoint_file,'r') as cp:
            content = [line.strip().split() for line in cp.readlines()]
            for number,line in enumerate(content):
                status = line[1]

                if status == '1':
                    count_done+= 1
                elif status == '-1':
                    count_todo+= 1
                elif status == '0':
                    count_ip+= 1

        print('--Multi-Planet Status--')
        print('Number of Simulations completed: ' + str(count_done))
        print('Number of Simulations in progress: '+ str(count_ip))
        print('Number of Simulations remaining: ' + str(count_todo))


mpstatus(sys.argv[1])

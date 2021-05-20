import subprocess as sub
import os
import glob

#gets the list of examples
example_list = sorted(next(os.walk('../examples'))[1])

#list of examples that should not be ran in the test
no_fly_list = ['AbioticO2', 'ChaoticResonances', 'DampedCBP',"TidalEarth", '__pycache__']


for i in example_list:
    if i in no_fly_list:
        continue
    else:
        foldername = '../examples/' + i + '/'
        os.chdir(foldername)
        sub.run(['python', 'makeplot.py'])
        for file in glob.glob("*.pdf"):
            if os.path.isfile(file) == False:
                assert False
            else:
                file_size = os.stat(file)
                if (file_size.st_size > 0):
                    assert True
                else:
                    assert False

        os.chdir('../../docs')

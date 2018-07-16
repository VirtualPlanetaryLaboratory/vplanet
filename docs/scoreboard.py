"""Figure out who needs to work on their docs!"""
import glob
import os
import numpy as np
import shutil

authors = {'distres': 'Russell Deitrick',
           'thermint': 'Peter Driscoll',
           'radheat': 'Rory Barnes',
           'vplanet': 'Rory Barnes',
           'poise': 'Russell Deitrick',
           'eqtide': 'Rory Barnes',
           'galhabit': 'Russel Deitrick',
           'options': 'Rory Barnes',
           'body': 'Rory Barnes',
           'output': 'Rory Barnes',
           'distorb': 'Russell Deitrick',
           'stellar': 'Rodrigo Luger',
           'spinbody': 'Hayden Smotherman',
           'binary': 'David Fleming',
           'distrot': 'Russell Deitrick',
           'flare': 'Rory Barnes',
           'module': 'Rory Barnes',
           'control': 'Rory Barnes',
           'verify': 'Rory Barnes',
           'halt': 'Rory Barnes',
           'system': 'Rory Barnes',
           'evolve': 'Rory Barnes',
           'update': 'Rory Barnes',
           'atmesc': 'Rodrigo Luger'}


def row(module, errors, parity='even'):
    return '<tr class="row-%s"><td>%s</td><td>%s</td><td>%d</td></tr>' % \
        (parity,
         "<a href='src/%s.html'>%s</a>" % (module, module),
         authors.get(module, '???'), errors)


# Get all the module names
modules = np.array([os.path.basename(x)[:-2] for x in glob.glob("../src/*.c")])
errors = np.array([0 for module in modules])

# Open the log
with open("docs.log", "r") as f:
    log = f.read()
shutil.copyfile("docs.log", ".build/html/docs.log")

# Count number of errors per module
for i, module in enumerate(modules):
    errors[i] = log.count(module + ".c") + log.count(module + ".h")

# Sort by number of errors
idx = np.argsort(errors)[::-1]
errors = errors[idx]
modules = modules[idx]

# Generate the HTML table
table = ['<table border="1" class="docutils">',
         '<colgroup>',
         '<col width="40%" />',
         '<col width="40%" />',
         '<col width="20%" />',
         '</colgroup>',
         '<tbody valign="top">',
         '<tr class="row-odd">',
         '<td><span style="font-weight: bold;">Module</span></td>',
         '<td><span style="font-weight: bold;">Author</span></td>',
         '<td><span style="font-weight: bold;">Errors</span></td>',
         '</tr>']
parity = 'even'
for module, err in zip(modules, errors):
    table.append(row(module, err, parity))
    if parity == 'even':
        parity = 'odd'
    else:
        parity = 'even'
table.append('</tbody></table>')

header = ['Scoreboard for documentation errors. If you are near the top ' +
          'of this list, you probably have a lot of commenting to do! ' +
          'The docs error log can be viewed <a href="docs.log">here</a>.' +
          '<br><br>']
with open(".build/html/scoreboard.html", "r") as html:
    lines = html.readlines()
    for i, line in enumerate(lines):
        if "<h1>Docs Scoreboard" in line:
            top = lines[:i + 1]
            bottom = lines[i + 1:]
            lines = top + header + table + bottom

with open(".build/html/scoreboard.html", "w") as html:
    for line in lines:
        print(line, file=html)

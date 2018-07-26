"""Figure out who needs to work on their docs!"""
import glob
import os
import numpy as np
import shutil

# Module-author keys
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

# Username-errors-author keys
users = {'rdeitrick': 'Russell Deitrick',
         'dflemin3': 'David Fleming',
         'RoryBarnes': 'Rory Barnes',
         'rodluger': 'Rodrigo Luger',
         'smotherh': 'Hayden Smotherman',
         'peteredriscoll': 'Peter Driscoll',
         'dm1681': 'Diego McDonald',
         'trquinn': 'Tom Quinn'}


def todo_row(name, errors, parity='even'):
    return '<tr class="row-%s"><td>%s</td><td>%d</td></tr>' % \
        (parity, name, errors)


def docs_row(module, errors, parity='even'):
    return '<tr class="row-%s"><td>%s</td><td>%s</td><td>%d</td></tr>' % \
        (parity,
         "<a href='src/%s.html'>%s</a>" % (module, module),
         authors.get(module, '???'), errors)


# -- TODOs --

# Open the todo page
with open(".build/html/todo.html", "r") as f:
    todo = f.read()

# Count number of errors per user
names = []
errors = []
for user, name in users.items():
    names.append(name)
    errors.append(todo.count(user))

# Sort by number of errors
idx = np.argsort(errors)[::-1]
errors = np.array(errors)[idx]
names = np.array(names)[idx]

# Generate the HTML table
table1 = ['<table border="1" class="docutils">',
          '<colgroup>',
          '<col width="60%" />',
          '<col width="40%" />',
          '</colgroup>',
          '<tbody valign="top">',
          '<tr class="row-odd">',
          '<td><span style="font-weight: bold;">Author</span></td>',
          '<td><span style="font-weight: bold;">To-Do Errors</span></td>',
          '</tr>']
parity = 'even'
for name, err in zip(names, errors):
    table1.append(todo_row(name, err, parity))
    if parity == 'even':
        parity = 'odd'
    else:
        parity = 'even'
table1.append('</tbody></table>')

# The header
header1 = ['<h2>To-Do Scoreboard</h2>' +
           'Scoreboard for <a href="todo.html">to-do items</a>. If you are ' +
           'near the top ' +
           'of this list, you have a lot of work to do! ' +
           '<br><br>']

# -- DOCS --

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
table2 = ['<table border="1" class="docutils">',
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
    table2.append(docs_row(module, err, parity))
    if parity == 'even':
        parity = 'odd'
    else:
        parity = 'even'
table2.append('</tbody></table>')

# The header
header2 = ['<h2>Documentation Scoreboard</h2>' +
           'Scoreboard for documentation errors. If you are near the top ' +
           'of this list, you probably have a lot of commenting to do! ' +
           'The docs error log can be viewed <a href="docs.log">here</a>.' +
           '<br><br>']

# Add to the html page
with open(".build/html/scoreboard.html", "r") as html:
    lines = html.readlines()
    for i, line in enumerate(lines):
        if "<h1>Scoreboards" in line:
            top = lines[:i + 1]
            bottom = lines[i + 1:]
            lines = top + header1 + table1 + header2 + table2 + bottom

with open(".build/html/scoreboard.html", "w") as html:
    for line in lines:
        print(line, file=html)

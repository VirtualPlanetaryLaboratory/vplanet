import os

#def mpstatus():
count_done = 0
count_todo = 0
count_ip = 0
checkpoint_file = os.getcwd() + '/' + '.multi-planet'

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

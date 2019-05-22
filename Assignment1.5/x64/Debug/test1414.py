#!/bin/python
__author__ = 'fll'

import threading
import subprocess
import time
import sys

tests = [
    {
        'testname': 'one',
        'serverDelay': 5,
        'clientsDelays': [5],
        'numMsg': 10000,
        'memSize': 10,
        'msgSize': 1024,
    },
    {
        'testname': 'two',
        'serverDelay': 1,
        'clientsDelays': [5],
        'numMsg': 10000,
        'memSize': 10,
        'msgSize': 1024,
    },
    {
        'testname': 'three',
        'serverDelay': 5,
        'clientsDelays': [1],
        'numMsg': 10000,
        'memSize': 10,
        'msgSize': 1024,
    },
    {
         'testname': 'four',
        'serverDelay': 0,
        'clientsDelays': [0],
        'numMsg': 10000,
        'memSize': 10,
        'msgSize': 'random',
    }
]

# one thread per command
class ThreadTest(threading.Thread):
    def __init__(self, cmd):
        self.stdout = None
        self.stderr = None
        self.cmd = cmd
        threading.Thread.__init__(self)

    def run(self):
        p = subprocess.Popen( self.cmd.split(),
                         shell = False,
                         stdout = subprocess.PIPE,
                         stderr = subprocess.PIPE )
        self.stdout, self.stderr = p.communicate()


# change executable name if necessary to your program name
program = 'Assignment1.exe'

if __name__=='__main__':
    programName = program
    testName = 'all'
    for arg in sys.argv[1:]:
        # specify ONE test only using keyword 'test'. Eg. test=one
        if 'test' in arg:
            testName = arg[5:]

    for test in tests:
        # can use: test1414.py test=one 
        if test['testname'] in testName or testName == 'all':
            # repeat each test N times!
            for run in range(2):
                server = None
                clients = []
                print "====================================="
                print "Running test: " + test['testname']

                msgConf = "%s %s %s" % (test['memSize'],test['numMsg'],test['msgSize'])

                # start timer
                startTime = time.clock()

                commandLine = "%s producer %s %s" % (programName, test['serverDelay'], msgConf)
                server = ThreadTest(commandLine)
                server.start()

                time.sleep(0.1)

                # initiate all clients
                for clientDelay in test['clientsDelays']:
                    commandLine = "%s consumer %s %s" % (programName, clientDelay, msgConf)
                    print "Running: " + commandLine
                    clients.append ( ThreadTest (commandLine) )
                    clients[-1].start()
                    time.sleep(0.01)

                # wait for the server thread to finish.
                server.join()
                # wait for all clients threads to finish
                for c in clients:
                    c.join()

                endTime = time.clock()
                elapsed = endTime - startTime
                print "Time elapsed: ", elapsed

                # read the output of the server (producer)
                # note, if many messages are sent, the program will consume LOADS of RAM to
                # hold the intermediate results.
                result = server.stdout
                for c in clients:
                    # compare the output of each client with the server's
                    if (result != c.stdout):
                        print ""
                        print "Test failed"
                        print "Server output in file: server.out"
                        print "Client output in file: client.out"
                        with open("server.out", 'w') as serverout:
                            serverout.write(result)
                        with open("client.out", 'w') as clientout:
                            clientout.write(c.stdout)
                        sys.exit()
                else:
                    print "Test passed!"


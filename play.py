import heapq as hq
from calendar import c
import matplotlib.pyplot as plt
from scipy.stats import poisson
ENDOFTIME = 999999999999999999


class ProcessArrival ():
    pass


class Cpu():
    def __init__(self):
        self.myQue = []

    def getNewProcess():
        pass


class Disk():

    def __init__(self):
        self.myQue = []

    def getNewProcess():
        pass


class ProcessOutput():

    def __init__(self):
        self.myQue = []

    def getNewProcess():
        pass


class Process():
    pass


class CallMeAtTime():
    def __init__(self, time, parent):
        self.time = time
        self.parent = None  # generator or cpu or disk
        pass


def ProcessArrivalCheck():
    pass


def CpuCheck():
    pass


def DiskCheck():
    pass


def OutputCheck():
    pass


myProcessArrival = ProcessArrival()
myCpu = Cpu()
myDisk = Disk()
myOutput = ProcessOutput()
masterTime = 0
eventPriorityQueue = []
firstCall = CallMeAtTime(1, "myProcessArrival")
while True:

    # those ifs check if there is any action needed
    if ProcessArrivalCheck():
        newProcess = Process()
        myCpu.myQue.append(newProcess)
        pass

    if DiskCheck():
        myDisk.getNewProcess()
        pass

    if CpuCheck():
        myCpu.getNewProcess()
        pass

    if OutputCheck():
        myOutput.getNewProcess()
        pass

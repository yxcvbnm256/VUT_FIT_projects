

#CST:xpawlu00
import re
import os
import sys
from argParser import ArgParser
from search import FileExamination

"""Tvorba objektu ArgParser a FileExamination, nasledne se provede logika skriptu"""
parser = ArgParser()
filee = FileExamination(parser.method,parser.noPath,parser.inputFile,parser.output,parser.nosubdir)
filee.openFile(False)
filee.openFile(True)
filee.printSum()
sys.exit(0)
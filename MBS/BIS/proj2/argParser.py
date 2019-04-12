import getopt
import sys
import os
import eml_parser
import json
import binascii
import datetime
import base64
import binascii
import collections
import email
from analyze import EmailAnalyzer

"""Trida, ktera parsuje argumenty"""
class ArgParser(object):	
	def __init__(self):		
		opts = sys.argv[1:]		
		analyzer = EmailAnalyzer()
		for o in opts:
			try:
				with open(o, 'rb') as fhdl:
					raw_email = fhdl.read()
				parsed_eml = eml_parser.eml_parser.decode_email_b(raw_email, True, False)

				score = analyzer.analyze(parsed_eml, o)
			except FileNotFoundError:
				print(o + ' - FAIL - cannot find or open file')
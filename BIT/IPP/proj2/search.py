# -*- coding: latin-1 -*-
import re
import os
import glob
import fnmatch
import sys
from io import open
class FileExamination:
	inputFile = None
	maxPath = 0
	commentsCount = 0
	comments_regex = None
	other_regex = None
	replace_regex = None
	macro_regex = None	
	overall = 0
	noPath = False
	output = None
	maxNumber = 0
	sumNumber = 0
	noSubDir = False
	method = None
	complexOperator_regex = None
	finalList = []
	
	"""Konstruktor, ktery uklada informace z ArgParseru. method - metoda vyhledavani, noPath - true/false u zobrazovani cesty k souboru,
	input - vstupní slozka/soubor/nic, output - vystupni soubor/nic, nosubdir - true/false u prochazeni podslozek"""
	def __init__(self,method,noPath,input,output,nosubdir):
		if method == "k":
			self.comments_regex = re.compile("(?:/\\*(?:[^*]|(?:\\*+[^*/]))*\\*+/)|(?://.*\n)", re.MULTILINE)
			self.macro_regex = re.compile(r"(#define.*?\n|#ifdef.*?\n|#ifndef.*?\n|#endif.*?\n|#include.*?\n)")
			self.other_regex = re.compile("\W?(inline|_Bool|_Complex|_Imaginary|auto|break|case|const|continue|char|default|do|double|else|enum|extern|float|for|goto|if|int|long|register|return|short|signed|sizeof|static|struct|switch|typedef|union|unsigned|void|volatile|while)\W")
		elif method == "c":
			self.macro_regex = re.compile(r"(#define.*?\n|#ifdef.*?\n|#ifndef.*?\n|#endif.*?\n|#include.*?\n)")
			self.comments_regex = re.compile("(?:/\\*(?:[^*]|(?:\\*+[^*/]))*\\*+/)|(?://.*\n)", re.MULTILINE)
		elif method == "o":
			self.macro_regex = re.compile(r"(#define.*?\n|#ifdef.*?\n|#ifndef.*?\n|#endif.*?\n|#include.*?\n)")
			self.comments_regex = re.compile("(?:/\\*(?:[^*]|(?:\\*+[^*/]))*\\*+/)|(?://.*\n)", re.MULTILINE)
			self.replace_regex = re.compile("(char|const|double|float|int|long|short|void|_Bool|_Complex)(\s+|)(\*)(\s+|)[a-zA-Z_][a-zA-Z0-9_-]*")
			self.other_regex = re.compile("(\+|-|\*|\/|%|<|>|!|~|&|\|\^|=|\.)")
			self.complexOperator_regex = re.compile("\+\+|\-\-|\-=|\+=|\*=|\/=|%=|<<|<=|>=|>>\!=|&&|&=|\|\||\^=|\->|==")
			self.method = "o"	
		elif method == "i":
			self.comments_regex = re.compile("(?:/\\*(?:[^*]|(?:\\*+[^*/]))*\\*+/)|(?://.*\n)", re.MULTILINE)
			self.replace_regex = re.compile("(\W?(inline|_Bool|_Complex|_Imaginary|auto|break|case|const|continue|char|default|do|double|else|enum|extern|float|for|goto|if|int|long|register|return|short|signed|sizeof|static|struct|switch|typedef|union|unsigned|void|volatile|while)\W)|(\+|-|\*|\/|%|<|>|!|~|&|\|\^|=|\.)|(\"\w+|\\\")|(\'\w+|\\\')|(\\n|\\r|\\t)")
			self.macro_regex = re.compile(r"(#define.*?\n|#ifdef.*?\n|#ifndef.*?\n|#endif.*?\n|#include.*?\n)")
			self.other_regex = re.compile("[a-zA-Z_]+?\w*")	
		elif method != '':
			self.other_regex = re.compile("\W?(%s)\W" % method[1:])
		if noPath == True:
			self.noPath = True
		self.inputFile = input
		self.output = output
		if output != None:
			if os.path.isfile(output) == True:
				os.remove(output)
		if nosubdir == True:
			self.noSubDir = True			
	
	"""Aplikuje na textovy retezec regularni vyraz, ktery jej zbavi komentaru. comment_regex - zkompilovany regularni vyraz"""
	def getRidOfComments(self,string):
		return re.sub(self.comments_regex,'',string)

	"""Vyhleda v retezci urcity druh veci, other_regex obsahuje zkompilovany regularni vyraz urceny argumentem metody (-o, -i, -w, -c, -k)"""
	def applyRegex(self, string):
		return re.findall(self.other_regex,string)

	"""Metoda prochazejici soubor/soubory/podslozky. Mode - urcuje, kolikaty pruchod zrovna probiha (prvni - spocitani nejdelsi cesty atd.)"""
	def openFile(self, mode):
		"""Pokud neni zadan vstupni soubor"""
		if self.inputFile == None:
			if self.noSubDir == True:
				files = [f for f in os.listdir('.') if os.path.isfile(f)]
				for name in sorted(files):
					if name.endswith(('.c', '.h')):
						if self.noPath == False:
							Z = "%s" % (os.path.abspath(name))
						else:
							Z = "%s" % (name)	
						myFile = open(name, "r", encoding='latin8')								
						if mode == False:
							if len(Z) > self.maxPath:
								self.maxPath = len(Z)
							self.searchString(myFile.read(),Z, False)
						else:
							self.searchString(myFile.read(),Z, True)	
			else:	
				for path, subdirs, files in os.walk('./'):
					for name in sorted(files):
						if name.endswith(('.c', '.h')):
							if self.noPath == False:
								Z = "%s/%s" % (os.path.abspath(path),name)
							else:
								Z = "%s" % (name)	
							myFile = open(os.path.join(path, name), "r", encoding='latin8')								
							if mode == False:
								if len(Z) > self.maxPath:
									self.maxPath = len(Z)
								self.searchString(myFile.read(),Z, False)
							else:
								self.searchString(myFile.read(),Z, True)												
		elif os.path.isfile(self.inputFile) == True:
			if os.access(os.path.dirname(self.inputFile), os.R_OK) == False:
				print("Nelze otevrit vstupni soubor.",file=stderr)
				exit(3)
			if self.noPath == False:
				Z = "%s" % (os.path.abspath(self.inputFile))				
			else:
				Z = "%s" % (self.inputFile)
			self.maxPath = len(Z)	
			myFile = open(self.inputFile, "r", encoding='latin8')
			if mode == False:
				self.searchString(myFile.read(),Z, False)	
			else:
				self.searchString(myFile.read(),Z, True)	
		elif os.path.isdir(self.inputFile) == True:
			if os.access(os.path.dirname(self.inputFile), os.R_OK) == False:
				print("Nelze otevrit vstupni soubor.",file=stderr)
				exit(3)
			if self.noSubDir == True:
				files = [f for f in os.listdir('%s' % self.inputFile) if os.path.isfile(os.path.join(self.inputFile,f))]
				for name in sorted(files):
					if name.endswith(('.c', '.h')):
						if self.noPath == False:
							Z = "%s" % (os.path.abspath(name))
						else:
							Z = "%s" % (name)	
						myFile = open(os.path.join(self.inputFile,name), "r", encoding='latin8')								
						if mode == False:
							if len(Z) > self.maxPath:
								self.maxPath = len(Z)
							self.searchString(myFile.read(),Z, False)
						else:
							self.searchString(myFile.read(),Z, True)
			else:
				for path, subdirs, files in os.walk('%s' % self.inputFile):
					subdirs.sort()
					files.sort()
					for name in sorted(files):					
						if name.endswith(('.c', '.h')):
							if self.noPath == False:
								Z = "%s/%s" % (os.path.abspath(path),name)
							else:
								Z = "%s" % (name)
							myFile = open(os.path.join(path, name), "r", encoding='latin8')
							if mode == False:
								if len(Z) > self.maxPath:
									self.maxPath = len(Z)						
								self.searchString(myFile.read(),Z, False)
							else:
								self.searchString(myFile.read(),Z, True)
		else:
			print("Spatne zadan vstupni soubor/slozka.",file=sys.stderr)
			sys.exit(2)
	
	"""Metoda, ktera zbavi text nechtenych veci, jako textovych retezcu, escape znaku pred koncem radku atd."""
	def getRidOfOthers(self,string):
		string = string.replace('/\\\n*', '/*aa')
		string = string.replace('*\\\n/', 'aa*/')
		string = string.replace('\\\n/', '/aa')	
		if self.method != "o":
			string = string.replace('\\\n', '/a')
		else:
			string = string.replace('\\\n', 'aa')			
		string = re.sub(r'".*?"', '', string)
		string = re.sub(r'\'.*?\'', '', string)	
		return string

	"""Pokud chce uzivatel spocitat operatory, je nutno odstranit deklarace promennych, kde se vyskytuje *"""
	def operatorMode(self,string):
		string = re.sub(r'int\W*\**', '', string)
		string = re.sub(r'char\W*\**', '', string)
		string = re.sub(r'void\W*\**', '', string)
		string = re.sub(r'short\W*\**', '', string)
		string = re.sub(r'long\W*\**', '', string)		
		string = re.sub(r'struct\W*.+\W*\**', '', string)
		string = re.sub(r'\.\.\.', ' ', string)
		return string

	"""Metoda, ktera seradi seznam s vysledky, vypise jej a nakonec vypise i celkovy soucet."""
	def printSum(self):
		self.finalList.sort()
		for i in self.finalList:
			if self.output == None:
				print(i)
			else:
				with open(self.output, "a") as myfile0:
					myfile0.write("%s\n" % i)
		sumString = "CELKEM:"
		whitestring = ''
		for x in range (len(sumString), self.maxPath):
			whitestring += ' '
		if self.output == None:
			print ("%s%s %d" % (sumString, whitestring, self.sumNumber))
		else:			
			with open(self.output, "a") as myfile:
				myfile.write("%s%s %d\n" % (sumString, whitestring, self.sumNumber))

	"""Tato metoda aplikuje vsechny potrebne regularni vyrazy a dalsi upravy na text. V prvnim pruchodu (mode = false) spocita celkovy pocet
	vyskytu kvuli formatovani, v druhem (mode = true) jiz vklada spravne formatovane vysledky do seznamu. Path - cesta k souboru"""
	def searchString(self,string, path, mode):
		string = self.getRidOfOthers(string)			
		if self.comments_regex != None:	
			comments = ''.join(re.findall(self.comments_regex,string))			
			comments = comments.replace("\r\n", "_")
			comments = comments.replace("\n", "_")
			comments = comments.replace("\t", "_")
			self.commentsCount = len(comments)						
			string = self.getRidOfComments(string)		
		if self.macro_regex != None:
			string = re.sub(self.macro_regex, '', string)

		if self.replace_regex != None:
			string = re.sub(self.replace_regex, ',', string)
		if self.method == "o":
			string = self.operatorMode(string)
			found = re.findall(self.complexOperator_regex,string)
			count = len(found)
			string = re.sub(self.complexOperator_regex,'',string)
		if self.other_regex != None:
			found = self.applyRegex(string)
			if self.method == "o":
				count += len(found)
			else:
				count = len(found)
		else:
			count = self.commentsCount
		if mode == False:						
			self.sumNumber += count
		if mode == True:
			whitestring = ''
			if (self.maxPath < 7):
				for x in range (self.maxPath, 7):
					whitestring += ' '			
			for x in range (len(path), self.maxPath):
				whitestring += ' '
			for x in range (len(str(count)), len(str(self.sumNumber))):
				whitestring += ' '
			self.finalList.append("%s%s %d" % (path, whitestring, count))						

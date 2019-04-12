import getopt
import sys
import os

class ArgParser(object):
	output = None
	inputFile = None
	pattern = None
	method = None
	nosubdir = False
	noPath = False
	helpin = False

	"""Metoda, ktera tiskne napovedu"""
	def printHelp(self):
		print ("Napoveda pro pouziti scriptu:")
		print("		-c: Spocita vsechny znaky komentaru.")
		print("		-k: Spocita vsechna klicova slova.")
		print("		-i: Spocita vsechny identifikatory.")
		print("		-o: Spocita vsechny operatory.")
		print("		-w=prvek: Spocita vsechny vyskyty 'prvek'.")
		print("	Vyse uvedene parametry se nedaji kombinovat, musi byt zadan prave jeden.")
		print("		--input=input: Vyhledava se ve slozce/souboru 'input'.")
		print("		--output=output: Vysledek ulozi do souboru 'output'.")
		print("		--nosubdir: Prohledava se pouze v aktualni slozce bez podslozek.")
		print("		-p: Vypise misto cesty k souboru jen jeho jmeno.")
		sys.exit(0)

	"""Konstruktor, ktery zpracovava argumenty a uklada jej do tridnich promennych"""
	def __init__(self):
		try:
			opts, args = getopt.getopt(sys.argv[1:], "koicpw:",
                                       ["help", "input=",
                                        "nosubdir", "output="])
		except getopt.GetoptError as err:
			print(err)			
			print("Neznamy parametr.",file=sys.stderr)
			sys.exit(1)        
		for o, a in opts:
			if o == "-k":
				if self.method is None:
					self.method = "k"
				else:
					print("Spatna kombinace parametru.",file=sys.stderr)
					sys.exit(1)
			elif o == "-o":
				if self.method is None:
					self.method = "o"
				else:
					print("Spatna kombinace parametru.",file=sys.stderr)
					sys.exit(1)
			elif o == "-i":
				if self.method is None:
					self.method = "i"
				else:
					print("Spatna kombinace parametru.",file=sys.stderr)
					sys.exit(1)
			elif o == "-c":
				if self.method is None:
					self.method = "c"
				else:
					print("Spatna kombinace parametru.",file=sys.stderr)
					sys.exit(1)
			elif o == "-w":
				if self.method is None:
					self.method = a
				else:
					print("Spatna kombinace parametru.",file=sys.stderr)
					sys.exit(1)
			elif o == "--help":
				self.helpin = True
			elif o == "-p":
				self.noPath = True
			elif o == "--input":
				if self.inputFile != None:
					print("Dvojnasobne zadany input.", file=sys.stderr)
					sys.exit(1)
				if self.nosubdir == True:
					print("Spatna kombinace parametru.",file=sys.stderr)
					sys.exit(1)
				self.inputFile = a
			elif o == "--output":
				if self.output != None:
					print("Dvojnasobne zadany output.", file=sys.stderr)
					sys.exit(1)				
				if os.path.exists(a):
					self.output = a
				elif os.access(os.path.dirname(a), os.W_OK):
					self.output = a
				else:
					print("Nelze pristoupit k vystupnimu souboru", file=sys.stderr)
					exit(3)
			elif o == "--nosubdir":
				if self.inputFile == True:
					print("Spatna kombinace parametru.",file=sys.stderr)
					sys.exit(1)
				self.nosubdir = True
		if self.helpin == True and (self.method != None or self.nosubdir == True or self.noPath == True):
			print("Nelze kombinovat napovedu s jakymkoliv jinym parametrem.", file=sys.stderr)
			exit(1)
		elif self.helpin == True:
			self.printHelp()
		if self.method == None:
			print("Musi byt zadan prave jeden z parametru -w, -c, -k, -o nebo -i.",file=sys.stderr)
			exit(1)



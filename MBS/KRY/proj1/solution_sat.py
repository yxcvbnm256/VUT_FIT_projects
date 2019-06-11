#!/usr/bin/env python3

''' 
KRY 2019
1. projekt - SAT solver
Jan Pawlus - xpawlu00
'''

from z3 import *
import sys
import signal

N_B = 32
N = N_B * 8

''' funkce, ktera zkontroluje zadanou cestu k souboru/existenci souboru a pokud existuje, vrati jeho data. '''
def get_file(path, filename):
	if not os.path.isfile(path + filename):
		sys.stderr.write('Zadana cesta k souboru je spatna nebo neexistujici soubor.\n')
		exit(1)
	file = open(path + filename, 'rb')
	content = file.read()
	file.close()
	return content

''' funkce, ktera desifruje kus souboru se sifrovacim skriptem '''
def print_script(path, y):
	c = get_file(path, 'super_cipher.py.enc')
	script = bytearray(len(y))
	for i in range(len(script)):
		script[i] = y[i] ^ c[i]
	script = script.decode()
	print(script)

''' funkce simulujici pristup do pole SUB, jelikoz BitVec nelze pouzit jako index do pole... '''
def get_sub(x, i):
	# lsb indexu do SUB
	lsb = (x >> i) & 1
	# prostredni bit indexu do SUB
	mid = (x >> (i + 1)) & 1
	# msb indexu do SUB
	msb = (x >> (i + 2)) & 1
	# navrati 1, pokud indexy jsou 001, 010, 100, 110
	return (msb & ~lsb) | (~msb & ((~mid & lsb) | (mid & ~lsb)))

''' puvodni funkce step, upraveno je jen pristup do pole SUB. '''
def step(x):
	x = (x & 1) << N + 1 | x << 1 | x >> N - 1
	y = 0
	for i in range(N):
		y |= get_sub(x, i) << i
	return y

''' funkce hledajici reseni. '''
def solve(y, i):
	# klic je sice 256bitovy, avsak funkce step jej rozsiruje na 258 bitu, tedy velikost musi byt N + 2
	x = BitVec('x', N + 2)
	sat_solver = Solver()
	# prvni podminka - finalni vystup se musi rovnat y
	sat_solver.add(step(x) == y)
	# finalni klic ale musi byt 256bitovy - vyradime vysledky, ktere maji nenulove prvni dva bity
	sat_solver.add((x >> N) == 0)
	# pokud je nalezeno reseni
	while sat_solver.check() == sat:
		# pokud jeste nejsme v poslednim zanoreni, vezmeme aktualni vysledek (output step) a zanorime se znova
		if i < (N // 2) - 1:
			res = solve(sat_solver.model()[x].as_long(), i + 1)
			# zde umiraji mrtve vetve reseni, pokud je vysledek None
			if res is not None:
				return res
		# pokud jsme v poslednim zanoreni, provedli jsme funkci step N // 2 krat dle zasifrovaneho skriptu, mame tedy potencialni vysledek
		else:
			return sat_solver.model()[x]
		# vylouceni jiz prozkoumane klauzole -> urychleni	
		sat_solver.add(x != sat_solver.model()[x])

def handler(signum, frame):
    print ('Signal handler called with signal' + signum)



if __name__ == '__main__':
	# je treba zadat argument cesty k souborum
	signal.signal(signal.SIGABRT, handler)
	if len(sys.argv) != 2:
		sys.stderr.write('Nespravny pocet argumentu - musi byt zadana cesta ke vstupnim souborum.\n')
		exit(1)
	path = sys.argv[1]
	# ziskani obsahu plaintext souboru
	p = get_file(path, 'bis.txt')
	# ziskani obsahu ciphertext souboru
	c = get_file(path, 'bis.txt.enc')
	# dle velikosti plaintextu vytvoreno pole bajtu obsahujici xor plaintextu a ciphertextu
	y = bytearray(len(p))
	for i in range(len(y)):
		y[i] = p[i] ^ c[i]
	#print_script(path, y)
	# jelikoz vime, ze klic je 256bitovy, staci nam prvnich 32 bajtu 
	y_32 = int.from_bytes(y[:32], 'little')
	res = solve(y_32,0)
	if (res is None):
		sys.stderr.write('Nebylo nalezeno zadne reseni.\n')
		exit(1)
	key = res.as_long().to_bytes(N_B, 'little').decode()
	# jelikoz vysledek obsahuje prazdne bajty, je treba jej orezat jen do znaku }, ktery znamena konec tajemstvi
	key = key[:key.find('}') + 1]
	print(key, end = '')

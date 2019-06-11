#!/usr/bin/env python3

''' 
KRY 2019
1. projekt - rucni solver
Jan Pawlus - xpawlu00
'''

import sys
import os.path

N_B = 32
N = N_B * 8
mask = ~(1 << N)
SUB = [0, 1, 1, 0, 1, 0, 1, 0]

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

''' puvodni funkce step, slouzici pouze pro debug '''
def step(x):
	x = (x & 1) << N+1 | x << 1 | x >> N-1
	y = 0
	for i in range(N):
		y |= SUB[(x >> i) & 7] << i
	return y

''' reverzni funkce k funkci step'''
def reverse_step(keystream):
	# pole results obsahuje cisla (posloupnost bitu), ktere mohou vest k vysledku
	results = []
	# pole possibilities obsahuje mozne indexy do pole SUB na zaklade vystupu z pole SUB
	possibilities = []
	# docasne pole vysledku v jednotlivych iteracich
	tmp_results = []
	# indexy do pole SUB, ktere ukazuji na hodnotu 1
	ones = [1,2,4,6]
	# indexy do pole SUB, ktere ukazuji na hodnotu 0
	zeros = [0,3,5,7]
	# jako prvni se testuje hodnota LSB reverzovaneho retezce; pokud je 1, index do SUB mohl byt jeden z pole 'ones', jinak z pole 'zeros'
	val = keystream & 1
	if (val == 1):
		results = ones
	else:
		results = zeros
	# cyklus od 1 (LSB reverzovaneho retezce byla provedena jiz vyse)
	for i in (range(1, N)):
		# testujeme hodnoty od LSB k MSB
		val = (keystream >> i) & 1
		if (val == 1):
			possibilities = ones
		else:
			possibilities = zeros
		# ted se podivame na vsechny prozatimni moznosti z predchozich kroku a divame se, jak mohly pristupy do pole SUB na sebe navazovat, jelikoz 
		# v iteracich, ktere jdou po sobe, se diky bitovemu posuvu musi rovnat dva MSB predchoziho indexu (result) dvěma LSB aktualnich potencialnich indexu (possibilities)
		for result in results:
			tmp_res = (result >> i) & 3
			for possibility in possibilities:
				tmp_pos = possibility & 3
				if (tmp_pos == tmp_res):
					tmp_results.append(result | (possibility << i))
		results = tmp_results
		tmp_results = []
	# vysledkem jsou 4 mozne klice, avsak diky bitovym operacim s parametrem x vime, ze spravny je pouze takovy klic, jehoz dva MSB a LSB se rovnaji
	for i in results:
		if ((i >> N) & 3) == (i & 3):
			# nakonec vime, ze nalezeny klic byl rozsiren o jeden bit vlevo a jeden vpravo - provedeme orez
			i = (i >> 1) & mask
			return i
	return None

''' funkce, ktera desifruje jakykoliv soubor zasifrovany touto sifrou. vytvorena ze zvedavosti obsahu hint.gif '''
def decrypt_file(path, file, key):
	content = get_file(path, file)	
	keystr = key
	for i in range(N//2):
		keystr = step(keystr)
	keystr_bytes = keystr.to_bytes(32, 'little')
	dec = bytearray(len(content))
	for j in range(len(content)//len(keystr_bytes)+1):		
		for i in range(len(keystr_bytes)):
			if (j*len(keystr_bytes)+i == len(content)):
				break
			dec[j*len(keystr_bytes)+i] = keystr_bytes[(i)] ^ content[j*len(keystr_bytes)+i]
		keystr = step(keystr)
		keystr_bytes = keystr.to_bytes(32, 'little')
	f = open(path + file[:file.rfind('.')], 'wb')
	f.write(dec)
	f.close()

if __name__ == '__main__':
	# je treba zadat argument cesty k souborum
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
	for i in range(N//2):
		y_32 = reverse_step(y_32)
		if y_32 is None:
			sys.stderr.write('Klic nelze nalezt - chyba pri reverzaci.\n')	
	#decrypt_file(path, 'hint.gif.enc', y_32)
	key = y_32.to_bytes(32, 'little').decode()
	# jelikoz vysledek obsahuje prazdne bajty, je treba jej orezat jen do znaku }, ktery znamena konec tajemstvi
	key = key[:key.find('}') + 1]
	print(key, end = '')

#!/usr/bin/env python3

''' 
KRY 2019
2. projekt - ECC solver privatniho klice
Jan Pawlus - xpawlu00
zdroje ke vzoreckum a postupu - dokument Elipticke krivky a sifrovani ze zadani
'''

import sys

class Decipher(object):
	# pocatecni body - ze zadani - bod P
	def_point_x = 0x6b17d1f2e12c4247f8bce6e563a440f277037d812deb33a0f4a13945d898c296
	def_point_y = 0x4fe342e2fe1a7f9b8ee7eb4a7c0f9e162bce33576b315ececbb6406837bf51f5
	public_x = 0
	public_y = 0
	# modulo - ze zadani
	f_p = 0xffffffff00000001000000000000000000000000ffffffffffffffffffffffff
	a = -0x3
	b = 0x5ac635d8aa3a93e7b3ebbd55769886bc651d06b0cc53b0f63bce3c3e27d2604b

	''' zpracovani argumentu - verejny klic '''
	def parseArgs(self):
		if len(sys.argv) != 3:
			sys.stderr.write('Nespravny pocet argumentu - musi byt zadany pocatecni body.\n')
			exit(1)
		self.public_x = int(sys.argv[1][1:-1], 16)
		self.public_y = int(sys.argv[2][:-1], 16)

	''' metoda, ktera spocte inverzni prvek telesa G(f_p). Lze pouzit jen, pokud je f_p prvocislo. 
		zdroj: https://stackoverflow.com/questions/4798654/modular-multiplicative-inverse-function-in-python/4798776 '''
	def getInverseNumber(self, number):
		return pow(number, self.f_p-2, self.f_p)

	''' metoda, ktera vraci cislo v dane zbytkove tride - slo by napsat primo do kodu, metoda je kvuli prehlednosti kodu '''
	def getInModuloRange(self, number):					
		return number % self.f_p

	''' metoda pocitajici smernici tecny v prvnim kroku, kdy se pocita 2P = P + P, tedy stejne body (proto tecna) '''
	def slopeDifferentPoints(self, x1, y1, x2, y2):
		s = (y2 - y1)*self.getInverseNumber(x2 - x1)
		return self.getInModuloRange(s)

	''' metoda pocitajici smernici primky vytvorenou po secteni dvou ruznych bodu '''
	def slopeSamePoints(self, x, y):
		s = (3 * (x**2) + self.a) * self.getInverseNumber(2 * y)
		return self.getInModuloRange(s)

	''' metoda provadejici kroky vypoctu '''
	def doWork(self):
		# k = privatni klic (Q = k * P) - 2 proto, ze prvni se jeste mimo iteraci pocita 2P, tedy k == 2
		k = 2
		# vypocet 2P - ze stejnych dvou bodu (pocatecnich)
		s = self.slopeSamePoints(self.def_point_x, self.def_point_y)
		next_point_x = self.getInModuloRange(s**2 - self.def_point_x - self.def_point_x)
		next_point_y = self.getInModuloRange(s * (self.def_point_x - next_point_x) - self.def_point_y)
		while True:
			# pokud se vypoctene body rovnaji verejnemu klici, nasli jsme privatni klic - k
			if next_point_x == self.public_x and next_point_y == self.public_y:
				break
			# vypocet novych bodu - k * P
			s = self.slopeDifferentPoints(self.def_point_x, self.def_point_y, next_point_x, next_point_y)
			next_point_x = self.getInModuloRange(s ** 2 - self.def_point_x - next_point_x)
			next_point_y = self.getInModuloRange(s * (self.def_point_x - next_point_x) - self.def_point_y)
			k += 1
		return k

if __name__ == '__main__':
	decipher = Decipher()
	decipher.parseArgs()
	print(decipher.doWork(), end='')
	
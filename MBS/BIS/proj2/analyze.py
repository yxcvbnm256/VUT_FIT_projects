# -*- coding: latin-1 -*-
import re
import os
import glob
import fnmatch
import datetime
import sys
import unidecode
import eml_parser
import json
import binascii
from io import open

class EmailAnalyzer:
	emailParsed = ''
	threshold = 5.2
	"""Konstruktor, ktery kompiluje vyuzivane regexy"""
	def __init__(self):		
		self.dearFriendRegex = re.compile(r"(dear friend|drahy kamarade|ahoj kamarade|my good friend)", re.I | re.M)
		self.contentHTML = re.compile("(text/html)", re.I | re.M)
		self.contentPlaintext = re.compile("(text/plain)", re.I | re.M)
		self.linkIpAddress = re.compile(r"(http://\b\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}\b)", re.I | re.M)
		self.subject = re.compile(r"\"subject\": \"(your bill|your family|vase rodina|inheritance|dedictvi|inherited|zdedil)", re.I | re.M)
		self.halfDrug = re.compile(r"radiotherapy|lekarna|online pharmacy|purchase now|kupte ted|kup ted|gain muscle|lose weight|weight loss|burning fat|spalovani tuku|ziskat sval|zhubnout", re.I | re.M)
		self.drugBody = re.compile(r"viicodin|blood preasure|drug| meds |vagira|vairga|vlagra|heroin|octahedral|cholesterol|pills|pillz|adipex|vicodin| cialis |cilais|caiils|viagra|xanax|levitra|valium|no prescription|bez predpisu|diabetic|Diabetes|medicine|glucophage|online prescription|pharmacy", re.I | re.M)
		self.emailDomainOffer = re.compile(r"[A-Za-z0-9._%+-]+@[A-Za-z0-9._%+-]+offer.[A-Za-z0-9._%+-]", re.M | re.S | re.X | re.U)
		self.halfSexContent = re.compile(r"always hard|your husband|my husband|your wife|my wife|vas manzel|vas pritel|vase zena|vase pritel|tvuj manzel|tvoje manzelka", re.I | re.M)
		self.sexContent = re.compile(r" bang |does she cheat|podvadi te|sperm|slut|your area|handjob|erotika|eroticke|eroticky|erotic|shag|shaggy|get hard|enlarging oil|enlarging|erection|fagott|faggot|feeling tired|alpha male|premature ejaculat|predcasny orgazmus|orgasmus|orgazmus|penis enlargement| anus |your wife|spank|grows hard|grow hard|grew hard|weenie| lover|fucking|fuck|naked|sexually|medication|sex|vagina|penis length|penis|penis size|prolong penis|sex drive|chut na sex|sexual|sexualni|hot sex|in bed|v posteli|sex life|breasts|boobs| ass ", re.I | re.M)
		self.extraSexContent = re.compile(r"hot women|click here to see|fingered|fingering|lesbian|lesbick| gay |naughty|naughty pics|horny|naked pics|naked photos|suck my dick|suck your dick|f\*ck|f\*cking|pu\$\$y|a\$\$|pussy|\$ex|p\*ssy|is wet|quickie|one night stand", re.I | re.M)
		self.uppercaseWords = re.compile(r" [A-Z][A-Z]+", re.M)
		self.dating = re.compile(r" milf| milfs|dating online|online dating|seznamit se|re hot", re.I | re.M)
		self.halfDating = re.compile(r"profile|your profile", re.I | re.M)
		self.inheritance = re.compile(r"inheritance|inherited|deceased|family member|dedictvi|zdedit|zdedil|clen rodiny", re.I | re.M)
		self.halfInheritance = re.compile(r"secretary|sekretar|legal advice|legal representation", re.I | re.M)
		self.halfMoney = re.compile(r"insurance|pujcka|pujcky|pojisteni|income opportunities|not regret", re.I | re.M)
		self.money = re.compile(r"interest rate|bargain prices|business proposal|become your employer|stante se vasim zamestnavatelem|nasi zakaznici|our customers|advertising boost|advertise boost|investment needed|your sale|home loan|home business|for free|zivotni pojisteni|life insurance|card debt|dluh|make fortune|make a fortune|more money|special offer|specialni nabidka|millionaire|milionar|money back|your bill|phone bill|nove auto|new car|improve your|jednoduchy trik|easy trick|loterie|vyhra|vyhra v loterii|stastne cisla|stastna cisla|million dollar|millions of dollars|milion korun|milion dolaru|win millions|miliony korun|you can win|muzete vyhrat|vyhral jste|vyhral\(a\) jste|vyhrala jste|you won|you have won|into your account", re.I | re.M)
		self.misc = re.compile(r"not a joke|sokujici tajemstvi|shocking secret|you deserve|zaslouzite si|your future|vase budoucnost|tvoje budoucnost|mortgage|quit smoking|your dream|exclusive position|work for you|worked for me|wish for|bahama|offer only|miraculous|gratis|emergence|licensed physicians", re.I | re.M)
		self.halfMisc = re.compile(r"rolex|attention important|for only|jen za|special announcement|specialni oznameni|product works|produkt funguje|auction|partnership|aukce|low cost|emerging", re.I | re.M)
		self.youHavebeenSelected = re.compile(r"waiting for you|ceka jen na|blahopreji vam|you have to see this|musis to uvidet|musis to videt|i never sent|believe it or not|you have been selected|you were selected|congratulation|gratulujeme|byl jste vybran|byl\(a\) jste vybran|byla jste vybran", re.I | re.M)
		self.transaction = re.compile(r"transaction|transakce", re.I | re.M)

	"""Metoda, ktera provede analyzu e-mailu"""
	def analyze(self, parsed_eml, filename):
		score = 0
		content = ''
		log = []
		self.emailParsed = ''
		self.dumpclean(parsed_eml)
		for cont in parsed_eml['body']:
			content += unidecode.unidecode(cont['content'])
		score = 0
		for x in re.findall(self.extraSexContent,self.emailParsed):
			score += 9
			if x not in log:
				log.append(x)
		if score >= self.threshold:
			self.classificate(score, log, filename)
			return
		for x in re.findall(self.inheritance,self.emailParsed):
			score += 7
			if x not in log:
				log.append(x)
		if score >= self.threshold:
			self.classificate(score, log, filename)
			return
		for x in re.findall(self.dating,self.emailParsed):
			score += 6.5
			if x not in log:
				log.append(x)
		if score >= self.threshold:
			self.classificate(score, log, filename)
			return
		for x in re.findall(self.drugBody,self.emailParsed):
			score += 6.4
			if x not in log:
				log.append(x)
		if score >= self.threshold:
			self.classificate(score, log, filename)
			return
		for x in re.findall(self.sexContent,self.emailParsed):
			score += 6.2
			if x not in log:
				log.append(x)
		if score >= self.threshold:
			self.classificate(score, log, filename)
			return
		for x in re.findall(self.linkIpAddress,self.emailParsed):
			score += 4
			if x not in log:
				log.append(x)
		if score >= self.threshold:
			self.classificate(score, log, filename)
			return
		for x in re.findall(self.misc,self.emailParsed):
			score += 4
			if x not in log:
				log.append(x)
		if score >= self.threshold:
			self.classificate(score, log, filename)
			return
		for x in re.findall(self.halfDrug,self.emailParsed):
			score += 4
			if x not in log:
				log.append(x)
		if score >= self.threshold:
			self.classificate(score, log, filename)
			return
		for x in re.findall(self.halfSexContent,self.emailParsed):
			score += 3.5
			if x not in log:
				log.append(x)
		if score >= self.threshold:
			self.classificate(score, log, filename)
			return
		for x in re.findall(self.halfDating,self.emailParsed):
			score += 1.5
			if x not in log:
				log.append(x)
		if score >= self.threshold:
			self.classificate(score, log, filename)
			return
		for x in re.findall(self.halfInheritance,self.emailParsed):
			score += 3.5
			if x not in log:
				log.append(x)
		if score >= self.threshold:
			self.classificate(score, log, filename)
			return
		for x in re.findall(self.money,self.emailParsed):
			score += 3
			if x not in log:
				log.append(x)
		if score >= self.threshold:
			self.classificate(score, log, filename)
			return
		for x in re.findall(self.youHavebeenSelected,self.emailParsed):
			score += 2.8
			if x not in log:
				log.append(x)
		if score >= self.threshold:
			self.classificate(score, log, filename)
			return
		for x in re.findall(self.halfMoney,self.emailParsed):
			score += 2.5
			if x not in log:
				log.append(x)
		if score >= self.threshold:
			self.classificate(score, log, filename)
			return
		for x in re.findall(self.emailDomainOffer,self.emailParsed):
			score += 2.5
			if x not in log:
				log.append(x)
		if score >= self.threshold:
			self.classificate(score, log, filename)
			return
		for x in re.findall(self.halfMisc,self.emailParsed):
			score += 2.5
			if x not in log:
				log.append(x)
		for x in re.findall(self.transaction,self.emailParsed):
			score += 0.5
			if x not in log:
				log.append(x)
		#score += 10*len(re.findall(self.extraSexContent,self.emailParsed))
		#print(re.findall(self.extraSexContent,self.emailParsed))
		if score >= self.threshold:
			self.classificate(score, log, filename)
			return
		for x in re.findall(self.dearFriendRegex,self.emailParsed):
			score += 2
			if x not in log:
				log.append(x)
		if score >= self.threshold:
			self.classificate(score, log, filename)
			return
		if bool(re.search(self.contentHTML,self.emailParsed)) == True and bool(re.search(self.contentPlaintext,self.emailParsed)) == False:
			score += 2
		score += 0.1*len(re.findall(self.uppercaseWords,content))			
		self.classificate(score, log, filename)
		return
	"""Metoda provadejici vyhodnoceni"""
	def classificate(self, score, log, filename):
		self.emailParsed = ''
		wordsList = ''
		for x in log:
			wordsList += x + ', '
		wordsList = wordsList[:-2]
		if score >= self.threshold:
			print(filename + ' - SPAM - contains words ' + wordsList)
		else:
			print(filename + ' - OK')
		return

	"""Metoda, ktera prevede e-mail do jednoho stringu"""
	def dumpclean(self, obj):
		if type(obj) == dict:
			for k, v in obj.items():
				if hasattr(v, '__iter__'):
					self.emailParsed += k + ': '
					self.dumpclean(v)
		elif type(obj) == list:
			for v in obj:
				if hasattr(v, '__iter__'):
					self.dumpclean(v)
				else:
					self.emailParsed += v
		else:
			if type(obj) != bytes:
				self.emailParsed += unidecode.unidecode(obj)

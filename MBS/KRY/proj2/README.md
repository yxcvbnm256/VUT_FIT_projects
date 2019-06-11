7/7

Základní informace
Tento projekt se věnuje asymetrické kryptografii za použití eliptických křivek (ECC - Elliptic Curve Cryptography). Vaším úkolem je seznámit se s operacemi nad eliptickými křivkami, jejich použitím v kryptografii a prolomit zadaný ECC klíč.

Materiály pro studium ECC:

Eliptické křivky a šifrování
Zadání projektu
Vaším úkolem je v libovolném jazyce implementovat program, který bude schopný ze zadaného veřejného ECC klíče vygenerovat klíč privátní. Tento program musí být spustitelný na serveru Merlin.

Dokumentaci není v tomto projektu třeba řešit. Taktéž ošetřování vstupu není předmětem projektu a není tedy nezbytné ošetřovat nevalidní vstupy.

Vstupní údaje pro ECC

Fp
0xffffffff00000001000000000000000000000000ffffffffffffffffffffffff
a
-0x3
b
0x5ac635d8aa3a93e7b3ebbd55769886bc651d06b0cc53b0f63bce3c3e27d2604b
základní bod
(0x6b17d1f2e12c4247f8bce6e563a440f277037d812deb33a0f4a13945d898c296,
 0x4fe342e2fe1a7f9b8ee7eb4a7c0f9e162bce33576b315ececbb6406837bf51f5)

Odevzdání projektu
Do WISu odevzdávejte archiv "{xloginXY}.tgz" vytvořený archivačním programem tar s kompresí gzip. Archiv bude v kořenovém adresáři (bez zanoření) obsahovat Vaše zdrojové soubory a soubor Makefile. Soubor Makefile bude obsahovat pravidlo decipher s parametrem publicKey, kdy na standardním výstupu lze po zavolání očekávat pouze číslo, které je řešením (soukromým klíčem). V případě nedokončeného řešení vypište na standardní výstup 0.

V případě nedodržení těchto pravidel bude projekt automaticky hodnocen 0 body!

Příklad ukázky spuštění
Pokud jste nalezli klíč s hodnotou 17:
$ make decipher publicKey="(0x477...3e, 0xaa0...dc)"
17
Pokud Vaše řešení není dokončené:
$ make decipher publicKey="(0x477...3e, 0xaa0...dc)"
0

Testovací veřejný klíč
(0x52910a011565810be90d03a299cb55851bab33236b7459b21db82b9f5c1874fe,
0xe3d03339f660528d511c2b1865bcdfd105490ffc4c597233dd2b2504ca42a562)
Klíč je zvolen tak, aby řešení bylo možné i na relativně pomalém počítači.
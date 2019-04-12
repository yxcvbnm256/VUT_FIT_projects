--xpawlu00
--xstude21

  DROP TABLE Druh_caje CASCADE CONSTRAINTS;
  DROP TABLE Zeme CASCADE CONSTRAINTS;
  DROP TABLE Oblast CASCADE CONSTRAINTS;
  DROP TABLE Caj CASCADE CONSTRAINTS;
  DROP TABLE Kvalita CASCADE CONSTRAINTS;
  DROP TABLE Hodnoceni CASCADE CONSTRAINTS;
  DROP TABLE Dodavatel CASCADE CONSTRAINTS;
  DROP TABLE Varka CASCADE CONSTRAINTS;
  DROP TABLE Objednavka CASCADE CONSTRAINTS;
  DROP TABLE Mnozstvi CASCADE CONSTRAINTS;
  DROP TABLE Odberatel CASCADE CONSTRAINTS;
  DROP TABLE Firma CASCADE CONSTRAINTS;
  DROP TABLE Fyzicka_osoba CASCADE CONSTRAINTS;

  

   CREATE TABLE Druh_caje
    (
      id_druh NUMBER NOT NULL PRIMARY KEY ENABLE,
      nazev_druh VARCHAR(35) NOT NULL ENABLE
    );

    CREATE TABLE Zeme
(
  nazev_zeme VARCHAR(30) NOT NULL PRIMARY KEY,
  zajimavost_zeme VARCHAR(666)
);

CREATE TABLE Oblast
(
  id_oblast NUMBER NOT NULL PRIMARY KEY,
  nazev_oblast VARCHAR(30) NOT NULL, 
  nazev_zeme VARCHAR(30) NOT NULL, 
  charakteristika VARCHAR(255),
  zajimavost_oblast VARCHAR(666)
);

ALTER TABLE Oblast ADD (CONSTRAINT FK_zeme FOREIGN KEY (nazev_zeme) 
   REFERENCES Zeme ON DELETE CASCADE);

CREATE TABLE Caj 
(  id_caj NUMBER NOT NULL PRIMARY KEY,
   id_druh NUMBER NOT NULL,
  id_oblast NUMBER NOT NULL,
  nazev_caj VARCHAR2(35) NOT NULL, 
  doba_louhovani NUMBER NOT NULL, 
  popis VARCHAR2(255) NOT NULL   
); 

ALTER TABLE Caj ADD CONSTRAINT fk_druh FOREIGN KEY (id_druh) 
   REFERENCES Druh_caje ON DELETE CASCADE;

ALTER TABLE Caj ADD CONSTRAINT fk_oblast FOREIGN KEY (id_oblast) 
   REFERENCES Oblast ON DELETE CASCADE;

CREATE TABLE Kvalita
(
  nazev_stupnice VARCHAR(30) NOT NULL PRIMARY KEY,
  zpusob_hodnoceni VARCHAR(255)
);

CREATE TABLE Hodnoceni
(
  id_caj NUMBER NOT NULL,
  nazev_stupnice VARCHAR(30) NOT NULL,
  Hodnoceni NUMBER NOT NULL  
);

ALTER TABLE Hodnoceni ADD CONSTRAINT fk_caj FOREIGN KEY (id_caj) 
   REFERENCES Caj ON DELETE CASCADE;

ALTER TABLE Hodnoceni ADD CONSTRAINT fk_kvalita FOREIGN KEY (nazev_stupnice) 
   REFERENCES Kvalita ON DELETE CASCADE;

  CREATE TABLE Dodavatel
    (
      id_dodavatel NUMBER NOT NULL PRIMARY KEY,
      jmeno VARCHAR(20) NOT NULL,
      prijmeni VARCHAR(20) NOT NULL,
      popis VARCHAR(255) NOT NULL,
      verejny NUMBER DEFAULT 1
    );

   CREATE TABLE Varka 
   (
    id_varka NUMBER NOT NULL PRIMARY KEY, 
    id_caj NUMBER NOT NULL,
    id_dodavatel NUMBER NOT NULL,
  datum_expedice DATE NOT NULL, 
  druh_prepravy VARCHAR(20) NOT NULL,
  mnozstvi NUMBER NOT NULL,
  cena NUMBER NOT NULL,
  sleva NUMBER
   );

ALTER TABLE Varka ADD CONSTRAINT fk_caj_varka FOREIGN KEY (id_caj) 
   REFERENCES Caj ON DELETE CASCADE;

ALTER TABLE Varka ADD CONSTRAINT fk_dodavatel FOREIGN KEY (id_dodavatel) 
   REFERENCES Dodavatel ON DELETE CASCADE;

CREATE TABLE Odberatel
 (
  id_odberatel NUMBER NOT NULL PRIMARY KEY,
  adresa VARCHAR(50),
  email VARCHAR(100),
  telefon NUMBER
 );

  CREATE TABLE Objednavka
  (    
    id_objednavka NUMBER NOT NULL PRIMARY KEY,
    id_odberatel NUMBER NOT NULL,
    datum_objednani DATE NOT NULL,
    stav_objednavky VARCHAR(30) NOT NULL    
  );

  ALTER TABLE Objednavka ADD CONSTRAINT fk_odberatel FOREIGN KEY (id_odberatel) 
   REFERENCES Odberatel ON DELETE CASCADE;

  CREATE TABLE Mnozstvi
  (
    id_objednavka NUMBER NOT NULL,
    id_varka NUMBER NOT NULL,
    mnozstvi NUMBER NOT NULL,
    nakupni_cena NUMBER NOT NULL
  );
 
ALTER TABLE Mnozstvi ADD CONSTRAINT fk_objednavka FOREIGN KEY (id_objednavka) 
   REFERENCES Objednavka ON DELETE CASCADE;

ALTER TABLE Mnozstvi ADD CONSTRAINT fk_varka FOREIGN KEY (id_varka) 
   REFERENCES Varka ON DELETE CASCADE;

 CREATE TABLE Firma
 (
  id_firma NUMBER NOT NULL PRIMARY KEY,
  id_odberatel NUMBER NOT NULL,
  ICO NUMBER NOT NULL,
  nazev VARCHAR(50)
);

ALTER TABLE Firma ADD CONSTRAINT fk_odberatel_firma FOREIGN KEY (id_odberatel) 
   REFERENCES Odberatel ON DELETE CASCADE;

 CREATE TABLE Fyzicka_osoba
 (
  id_osoba NUMBER NOT NULL PRIMARY KEY,
  id_odberatel NUMBER NOT NULL,
  jmeno VARCHAR(20) NOT NULL,
  prijmeni VARCHAR(30) NOT NULL
 );

ALTER TABLE Fyzicka_osoba ADD CONSTRAINT fk_odberatel_fyz FOREIGN KEY (id_odberatel) 
   REFERENCES Odberatel ON DELETE CASCADE;









INSERT INTO Druh_caje 
VALUES('0001', 'Zelený');
INSERT INTO Druh_caje 
VALUES('0002', 'Bílý');
INSERT INTO Druh_caje 
VALUES('0003', 'Bylinný');
INSERT INTO Druh_caje 
VALUES('0004', 'Rooibos');


INSERT INTO Zeme 
VALUES('Kolumbie', 'Kolumbie je stát v Jižní Americe, který je součástí karibské Jižní Ameriky. Jako jediná země Jižní Ameriky se rozkládá u obou oceánů, na pobřeží Karibiku u Atlantského oceánu a Tichého oceánu, na jejichž pobřeží leží. Sousedí s Venezuelou, Brazílii, Peru, Ekvádorem a Panamou. Počet obyvatel dosahuje 43 milionů. Hlavním městem je Bogotá. Obyvatelé mluví španělsky a hlásí se ke katolické církvi. V zemi donedávna probíhala válka mezi vládou a drogovými kartely (do roku 2012).');
INSERT INTO Zeme 
VALUES('Brazílie', 'Na území Brazílie se rozkládá největší tropický deštný les na světě – Amazonský prales. Brazílie byla v minulosti největší kolonií Portugalska a portugalština je zde také úředním jazykem. Metropolí Brazílie je město Brasília, plánovitě vybudovaná v letech 1956–1960 v brazilském vnitrozemí. Největším městem s 10 milióny obyvatel je São Paulo.');
INSERT INTO Zeme 
VALUES('Japonsko', 'Jméno Japonska doslova znamená Země vycházejícího slunce: 日 (ni, slunce) 本 (hon, původ) 国 (koku, země). Čínsky se tytéž znaky čtou Ž-pen-kuo, odtud zřejmě pocházejí názvy v  evropských jazycích (francouzské Japon [žapon], anglické Japan [džpen]).
Symbolem Japonska je sopka Fudži (Fudži-san), které nerodilí mluvčí někdy nesprávně říkají Fudži-jama, což vzniká špatným čtením znaku pro horu.');
INSERT INTO Zeme 
VALUES('Čína', 'Čína (tradiční znaky: 中國, zjednodušené znaky: 中国, hanyu pinyin: Zhōngguó, český přepis: Čung-kuo; tento pojem ovšem neznamená „Říše středu“, jak praví žurnalistické klišé: ve starověku se jím označovaly „ústřední státy“, tedy státy seskupené v Centrální planině, které představovaly tradiční kolébku čínské starověké kultury) je geografická oblast a historické území v Asii. Označuje prostor ovládaný čínskými impérii bez ohledu na to, šlo-li o etnicky čínské dynastie.');


INSERT INTO Oblast
VALUES('0001', 'Bogota', 'Kolumbie', 'Hodně drog', 'nic');
INSERT INTO Oblast
VALUES('0002', 'Rio de Janeiro', 'Brazílie', 'Hodně drog', 'nic');
INSERT INTO Oblast
VALUES('0003', 'Šikoku', 'Japonsko', 'Hodne robotů', 'nic');
INSERT INTO Oblast
VALUES('0004', 'Peking', 'Čína', 'Komanči', 'nic');


INSERT INTO Caj
VALUES('0001','0001', '0001', 'Chun Mei', '160', 'Vzácný čaj, který pili staří myslitelé.');
INSERT INTO Caj
VALUES('0002','0001', '0002', 'Bancha Arashiyama', '180', 'Čaj je produkován na podzim. Bancha se vyznačuje silnou chutí, která se volně rozlévá do celé ústní dutiny.');
INSERT INTO Caj
VALUES('0003','0001', '0003', 'Bancha Kagoshima', '160', 'Podzimní čaj z oblasti Kagoshima na jihu Japonska. Jedná se o poslední sklizeň v roce, a proto se na sběr používají stroje.');
INSERT INTO Caj
VALUES('0004','0002', '0004', 'Bai He Xian Zi', '260', 'Složení: pravý bílý čaj, květy lilie, laskavcovky a jasmínu.');
INSERT INTO Caj
VALUES('0005','0002', '0001', 'China Jasmine Dragon Phoenix Pearls', '190', 'Nejjemnější čajové lístky a pupeny jsou ručně svinovány do malých perel a obohaceny vůní čerstvých jasmínových květů.');
INSERT INTO Caj
VALUES('0006','0002', '0002', 'He Jia Huan Le', '130', 'Složení: pravý bílý čaj, květy jasmínu a lilie.');
INSERT INTO Caj
VALUES('0007','0003', '0003', 'Bílý Ibišek ', '200', 'Květy bílého ibišku (Hibisci flos). Podporuje funkci cévní soustavy a ulevuje od tzv. těžkých nohou.Má osvěžující účinek, ovlivňuje tonus a vitalitu.');
INSERT INTO Caj
VALUES('0008','0003', '0004', 'Heřmánek', '180', 'Květy heřmánku (Matricariae flos). Podporuje normální funkci dýchacího systému a imunitu. Má vliv na stav pokožky, trávení a na duševní zdraví.');
INSERT INTO Caj
VALUES('0009','0004', '0001', 'Cesta rájem', '160', 'Zelený bio rooibos (min.76,8%), banán (banán, med, cukr, banánové aroma, kokosový olej), aroma, meloun (meloun, kyselina citrónová), maracuja, měsíček, pivoňka.');
INSERT INTO Caj
VALUES('0010','0004', '0002', 'Jablko z ráje', '220', 'Zelený bio rooibos (min.68,2%), granátové jablko (min.17%), plody dřišťálu obecného, aroma, vousatka citrónová, chrpa.');


INSERT INTO Kvalita
VALUES('European Rating System','Čaje jsou nejprve uvařeny a poté je ochutnávají Francouztí mniši.');
INSERT INTO Kvalita
VALUES('Tea rating','Mezinárodní hodnocení, je posuzována jak velikost lístků, tak chuť čaje.');
INSERT INTO Kvalita
VALUES('Finnest Tea Association','Smetánka mezi čaji, tyhle čaje pro vás vybrali ti největší experti.');

INSERT INTO  Hodnoceni
VALUES('0002','European Rating System', '7');
INSERT INTO  Hodnoceni
VALUES('0004','European Rating System', '9');
INSERT INTO  Hodnoceni
VALUES('0009','European Rating System', '8');
INSERT INTO  Hodnoceni
VALUES('0002','Tea rating', '4');
INSERT INTO  Hodnoceni
VALUES('0001','Finnest Tea Association', '4');
INSERT INTO  Hodnoceni
VALUES('0005','Finnest Tea Association', '10');
INSERT INTO  Hodnoceni
VALUES('0008','Finnest Tea Association', '8');

INSERT INTO  Dodavatel
VALUES('0001', 'Juan', 'Cisco', 'Mladý farmář V Brazílii.', '');
INSERT INTO  Dodavatel
VALUES('0002', 'Lukas', 'Podolski', 'Mladý farmář V Komlumbii.', '');
INSERT INTO  Dodavatel
VALUES('0003', 'Petr', 'Cech', 'Starý farmář V Brazílii.', '');
INSERT INTO  Dodavatel
VALUES('0004', 'Pavel', 'Kagawa', 'Mladý farmář V Číně.', '');
INSERT INTO  Dodavatel
VALUES('0005', 'Mikael', 'Arteta', 'Mladý farmář V Japonsku.', '');
INSERT INTO  Dodavatel
VALUES('0006', 'Lukas', 'Pacer', 'Mladý farmář V Pacelandu.', '');


INSERT INTO  Varka
VALUES('0001', '0001', '0001', TO_DATE('20151506', 'YYYYDDMM'), 'autem', '40', '1090', '0');
INSERT INTO  Varka
VALUES('0002', '0001', '0001', TO_DATE('20151206', 'YYYYDDMM'), 'lodi','50', '2090', '0');
INSERT INTO  Varka
VALUES('0003', '0004', '0002', TO_DATE('20151306', 'YYYYDDMM'), 'autem','40', '1390', '30');
INSERT INTO  Varka
VALUES('0004', '0005', '0002', TO_DATE('20142306', 'YYYYDDMM'), 'letadlem','60', '1590', '20');
INSERT INTO  Varka
VALUES('0005', '0007', '0004', TO_DATE('20151503', 'YYYYDDMM'), 'letadlem','40', '1390', '0');
INSERT INTO  Varka
VALUES('0006', '0008', '0001', TO_DATE('20151304', 'YYYYDDMM'), 'lodi','120', '1190', '20');
INSERT INTO  Varka
VALUES('0007', '0009', '0002', TO_DATE('20151101', 'YYYYDDMM'), 'autem','80', '980', '0');
INSERT INTO  Varka
VALUES('0008', '0010', '0004', TO_DATE('20150303', 'YYYYDDMM'), 'letadlem','40', '1490', '0');
INSERT INTO  Varka
VALUES('0009', '0003', '0001', TO_DATE('20150404', 'YYYYDDMM'), 'lodi','290', '2090', '0');
INSERT INTO  Varka
VALUES('0010', '0002', '0003', TO_DATE('20152004', 'YYYYDDMM'), 'lodi','40', '3090', '10');
INSERT INTO  Varka
VALUES('0011', '0005', '0002', TO_DATE('20153006', 'YYYYDDMM'), 'letadlem','410', '1590', '0');
INSERT INTO  Varka
VALUES('0012', '0006', '0001', TO_DATE('20151706', 'YYYYDDMM'), 'autem','140', '1990', '0');


INSERT INTO Odberatel
VALUES('0001', 'Stefanikova 6654, 76001 Brno', 'petr@seznam.cz', '666666666');
INSERT INTO Odberatel
VALUES('0002', 'Poruba 6, 76003 Brno', 'petr@seznam.cz', '666666666');
INSERT INTO Odberatel
VALUES('0003', 'Semilaso 8743, 76002 Brno', 'petr@seznam.cz', '666666666');
INSERT INTO Odberatel
VALUES('0004', 'Modrá 65, 76003 Brno', 'petr@seznam.cz', '666666666');
INSERT INTO Odberatel
VALUES('0005', 'Tvarůžková 1562, 76008 Olomouc', 'petr@seznam.cz', '666666666');
INSERT INTO Odberatel
VALUES('0006', 'Zelená 654, 76003 Olomouc', 'petr@seznam.cz', '666666666');
INSERT INTO Odberatel
VALUES('0007', 'Olomoucká 6654, 76000 Olomouc', 'petr@seznam.cz', '666666666');


INSERT INTO Objednavka
VALUES('00000001', '0001', TO_DATE('20152005', 'YYYYDDMM'), 'Vyřízeno');
INSERT INTO Objednavka
VALUES('00000002', '0002', TO_DATE('20152105', 'YYYYDDMM'), 'Vyřízeno');
INSERT INTO Objednavka
VALUES('00000003', '0004', TO_DATE('20150806', 'YYYYDDMM'), 'Vyřízeno');
INSERT INTO Objednavka
VALUES('00000004', '0002', TO_DATE('20151806', 'YYYYDDMM'), 'Čeká na vyřízení');
INSERT INTO Objednavka
VALUES('00000005', '0001', TO_DATE('20150806', 'YYYYDDMM'), 'Čeká na vyřízení');
INSERT INTO Objednavka
VALUES('00000006', '0001', TO_DATE('20152009', 'YYYYDDMM'), 'Vyřízeno');
INSERT INTO Objednavka
VALUES('00000007', '0005', TO_DATE('20152405', 'YYYYDDMM'), 'Vyřízeno');
INSERT INTO Objednavka
VALUES('00000008', '0003', TO_DATE('20152002', 'YYYYDDMM'), 'Expeduje');
INSERT INTO Objednavka
VALUES('00000009', '0004', TO_DATE('20151102', 'YYYYDDMM'), 'Expeduje');
INSERT INTO Objednavka
VALUES('00000010', '0006', TO_DATE('20150808', 'YYYYDDMM'), 'Expeduje');
INSERT INTO Objednavka
VALUES('00000011', '0007', TO_DATE('20152102', 'YYYYDDMM'), 'Vyřízeno');

INSERT INTO Mnozstvi
VALUES('00000001', '0001', '40', '1000');
INSERT INTO Mnozstvi
VALUES('00000001', '0004', '40', '1400');
INSERT INTO Mnozstvi
VALUES('00000002', '0002', '20', '1200');
INSERT INTO Mnozstvi
VALUES('00000002', '0005', '10', '900');
INSERT INTO Mnozstvi
VALUES('00000002', '0001', '4', '1010');
INSERT INTO Mnozstvi
VALUES('00000002', '0006', '40', '1070');
INSERT INTO Mnozstvi
VALUES('00000003', '0003', '4', '1900');
INSERT INTO Mnozstvi
VALUES('00000004', '0002', '30', '1290');
INSERT INTO Mnozstvi
VALUES('00000005', '0001', '10', '890');
INSERT INTO Mnozstvi
VALUES('00000006', '0004', '14', '1040');
INSERT INTO Mnozstvi
VALUES('00000007', '0006', '12', '1300');
INSERT INTO Mnozstvi
VALUES('00000008', '0002', '8', '1700');
INSERT INTO Mnozstvi
VALUES('00000009', '0003', '5', '10810');
INSERT INTO Mnozstvi
VALUES('00000010', '0004', '20', '1021');
INSERT INTO Mnozstvi
VALUES('00000011', '0001', '10', '1287');

INSERT INTO Firma
VALUES('0001', '0001', '8801037786', 'Příšerky s.r.o');
INSERT INTO Firma
VALUES('0002', '0003', '8801037786', 'Příšerky s.r.o');
INSERT INTO Firma
VALUES('0003', '0005', '8801037786', 'Příšerky s.r.o');

INSERT INTO Fyzicka_osoba
VALUES('0001', '0002', 'Marek', 'Mokrý');
INSERT INTO Fyzicka_osoba
VALUES('0002', '0004', 'Stanislav', 'Suchý');
INSERT INTO Fyzicka_osoba
VALUES('0003', '0006', 'Petr', 'Prasklý');
INSERT INTO Fyzicka_osoba
VALUES('0004', '0007', 'Jan', 'Jasný');





--Tento dotaz vybere z tabulky Caj jednotlivé čaje a k nim z tabulky Druh_caje druh čaje. 
SELECT Caj.nazev_caj, Druh_caje.nazev_druh FROM Caj INNER JOIN Druh_caje ON Caj.id_druh = Druh_caje.id_druh;

--Tento dotaz vybere z tabulky Oblast jednotlivé oblasti a k nim z tabulky Zeme zemi. 
SELECT Oblast.nazev_oblast, Zeme.nazev_zeme FROM Oblast INNER JOIN Zeme ON Oblast.nazev_zeme = Zeme.nazev_zeme;

--Tento dotaz vybere všechny čaje a k nim přiřadí jejich oblast, ke které následně připojí i zemi.
SELECT Caj.nazev_caj, Oblast.nazev_oblast, Zeme.nazev_zeme FROM Oblast INNER JOIN Zeme ON Oblast.nazev_zeme = Zeme.nazev_zeme INNER JOIN Caj ON Caj.id_oblast = Oblast.id_oblast ORDER BY Caj.nazev_caj;

--Tento dotaz vypíše ke každé oblasti počet čajů, které odtud pocházejí.
SELECT Oblast.nazev_oblast, COUNT(*) AS pocet_caju FROM Caj INNER JOIN Oblast ON Oblast.id_oblast = Caj.id_oblast GROUP BY Oblast.nazev_oblast;

--Tento dotaz vypíše ke každému čaji počet várek, ve kterých se tento čaj vyskytuje.
SELECT Caj.nazev_caj, COUNT(*) AS pocet_varek FROM Varka INNER JOIN Caj ON Caj.id_caj = Varka.id_caj GROUP BY Caj.nazev_caj;

--Tento dotaz vypíše všechny objednávky, pokud existuje nějaká objednávka, která má svého odběratele.
SELECT * FROM Objednavka WHERE EXISTS (SELECT * FROM Odberatel INNER JOIN Objednavka ON Odberatel.id_odberatel = Objednavka.id_odberatel WHERE Odberatel.id_odberatel = 1);

--Tento dotaz vypíše všechny čaje, které jsou ve várce, jejíž dodavatelem je někdo, jehož křestní jméno je Lukas.
SELECT Caj.nazev_caj FROM Caj WHERE Caj.id_caj IN (SELECT Varka.id_caj FROM Varka INNER JOIN Dodavatel ON Dodavatel.id_dodavatel = Varka.id_dodavatel WHERE Dodavatel.jmeno = 'Lukas');
8/8

Dostali jste několik souborů, které jsou šifrovány neznámou synchronní proudovou šifrou. Vašim cílem je zjistit tajemství, které má formát KRY{24 znaků ASCII textu}.

Programovací jazyk je Python 3. Odevzdvávejte archiv xlogin00.zip. Archiv bude obsahovat solution.py (soubor s obsahem ručního řešení), solution_sat.py (soubor s obsahem sat řešení), doc.pdf (dokumentace) a volitelně install.sh (pro automatickou instalaci závislostí, pouze pomocí pip3, pouze pro SAT řešení, z3 bude již nainstalován). Skripty po spuštění vypíší na stdout tajemství. Skripty budou pracovat s jedním argumentem. Argument bude cílová cesta k adresáři ve které bude rozbalený obsah archivu zadání (neodevzdávejte) a bude zjišťovat tajemství ze souborů v této složce. Pokud skript potřebuje závislosti, musí si je automaticky doinstalovat pomocí install.sh (install.sh bude spuštěn před spuštěním testu). Program vypíše tajemství na stdout a skončí. Vaše řešení bude testováno na Ubuntu 18.04 LTS.

V druhé části zadaní získejte tajemství pomocí SAT solveru aplikovaného na spravnou část šifry. 
https://www.cs.cornell.edu/gomes/pdf/2008_gomes_knowledge_satisfiability.pdf
https://yurichev.com/writings/SAT_SMT_draft-EN.pdf

Bodování: 50% za ruční řešení, 30% za SAT solver a 20% za dokumentaci (bez dokumentace za 0b).
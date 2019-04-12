#include "imapcl.h"

/* Konstruktor zpracující argumenty. */
Imapcl::Imapcl(int argc, char *argv[])
{
	this->bio = NULL;
	this->ctx = NULL;
	this->ssl = NULL;
	this->port = 0;
	this->savedMessages = 0;
	this->isSSL = false;
	this->justNewMessages = false;
	this->justHeaders = false;
	this->doneReading = false;
	if (argc < 2)
		throw std::runtime_error("Je třeba zadat povinné parametry.");
	// Zpracovani jednotlivych argumentu
	for (int i = 1; i < argc; i++)
	{
		// Pokud je zadan argument -p, nasledujici argument musi byt kladne cislo portu, jinak chyba.
		if (strcmp(argv[i], "-p") == 0)
		{
			if (argc <= i+1)
				throw std::runtime_error("Chybí číslo portu.");			
			this->port = std::stoi(argv[i+1]);
			if (this->port < 1)
				throw std::runtime_error("Čislo portu musí být kladné číslo.");			
			i++;	
		}
		// Pokud je zadan argument -T, zapne se sifrovane spojeni.
		else if (strcmp(argv[i], "-T") == 0)
			this->isSSL = true;
		// Pokud je zadan argument -c, nasledujici argument musi byt jmeno souboru s certifikatem, jinak chyba.
		else if (strcmp(argv[i], "-c") == 0)
		{
			if (argc <= i+1 || argv[i+1][0] == '-')
				throw std::runtime_error("Chybí název certifikátu.");
			this->certfile = argv[i+1];
			i++;
		}
		// Pokud je zadan argument -C, nasledujici argument musi byt cesta pro slozku s certifikatem, jinak chyba.
		else if (strcmp(argv[i], "-C") == 0)
		{
			if (argc <= i+1 || argv[i+1][0] == '-')
				throw std::runtime_error("Chybí cesta k certifikátu.");
			this->certpath = argv[i+1];
			i++;
		}
		// Pokud je zadan argument -n, zapne se stahovani pouze novych e-mailu.
		else if (strcmp(argv[i], "-n") == 0)
			this->justNewMessages = true;
		// Pokud je zadan argument -h, zapne se stahovani pouze hlavicek e-mailu.
		else if (strcmp(argv[i], "-h") == 0)
			this->justHeaders = true;
		// Pokud je zadan argument -a, nasledujici argument musi byt cesta k souboru s prihlasovacimi udaji, jinak chyba.
		else if (strcmp(argv[i], "-a") == 0)
		{
			if (argc <= i+1 || argv[i+1][0] == '-')
				throw std::runtime_error("Chybí soubor s autentizačními údaji.");
			this->authfile = argv[i+1];
			this->loadAuthFile();
			i++;
		}
		// Pokud je zadan argument -b, nasledujici argument musi byt jmeno schranky s e-maily, jinak chyba.
		else if (strcmp(argv[i], "-b") == 0)
		{
			if (argc <= i+1 || argv[i+1][0] == '-')
				throw std::runtime_error("Chybí název složky s e-maily.");
			this->inbox = argv[i+1];
			i++;
		}
		// Pokud je zadan argument -o, nasledujici argument musi byt nazev slozky pro vystup, jinak chyba.
		else if (strcmp(argv[i], "-o") == 0)
		{
			if (argc <= i+1 || argv[i+1][0] == '-')
				throw std::runtime_error("Chybí název složky, do které se mají ukládat e-maily.");
			this->outputDirectory = argv[i+1];
			i++;
		}
		else
		{
			this->server = argv[i];
		}		
	}
	// Pokud nebyl zadan parametr pro vystupni slozku, chyba.
	if (this->outputDirectory.empty())
		throw std::runtime_error("Parametr pro složku, kam se mají ukládat e-maily, je povinný.");
	else
	{
		if (this->outputDirectory.back() != '/')
			this->outputDirectory += "/";
		DIR* dir = opendir(this->outputDirectory.c_str());
		// Pokud slozka pro vystup existuje
		if (dir)
		{
			closedir(dir);
		}
		// Pokud slozka neexistuje, program se ji pokusi vytvorit. Pokud se to nepovede, chyba.
		else if (ENOENT == errno)
		{
			if (mkdir(this->outputDirectory.c_str(), 0777) == -1)
				throw std::runtime_error("Nepodařilo se vytvořit složku.");
		}
		else
		{
			throw std::runtime_error("Nepodařilo se otevřít složku.");
		}
	}
	// Pokud nebyl zadan parametr pro soubor s prihlasovacimi udaji, chyba.
	if (this->authfile.empty())
		throw std::runtime_error("Parametr pro soubor s autentizačními údaji je povinný.");
	// Pokud nebyl zadan parametr pro nazev serveru, chyba.
	if (this->server.empty())
		throw std::runtime_error("Parametr pro název serveru je povinný.");
	// Pokud nebyl zadan parametr pro schranku s e-maily, nastavi se defaultne INBOX.
	if (this->inbox.empty())
		this->inbox = "INBOX";
	// Pokud nebyl zadan parametr pro port, nastavi se defaultne 993 pro sifrovane a 143 pro nesifrovane spojeni.
	if (this->port <= 0)
	{
		if (this->isSSL)
			this->port = 993;
		else
			this->port = 143;
	}		
	if (this->certpath.empty())
		this->certpath = "/etc/ssl/certs/";
	else
	{
		if (this->certpath.back() != '/')
			this->certpath += "/";
	}
}

/* Funkce, ktera vrati prihlasovaci udaje nactene ze souboru specifikovaneho parametrem -a. */
std::string Imapcl::loadAuthFile()
{	
	std::ifstream infile(this->authfile);	
	if (infile.fail())
		throw std::runtime_error("Soubor s přihlašovacími údaji neexistuje.");
	std::string line;
	std::string login;
	size_t pos;
	while (std::getline(infile, line))
	{
		if ((pos = line.find("username: ")) != std::string::npos)
		{
			line.erase(0, 10);
			login += line;
		}
		else if ((pos = line.find("password: ")) != std::string::npos)
		{
			line.erase(0, 10);
			login += line;
		}		
		// Pokud nejsou radky v korektnim formatu, chyba.
		else
			throw std::runtime_error("Nelze přečíst soubor s přihlašovacími údaji.");
		login += " ";
    }  
    return login;
}

/* Funkce, ktera otevre spojeni se serverem. Vraci true pri uspechu, false pri neuspechu. */
bool Imapcl::openConnection()
{
	OpenSSL_add_all_algorithms();
	std::ostringstream connection;
	// Nacteni certifikatu, pokud ma byt spojeni zasifrovane
	if (this->isSSL)
	{
		SSLeay_add_ssl_algorithms();
		SSL_CTX * ctx = SSL_CTX_new(SSLv23_client_method());	
		if(! SSL_CTX_load_verify_locations(ctx, this->certfile.empty() ? NULL : this->certfile.c_str(), this->certpath.empty() ? NULL : this->certpath.c_str()))
			throw std::runtime_error("Nelze načíst soubor s certifikátem.");
		bio = BIO_new_ssl_connect(ctx);
		BIO_get_ssl(bio, & ssl);
		SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);
	}
	char temp[BUFFER];	
	connection.clear();
	connection.str("");
	connection << this->server << ":" << this->port;
	// Nastaveni pro nesifrovane spojeni	
	if (!this->isSSL)
	{
		char * b = new char [connection.str().length()+1];
		strcpy(b, connection.str().c_str());
		bio = BIO_new_connect(b);
		delete[] b;

	}
	else
		BIO_set_conn_hostname(bio, connection.str().c_str());
	if (bio == NULL)
	    throw std::runtime_error("Připojení se nezdařilo. Zkontrolujte parametry.");
	if (BIO_do_connect(bio) <= 0)
		throw std::runtime_error("Připojení se nezdařilo. Zkontrolujte parametry.");	
	BIO_set_nbio(bio, 0);
	if (this->isSSL && SSL_get_verify_result(ssl) != X509_V_OK)
		throw std::runtime_error("Není možné ověřit identitu serveru " + this->server + ".");
	memset(temp, '\0', sizeof(temp)-1);
	int x = BIO_read(bio, temp, BUFFER - 1);
	if (x == 0)
	    throw std::runtime_error("Server neodpověděl.");
	else if (x < 0)
	{
		if (!BIO_should_retry(bio))
			throw std::runtime_error("Server neodpověděl.");	 
	    throw std::runtime_error("Server neodpověděl. Je třeba pokus opakovat.");
	}
	char *result = strstr(temp, " OK ");
	connection.clear();
	connection.str("");
	if (result)
	{
		memset(temp, '\0', sizeof(char)*BUFFER);
		return true;
	}
	else
	{
		memset(temp, '\0', sizeof(char)*BUFFER);
		return false;
	}		
}

/* Uzavre spojeni se serverem. */
void Imapcl::closeConnection()
{	
	if (this->bio)
	{
		(void) BIO_reset(bio);
		BIO_free_all(bio);
	}	
	if (this->ctx)		
	{		
		ERR_remove_state(0);
		ERR_free_strings();
		EVP_cleanup();
		CRYPTO_cleanup_all_ex_data();
		SSL_CTX_free(ctx);
	}
}


/* Funkce, ktera tvori requesty na IMAP server. */
void Imapcl::service()
{
	// Interni cislovani instrukci
	this->instructionCount = 419;
	std::ostringstream message;
	// Prihlaseni k serveru se zadanymi udaji
	message.str("");
	message.clear();
	this->buf.clear();
	message << "a" << this->instructionCount << " LOGIN " << this->loadAuthFile() << "\r\n";
	if (!this->sendRequestGetResponse(message.str()))
		throw std::runtime_error("Nepodařilo se přihlásit se k serveru. Zkontrolujte přihlašovací údaje, nebo se ujistěte, zda server na zadaném portu podporuje plaintextové přihlášení.");	
	this->instructionCount++;
	// Vyber schranky
	message.str("");
	message.clear();
	this->buf.clear();		
	message << "a" << this->instructionCount << " SELECT " << this->inbox << "\r\n";
	if (!this->sendRequestGetResponse(message.str()))
		throw std::runtime_error("Nepodařilo se otevřít složku " + this->inbox + " v e-mailové schránce.");
	this->instructionCount++;
	message.str("");
	message.clear();
	std::vector<std::string> ids;
	// V pripade stazeni pouze novych e-mailu je nutne pozadat server o seznam ID novych e-mailu
	if (this->justNewMessages)
	{
		message.str("");
		message.clear();	
		message << "a" << this->instructionCount << " SEARCH UNSEEN" << "\r\n";	
		if (!this->sendRequestGetResponse(message.str()))
			throw std::runtime_error("Nepodařilo se vyhledat nové e-maily.");		
		ids = this->getNewEmails();
		if (ids.size() < 1)
		{
			this->printResult();
			return;
		}
	}
	message.str("");
	message.clear();
	this->buf.clear();
	instructionCount++;		
	if (this->justHeaders)
	{
		message << "a" << this->instructionCount << " FETCH ";
		if (this->justNewMessages)
		{
			for (std::vector<std::string>::const_iterator i = ids.begin(); i != ids.end(); ++i)
			{
				message << *i;
				if (i+1 != ids.end())
					message << ",";
			}											
		}
		else
			message << "1:*";
		message << " (FLAGS BODY[HEADER])\r\n";
	}
	else
	{
		message << "a" << this->instructionCount << " FETCH ";
		if (this->justNewMessages)
		{
			for (std::vector<std::string>::const_iterator i = ids.begin(); i != ids.end(); ++i)
			{
				message << *i;
				if (i+1 != ids.end())
					message << ",";
			}										
		}
		else
			message << "1:*";
		message << " (FLAGS BODY[HEADER] BODY.PEEK[TEXT])\r\n";
	}	
	if (!this->sendRequestGetResponse(message.str()))
		throw std::runtime_error("Nepodařilo se stáhnout zadané e-maily.");
	this->doneReading = true;
	this->instructionCount++;
	this->saveEmail();
	message.str("");
	message.clear();
	// Logout
	message << "a" << this->instructionCount << " LOGOUT\r\n";
	if (!this->sendRequestGetResponse(message.str()))
		throw std::runtime_error("Nepodařilo se odhlásit.");
	message.str("");
	message.clear();
	this->buf.clear();
	this->printResult();
}

/* Funkce, ktera z odpovedi serveru vybere uid novych zprav a vrati je ve vectoru. */
std::vector<std::string> Imapcl::getNewEmails()
{
	std::string message;
	std::vector<std::string> ids;
	size_t begin = this->buf.find("SEARCH");
	if (begin != std::string::npos)
	{
		// Posunuti pozice za "SEARCH"
		begin = begin + 7;
		while(true)
		{
			// Mezera znaci oddeleni uid	
			if (isspace(buf.at(begin)))
			{
				if (!message.empty())
					ids.push_back(message);
				message.clear();
				begin = begin + 1;
				// Pokud se program dostane az na pozici, kde je konec radku, parsovani skonci.
				if (buf[begin-1] == '\n')
					break;
				continue;
			}
			message += buf[begin];
			begin = begin + 1; 
		}		
	}
	return ids;
}

/* Funkce, ktera zpracuje celou odpoved od serveru a ulozi e-maily do jednotlivych souboru. Vrati pocet ulozenych e-mailu. */
void Imapcl::saveEmail()
{
    // Pomocny stringstream, ktery bude slouzit hlavne k vyhledavani v bufferu
    std::ostringstream temp;
    std::regex headerEnd("FETCH FLAGS.+}\r\n");
    int length = 0;
    while (true)
    {
    	temp.str("");
		temp.clear();
		// Vyhledani zacatku zpravy (zpravy zacinaji IMAP hlavickou)
		temp << " FETCH (FLAGS";
	    auto begin = this->buf.find(temp.str());
	    if (begin == std::string::npos)
	    {
	    	break;	    	
	    }
	    if (!this->doneReading)
	    {
	    	if (this->buf.find(temp.str(), begin + 1) == std::string::npos)
	    		break;	    	
	    }	    
	    // Pokud jiz zadna zprava v bufferu neni, ukladani skonci	      	
	    auto tmp = begin;
	    std::string messageId;
	    // Nalezeni ID zpravy (nachazi se za hvezdickou)
	    while (this->buf[tmp--] != '*')
	    {
	    	if (this->buf[tmp] != ' ' && this->buf[tmp] != '*')
	    		messageId += this->buf[tmp];
	    }
	    // Otoceni stringu - nacitani ID zpravy probihalo obracene
	    std::reverse(messageId.begin(), messageId.end());
	    size_t end = 0;
	    // Nalezeni konce IMAP hlavicky
	    end = this->buf.find("}\r\n", 0);
	    // Pokud se maji stahovat pouze hlavicky e-mailu, je nutne zjistit delku hlavicky
	    if (this->justHeaders)
	    {
	    	std::string lengthString = "";
	    	tmp = end-1;
	    	// Je nalezeny konec IMAP hlavicky - velikost se nachazi v hranatych zavorkach na tomto konci
	    	while (this->buf[tmp] != '{')
	    		lengthString += this->buf[tmp--];
	    	std::reverse(lengthString.begin(), lengthString.end());
	    	length = std::stoi(lengthString);
	    	// Zajisti smazani prazdneho radku
	    	tmp = -2;
	    }
	    // Posunuti pozice end o tri znaky - \r\n) - tedy na zacatek samotne zpravy
	    end += 3;
	    // Smazani IMAP hlavicky
		this->buf.erase(0, end);
		temp.str("");
		temp.clear();
		// Pokud se maji stahovat cele zpravy, je treba zjistit, jak dlouha zprava je
		if (!this->justHeaders)
		{
			begin = this->buf.find(" BODY[TEXT] {");
			if (begin == std::string::npos)
				break;			
			// Posunuti o " BODY[TEXT] {"
			tmp = begin + 13;
			// Nacteni velikosti zpravy
			while (this->buf[tmp] != '}')	
				temp << this->buf[tmp++];
			// Posunuti o CLRF na konci body
			tmp+=3;
			length = std::stoi(temp.str());
		}		
		// Pokud je v bufferu vice nebo rovno znaku, nez je udajna velikost zpravy
		if ((this->buf.size()-tmp) >= (size_t)length)
		{
			// Vyber samotne zpravy
			std::string message = this->buf.substr(0, tmp+length);
			// Smazani zpravy z bufferu			
			this->buf.erase(0, tmp+length);
			// Pokud jsou stazeny cele zpravy, smazani BODY[TEXT] {velikost} z jiz separovaneho e-mailu
			if (!this->justHeaders)
				message.erase(begin, tmp-begin);
			// Zapis do souboru
			std::ofstream out(this->outputDirectory+messageId, std::ios::out | std::ios::trunc);
		    out << message;
		    out.close();
		    savedMessages++;
		    temp.str("");
			temp.clear();
		}	
    }    
}

/* Odesle zadanou zpravu, prijme odpoved a pripoji ji k bufferu. */
bool Imapcl::sendRequestGetResponse(std::string request)
{
	buf.clear();	
	if (BIO_write(bio, &request[0], request.length()) <= 0)
	{
		if(! BIO_should_retry(bio))
	    	throw std::runtime_error("Neporařilo se odeslat zprávu na server.");
	}	
	buf.clear();
	char temp[BUFFER];
	int r;
	std::ostringstream ending;
	ending << "a" << this->instructionCount << " " << "(OK)";
	std::regex success(ending.str());
	ending.str("");
	ending.clear();
	ending << "a" << this->instructionCount << " " << "(BAD|NO)";
	std::regex failure(ending.str());
	while (1)
	{
		// Pokud je velikost bufferu cca 50 MB, zpracuje se
		if (this->buf.size() > 50000000)
			this->saveEmail();			
		memset(temp, '\0', sizeof(char)*BUFFER);
		r = BIO_read(bio, temp, BUFFER-1);
		if (r == -1)
			throw std::runtime_error("Chyba při čtení ze serveru.");
		if (r < 1)
			break;		
		temp[r] = '\0';		
		buf+=temp;	
		// Pokud je v odpovedi od serveru ukoncujici znacka instrukce, cteni se ukonci.
		if (std::regex_search(&temp[0], &temp[r], success))
			return true;									
		if (std::regex_search(&temp[0], &temp[r], failure))
			return false;	
	}
	return true;
}

/* Funkce, ktera vypise informace o stazenych zpravach. */
void Imapcl::printResult()
{
	switch (this->savedMessages)
	{
		case 1:
			std::cout << "Stažena 1 " << (this->justNewMessages ? "nová " : "") << (this->justHeaders ? "hlavička zprávy " : "zpráva ") << "ze složky " << this->inbox << ".\n";
			break;
		case 2:
			std::cout << "Staženy 2 " << (this->justNewMessages ? "nové " : "") << (this->justHeaders ? "hlavičky zpráv " : "zprávy ") << "ze složky " << this->inbox << ".\n";
			break;
		case 3:
			std::cout << "Staženy 3 " << (this->justNewMessages ? "nové " : "") << (this->justHeaders ? "hlavičky zpráv " : "zprávy ") << "ze složky " << this->inbox << ".\n";
			break;
		case 4:
			std::cout << "Staženy 4 " << (this->justNewMessages ? "nové " : "") << (this->justHeaders ? "hlavičky zpráv " : "zprávy ") << "ze složky " << this->inbox << ".\n";
			break;
		default:
			std::cout << "Staženo " << this->savedMessages << " " << (this->justNewMessages ? "nových " : "") << (this->justHeaders ? "hlaviček " : "") << "zpráv ze složky " << this->inbox << ".\n";
	}
}

/* Funkce vypisujici napovedu. */
void Imapcl::printHelp()
{
	std::cout << "ISA projekt: IMAP klient, autor: Jan Pawlus" << std::endl;
	std::cout << "-------------------------------------------" << std::endl;
	std::cout << "Aplikaci lze spouštět s parametry:" << std::endl;	
	std::cout << "[server] - povinný parametr. Udává, na který server se má klient připojit." << std::endl;
	std::cout << "-p [port] - nepovinný parametr. Při nezadání se vybere vhodný port." << std::endl;
	std::cout << "-b [schranka] - nepovinný parametr. Při nezadání se defaultně stáhnou e-maily ze schránky INBOX." << std::endl;
	std::cout << "-o [slozka] - povinný parametr. Složka, kam se maji ukládat e-maily (v pripade neexistujici slozky se ji klient pokusi vytvorit)." << std::endl;
	std::cout << "-a [auth_file] - povinný parametr. Udává soubor s autentizačními údaji." << std::endl;
	std::cout << "-T - nepovinný parametr. Zapíná zabezpečené připojení." << std::endl;
	std::cout << "-c - nepovinný parametr. Udává název souboru s certifikátem." << std::endl;
	std::cout << "-C - nepovinný parametr. Udáva složku, kde se má najít určeny soubor s certifikátem. Pokud není zadáno, bude se hledat v /etc/ssl/certs." << std::endl;
	std::cout << "-h - nepovinný parametr. Zapíná stažení pouze hlaviček e-mailu." << std::endl;
	std::cout << "-n - nepovinný parametr. Zapíná stažení pouze nových zpráv." << std::endl;
}
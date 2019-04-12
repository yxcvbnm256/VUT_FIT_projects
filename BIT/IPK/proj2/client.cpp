#include <iostream>
#include <unistd.h>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sstream>
#include <fstream>
#include <ctime>
#include <string>
#include <stdlib.h>
#include <memory.h>
#include <netdb.h>

/* Trida ArgParser se stara o parsovani argumentu */
class ArgParser
{
	private:
		std::string scheme;
		std::string host;
		std::string port;
		std::string path;
	public:
		ArgParser(char**,int);

		void printHelp()
		{
			std::cout << "IPK HTTP client - pouziti" << std::endl;
			std::cout << "	./ipkhttpclient -h - vytiskne napovedu" << std::endl;
			std::cout << "	./ipkhttpclient URI - presne specifikovana struktura informace o pripojeni k serveru a o pozadavcich (napr. http://host:port/soubor.html)" << std::endl;
			exit(0);
		}

		void uriParse(char** argv)
		{
			std::string uri = argv[1];
			std::size_t pos = uri.find(":");
			this->scheme = uri.substr(0, pos+3);
			uri.erase(0, pos+3);	
			pos = uri.find(":");
			this->host = uri.substr(0,pos);
			uri.erase(0, pos+1);
			pos = uri.find("/");
			if (pos == std::string::npos)
			{				
				this->port = uri;
				this->path = "/";
			}
			else
			{				this->port = uri.substr(0,pos);
				uri.erase(0, pos+1);
				this->path = "/";
				this->path.append(uri);
			}	
		}

		std::string getScheme()
		{
			return this->scheme;
		}

		std::string getPort()
		{
			return this->port;
		}

		std::string getHost()
		{
			return this->host;
		}

		std::string getPath()
		{
			return this->path;
		}
		
};

ArgParser::ArgParser(char** argv, int argc)
{
	int c;
	while ((c = getopt (argc, argv, "h")) != -1)
	{
	   switch (c)
	    {
	      	case 'h':
	        	this->printHelp();
	        	break;	 
	        default:
	      		fprintf(stderr,"Neznamy argument\n");
	      		exit(1);
	      		break;
	    }
	}
	this->uriParse(argv);	
}

/* Trida client reprezentuje strukturu clienta */
class Client
{
	private:
		std::string scheme;
		std::string host;
		std::string port;
		std::string path;
		std::string outComm;
		std::string inComm;
		std::string timeNow;
		std::string data;
		std::string header;
	public:
		Client(std::string, std::string, std::string, std::string);
		/* Metoda generujici hlavicku HTTP requestu odesilaneho serveru */
		void generateHeader()
		{
			this->header = "GET ";
			this->header.append(this->path);
			this->header.append(" HTTP/1.1\n");
			this->header.append("Host: ");
			this->header.append(this->host);
			this->header.append("\n");
		}
		/* Metoda pro tvorbu logu */
		void createLog()
		{
			std::string filename = "ipkHttpClient-";
			filename.append(this->timeNow);
			std::string fileIn = filename;
			fileIn.append(".in.log");
			std::string fileOut = filename;
			fileOut.append(".out.log");
			std::ofstream in(fileIn);
			in.clear();
    		in << this->inComm;
    		in.close();
    		std::ofstream out(fileOut);
			out.clear();
    		out << this->outComm;
    		out.close();
    		filename.clear();
    		filename = "ipkResp-";
    		filename.append(this->timeNow);
    		fileOut.clear();
    		fileOut = filename;
    		fileOut.append(".header");
    		std::ofstream header(fileOut);
			header.clear();
    		header << this->header;
    		header.close();
    		fileIn = filename;
    		fileIn.append(".payload");
    		std::ofstream result(fileIn);
			result.clear();
    		result << this->data;
    		result.close();
		}
		/* Metoda, ktera ulozi cas pri spusteni programu (kvuli logum) */
		void setTime()
		{
			std::string timee;
			std::time_t t = std::time(NULL);
		    char mbstr[100];
		    if (std::strftime(mbstr, sizeof(mbstr), "%F:%T", std::localtime(&t))) 
		    	this->timeNow = mbstr;
		}
		/* Metoda, ktera prijima data do te doby, nez zprava neobsahuje delku 0 a pridava postupne relevantni obsah */
		void getChunkedData(int s, int length)
		{
			int n;
			std::string temp;
			while (true)
			{
				char* buffer = (char*)malloc(sizeof(char)*2);
				if ( ( n = read(s, buffer, 1 ) ) <0)
				{
	   				perror("error on read\n"); exit(1);
				}
				buffer[1] = '\0';				
				temp.append(buffer);
				if (temp.find("\r\n") == std::string::npos) continue;
				else
				{   
					this->inComm.append("\r\n");
					this->inComm.append(temp);				
    				length = std::strtoul(temp.data(), NULL, 16 );
    				temp.clear();    				
					free(buffer);
					if (length == 0) break;
					buffer = (char*)malloc(sizeof(char)*length + sizeof(char)*5);
					if ( ( n = read(s, buffer, length+5 ) ) <0)
					{
	   					perror("error on read\n"); exit(1);
					}					
					buffer[length] = '\0';
					this->inComm.append(buffer);
					this->data.append(buffer);
					free(buffer);
				}				
			}
		}
		/* Metoda aktivujici klienta, pripojeni se k serveru a zjisteni relevantnich informaci o odpovedi */
		void activateClient()
		{
			int s, n;
			struct sockaddr_in sin; struct hostent *hptr;
			if ((s = socket(PF_INET, SOCK_STREAM, 0 ) ) < 0)
			{
				perror("error on socket");
    			exit(1);
  			}
			sin.sin_family = PF_INET;
			sin.sin_port = htons(std::stoi(this->port));
			char *hostChar = (char*)malloc(sizeof(char)*this->host.length() + sizeof(char));
			this->host.copy(hostChar, this->host.length(), 0);
			hostChar[this->host.length()] = '\0';
			if ((hptr = gethostbyname(hostChar)) == NULL)
			{
				fprintf(stderr, "gethostname error: %s\n", hostChar);
				exit(1);
   			}
   			free(hostChar);
   			int k;
			memcpy( &sin.sin_addr, hptr->h_addr, hptr->h_length);
			k = connect (s, (struct sockaddr *)&sin, sizeof(sin) );
			if (k < 0 )
			{
				perror("error on connect\n"); close(s); exit(1);
			}
			this->generateHeader();
			char msg[1024];
			this->header.copy(msg, this->header.length(), 0);
			msg[this->header.length()] = '\0';
			if (write(s, msg, strlen(msg) +1) < 0 ) 
			{
				perror("error on write\n"); close(s); exit(1);
			}	
			this->outComm.append(msg);	
			if ( ( n = read(s, msg, sizeof(msg) ) ) <0)
			{  /* read message from server */
   				perror("error on read\n"); exit(1); close(s); 
			}
			std::string message = msg;
			this->inComm.append(message);
			std::size_t pos = message.find("Content-Length: ");
			message.erase(0,pos+16);
			pos = message.find("\n");
			int length = std::stoi(message.substr(0,pos));			
			message = msg;
			/* Pokud se v hlavicce nevyskytuje tento radek, bude se obsah prijimat v celku */
			if (message.find("Transfer-Encoding: chunked") == std::string::npos)
			{
				char *content = (char*)malloc(sizeof(char)*length);
				if ( ( n = read(s, content, length ) ) <0)
				{
	   				perror("error on read\n"); close(s); exit(1);
				}
				content[length] = '\0';
				this->inComm.append(content);
				this->data = content;
			}
			else this->getChunkedData(s,length);
			if (close(s) < 0)
			{ 
    			perror("error on close\n");
    			exit(1);
			}
			this->createLog();			
		}
};
/* Konstruktor, ktery si uklada relevantni informace z ArgParseru */
Client::Client(std::string scheme, std::string host, std::string port, std::string path)
{
	this->scheme = scheme;
	this->host = host;
	this->port = port;
	this->path = path;
	this->setTime();
}

int main(int argc,char** argv)
{
	ArgParser parser(argv,argc);
	Client client(parser.getScheme(), parser.getHost(), parser.getPort(), parser.getPath());
	client.activateClient();	
}
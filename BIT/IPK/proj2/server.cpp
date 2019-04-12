#include <iostream>
#include <unistd.h>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sstream>
#include <fstream>
#include <ctime>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
int t;
int s;

/* Funkce, ktera obsluhuje signal SIGINT */		
void my_function(int k)
{
	close(s);
	close(t);        
    exit(k);
}

/* Trida reprezntujici strukturu serveru */
class Server 
{
	private:
		int port;
		std::string file;
		std::string method;
		std::string outComm;
		std::string inComm;
		std::string timeNow;
		unsigned int chunkSize;
		int chunkTime;
		bool chunked;

	public:
		Server(int, int, int);
		/* Metoda, ktera ulozi cas pri spusteni programu (kvuli logum) */
		void setTime()
		{
			std::string timee;
			std::time_t t = std::time(NULL);
		    char mbstr[100];
		    if (std::strftime(mbstr, sizeof(mbstr), "%F:%T", std::localtime(&t))) 
		    	this->timeNow = mbstr;
		}
		/* Metoda, ktera vytvori socket a otevre spojeni */
		void activateServer()
		{
			s = socket(PF_INET, SOCK_STREAM, 0);
			if (s < 0)
    		{
        		fprintf(stderr,"chyba pri tvorbe socketu\n"); 
        		exit(1);
    		}
    		struct sockaddr_in sin;
    		sin.sin_family = PF_INET;   
		    sin.sin_port = htons(this->port);  
		    sin.sin_addr.s_addr  = INADDR_ANY;  
		    if (bind(s, (struct sockaddr *)&sin, sizeof(sin) ) < 0 )
		    {
		        fprintf(stderr,"chyba pri bindu\n"); exit(1);
		    }
		    this->waitForConnection(sin);
		}
		/* Metoda, ktera dostane jako parametr hlavicku od klienta a ulozi si z ni relevantni informace */
		void decodeHeader(std::string msg)
		{
            std::istringstream iss(msg);
			std::string token;
			this->file = "./www";
			token.clear();
            for (int i = 0; i < 2; i++)
            {
            	if (getline(iss, token, ' '))
            	{            			
            		if (i == 0)
            		{
            			this->method = token;
            		}
            		else
            		{
            			if (token != "/")
            				this->file.append(token); 
            			else
            				this->file.append("/index.html");           				
            		}
            	}
            }
		}
		/* Metoda generujici hlavicku odpovedi klientovi */
		std::string generateHeader(int code)
		{
			std::string header;
			if (code == 404)			
				header = "HTTP/1.1 404 Not Found\n";			
			else
				header = "HTTP/1.1 200 OK\n";
			std::string timee;
			std::time_t t = std::time(NULL);
		    char mbstr[100];
		    if (std::strftime(mbstr, sizeof(mbstr), "%a, %d %b %Y %H:%M:%S", std::localtime(&t))) 
		        //std::cout << mbstr << '\n';

		    header.append("Date: ");
		    header.append(mbstr);
		    header.append("\n");		    
		    header.append("Content-Length: ");
		    header.append(std::to_string(this->chunkSize));
		    header.append("\n");
		    if (this->chunked)
		    	header.append("Transfer-Encoding: chunked\n");
		    header.append("Connection: close\n\n");
		    return header;
		}
		/* Pokud je velikost obsahu vetsi, nez velikost maximalniho chunku, obsah se rozkouskuje na delku parametru length */
		void sendChunk(int length, std::string *content)
		{
			char buffer[20];
			sprintf(buffer,"%x",length);
			char* charResponse = (char*)malloc(sizeof(char)*(length) + sizeof(char)*strlen(buffer) + sizeof(char)*9);
			strcpy(charResponse, buffer);
			strcat(charResponse, "\r\n");
			char* tempContent = (char*)malloc(sizeof(char)*length + sizeof(char));
			content->copy(tempContent, length, 0);
			tempContent[length] = '\0';
			strcat(charResponse, tempContent);
			free(tempContent);
			strcat(charResponse, "\r\n");
			charResponse[length+strlen(buffer)+8] = '\0';
			content->erase(content->begin(), content->begin() + length);
			this->outComm.append(charResponse);
			if ( write(t, charResponse, strlen(charResponse) ) < 0 )
			{
	      		fprintf(stderr,"error on write\n");    exit(1);
	    	}
			free(charResponse);
		}
		/* Metoda vytvarejici logy */
		void createLog()
		{
			std::string filename = "ipkHttpServer-";
			filename.append(this->timeNow);
			std::string fileIn = filename;
			fileIn.append(".in.log");
			std::string fileOut = filename;
			fileOut.append(".out.log");
			std::ofstream in;
			in.open(fileIn, std::ofstream::out | std::ofstream::trunc);			
    		in << this->inComm;
    		in.close();
    		std::ofstream out;
    		out.open(fileOut, std::ofstream::out | std::ofstream::trunc);			
    		out << this->outComm;
    		out.close();
    		this->inComm.clear();
    		this->outComm.clear();
		}
		/* Metoda ovladajici hlavni beh programu */
		void waitForConnection(struct sockaddr_in sin)
		{
			if (listen(s, 3))
		    { 
		        fprintf (stderr,"chyba pri listen\n");
		        exit(1);
		    }
		    socklen_t sinlen = sizeof(sin);
		    char buffer[1024];
		    while (true)
		    {
		    	/* Nastaveni obsluhy SIGINT */
		    	signal(SIGINT, my_function);
		    	if ( (t = accept(s, (struct sockaddr *) &sin, &sinlen) ) < 0 )
        		{
            		fprintf(stderr,"chyba pri acceptu\n");
            		exit(1);
        		}
        		std::string msg;
        		buffer[0] = '\0';
        		if ( read(t, buffer, 1024 ) <0)
            	{  
	                fprintf(stderr,"chyba pri cteni\n");
	                exit(1);
            	}        
            	msg = buffer;
            	this->inComm.append(msg);
            	std::string temp;
            	std::string content;
            	std::string header;
            	this->decodeHeader(msg);
            	std::ifstream myfile(this->file);
            	/* Pokud pozadovany soubor existuje */
				if (myfile.is_open())
				{
					while ( getline (myfile,temp) )
				    {
				    	content.append(temp);
				    	content.append("\n");
				    }
				    myfile.close();
				    if (content.length() > this->chunkSize)
						this->chunked = true;
					else this->chunked = false;
				    header = generateHeader(200);
				}
				/* Pokud pozadovany soubor neexistuje, posle se odpoved 404 */
				else
				{
					content = "<html><body><p>Error 404: File not found</p></body></html>";					
					if (content.length() > this->chunkSize)
						this->chunked = true;
					else
					{ 
						this->chunked = false;
						this->chunkSize = content.length();
					}
					header = generateHeader(404);	
			
				}
				/* Prvni se odesle hlavicka */ 
				std::string response = header;
				char* charHeader = (char*)malloc(sizeof(char)*header.length());
				std::size_t lengthHeader = response.copy(charHeader, header.length(), 0);
				charHeader[lengthHeader] = '\0';					
				if ( write(t, charHeader, header.length() ) < 0 )
				{
	      			fprintf(stderr,"error on write\n");    exit(1);
	    		}
	    		this->outComm.append(charHeader);
	    		free(charHeader);
	    		usleep(500);
				if (this->chunked)
				{	
					/* Dokud neni obsah prazdny, posilaji se chunks */    			
					while (!content.empty())
					{
						if (content.length() > this->chunkSize)
						{							
							this->sendChunk(this->chunkSize, &content);
						}
						else
						{
							this->sendChunk(int(content.length()), &content);
						}	
						usleep(this->chunkTime*1000);					
					}
					/* Zaslani posledniho chunku 0\r\n\r\n */
					char* charResponse = (char*)malloc(sizeof(char)*10);
					charResponse[0] = '0';
					charResponse[1] = '\0';
					strcat(charResponse, "\r\n\r\n");
					charResponse[9] = '\0';
					if ( write(t, charResponse, strlen(charResponse) ) < 0 )
					{
	      				fprintf(stderr,"error on write\n");    exit(1); /*  write error */
	    			}
	    			this->outComm.append(charResponse);
					free(charResponse);
				}
				else
				{
					response = content;				
					char* charResponse = (char*)malloc(sizeof(char)*response.length());
					std::size_t length = response.copy(charResponse, response.length(), 0);
					charResponse[length] = '\0';
					
					if ( write(t, charResponse, response.length() ) < 0 )
					{
	      				fprintf(stderr,"error on write\n");    exit(1); /*  write error */
	    			}
	    			this->outComm.append(charResponse);
	    			free(charResponse);	    			
    			} 
    			this->createLog();      			
			}
			if (close(t) < 0) { fprintf(stderr,"error on close"); exit(1);} 			
		}
};
/* Konstruktor serveru - ulozi potrebne informace z ArgParseru */
Server::Server(int port, int chunkSize, int chunkTime)
{
	this->port = port;
	this->chunkSize = chunkSize;
	this->chunkTime = chunkTime;
	this->setTime();
}
/* Trida ArgParser se stara o parsovani argumentu */
class ArgParser
{
	private:
		int chunkSize = 2048;
		int port = 10000;
		int chunkTime = 5;
	public:
		ArgParser(char**,int);
		void printHelp()
		{
			std::cout << "IPK HTTP server - pouziti:" << std::endl;
			std::cout << "	./ipkhttpserver -p port - na kterem portu ma byt spojeni otevreno" << std::endl;
			std::cout << "	./ipkhttpserver -c chunk-size - maximalni delka chunku" << std::endl;
			std::cout << "	./ipkhttpserver -t time - minimalni doba, za kterou se posle dalsi chunk" << std::endl;
			exit(0);
		}
		int getChunkSize()
		{
			return this->chunkSize;
		}
		int getPort()
		{
			return this->port;
		}
		int getChunkTime()
		{
			return this->chunkTime;
		}
};

ArgParser::ArgParser(char** argv, int argc)
{
	int c;
	while ((c = getopt (argc, argv, "hc:p:t:")) != -1)
	{
	   switch (c)
	    {
	      	case 'h':
	        	this->printHelp();
	        	break;
	      	case 'c':
	        	this->chunkSize = std::stoi(optarg);
	        	break;
	      	case 'p':
	        	this->port = std::stoi(optarg);
	        	break;
	        case 't':
	        	this->chunkTime = std::stoi(optarg);
	        	break;
	      	default:
	      		fprintf(stderr,"Neznamy argument\n");
	       		exit(1);
	       		break;
	    }
	}
}

int main(int argc,char** argv)
{
	ArgParser parser(argv,argc);
	Server server(parser.getPort(), parser.getChunkSize(), parser.getChunkTime());	
	server.activateServer();
	exit(0);
}
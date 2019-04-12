#ifndef _IMAPCL_H
#define _IMAPCL_H
#include <string>
#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <algorithm>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <unistd.h>
#include <vector>
#include <unistd.h>
#include "openssl/bio.h"
#include "openssl/ssl.h"
#include "openssl/err.h"
#include <stdexcept>
#include <regex>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <thread>
#include <future>
#include <mutex>

#define BUFFER (4096)

/* Class that represents the node. */
class Imapcl {
private:
	std::string buf;
	BIO* bio;
	SSL_CTX *ctx;
	SSL *ssl;
	int messageID;
	int port;
	int instructionCount;
	int savedMessages;
	bool isSSL;
	bool justHeaders;
	bool justNewMessages;	
	std::string currentInstruction;
	std::string certfile;
	std::string certpath;
	std::string server;
	std::string authfile;
	std::string inbox;
	std::string outputDirectory;
	bool doneReading;
	
public:
	Imapcl();	
	Imapcl(int argc, char *argv[]);
	std::string loadAuthFile();
	void printHelp();
	
	bool openConnection();
	void closeConnection();
	void service();

	std::vector<std::string> getNewEmails();
	void saveEmail();
	void printResult();
	bool sendRequestGetResponse(std::string request);	
};

#endif // _CLIENT_H
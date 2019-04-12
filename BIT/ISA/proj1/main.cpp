#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "main.h"

int main(int argc, char *argv[])
{
	Imapcl* client = NULL;
	try 
	{
		client = new Imapcl(argc, argv);
		client->loadAuthFile();
		if (client->openConnection())
			client->service();
		client->closeConnection();
		delete client;
	}
	catch(std::exception& ex)
	{
		if (strcmp(ex.what(), "stoi") == 0)
			std::cerr << "Port musí být kladné číslo.\n";
		else
			std::cerr << ex.what() << std::endl;
		client->printHelp();
		delete client;
		return 1;
	}
	return 0;
}
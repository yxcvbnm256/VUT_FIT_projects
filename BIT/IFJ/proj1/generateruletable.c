/**
 * Projekt: Implementace interpretu imperativního jazyka IFJ14
 */
/**
 * @file generateruletable.c
 * @author Michal Janousek(xjanou15)
 * @brief Generovani tabulky pravidel
 */
#include "parserexpression.h"
#include "tokens.h"
#include "errors.h"
#include <stdio.h>
#include <string.h>
#include "globalvariables.h"

#define DELIMITER ";"
#define BUFFER_SIZE 100

int main(int argc, char const *argv[])
{
	FILE *f;

	char * tokenName [] = {
		"NOT",
		"MULTIPLY", "DIVIDE", "AND",
		"PLUS", "MINUS", "OR", "XOR",
		"EQUAL", "LESS", "MORE", "NOT_EQUAL", "LESS_OR_EQUAL", "MORE_OR_EQUAL",
		"LEFT_BRACKET", "RIGHT_BRACKET",
		"REAL", "INTEGER", "BOOLEAN", "ID", "STRING",
		"STRING_TYPE", "REAL_TYPE", "INTEGER_TYPE", "BOOLEAN_TYPE", 
		"VAR", "COLON", 
		"ARRAY", "OF", "DOUBLEDOT", 
		"SEMICOLON", 
		"FUNCTION", "LEFT_PARENTHESIS", "RIGHT_PARENTHESIS", "FORWARD",
		"BEGIN", "END", "DOT",
		"IF", "THEN", "ELSE",
		"WHILE", "DO",
		"REPEAT", "UNTIL",
		"ASSIGNMENT",
		"WRITE", "READLN", "COMMA",
		"FILE_END",
		"DOLLAR", "E",
		"LEFT", "RIGHT", "MIDDLE", "BLANK", "FUNC"
	};

	int tokenType [] = {
		NOT,
		MULTIPLY, DIVIDE, AND,
		PLUS, MINUS, OR, XOR,
		EQUAL, LESS, MORE, NOT_EQUAL, LESS_OR_EQUAL, MORE_OR_EQUAL,
		LEFT_BRACKET, RIGHT_BRACKET,
		REAL, INTEGER, BOOLEAN, ID, STRING,
		STRING_TYPE, REAL_TYPE, INTEGER_TYPE, BOOLEAN_TYPE, 
		VAR, COLON, 
		ARRAY, OF, DOUBLEDOT, 
		SEMICOLON, 
		FUNCTION, LEFT_PARENTHESIS, RIGHT_PARENTHESIS, FORWARD,
		BEGIN, END, DOT,
		IF, THEN, ELSE,
		WHILE, DO,
		REPEAT, UNTIL,
		ASSIGNMENT,
		WRITE, READLN, COMMA,
		FILE_END,
		DOLLAR, E,
		LEFT, RIGHT, MIDDLE, BLANK, FUNC
	};

	if (argc == 1)
	{
		errCode = FILE_ERROR;
		fatalError("Neni zadan vstupni soubor\n");
	}
	if ((f = fopen(argv[1], "r")) == NULL)
	{
		errCode = FILE_ERROR;
		fatalError("Soubor se nepodarilo otevrit\n");
	}

	char buffer[BUFFER_SIZE];

	printf("char * expressionRuleTable[] = {\n\t\"");

	while(fscanf(f, "%s", buffer) == 1)
	{
		for(unsigned i = 0; i < sizeof(tokenName)/sizeof(*tokenName); ++i)
		{
			if(strcmp(tokenName[i], buffer) == 0)
			{
				printf("\\x%x",tokenType[i]);
				break;
			}
		}
		if(strcmp(DELIMITER, buffer) == 0)
		{
			printf("\",\n\t\"");
		}
	}
	printf("\"\n};");

	return 0;
}

/**
 * Projekt: Implementace interpretu imperativního jazyka IFJ14
 */
/**
 * @file scanner.c
 * @author Zdenek Studeny (xstude21)
 * @brief Implementace lexikalniho analyzatoru
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h> // isspace()
#include "globalvariables.h"
#include "memoryallocation2.h"
#include "scanner.h"
#include "debug.h"
#include "errors.h"


 /**
 * @brief Inicializace jednosměrného listu
 *
 */
void InitList (tList *L)
{
	L->Act = NULL;
	L->First = NULL;
}

/**
 * @brief Vloží na začátek
 *
 */
void InsertFirst (tList *L, char val)
{
	tElemPtr Element;	
	Element = getDTElem(1);

	Element->data = val;
	Element->ptr = L->First;
	L->First = Element;
}

/**
 * @brief Dá hodnotu prví položky
 *
 */
void CopyFirst (tList *L, char *val)
{
	if (L->First == NULL)
		//Error();
		return;
	else
		*val = L->First->data;
}

/**
 * @brief Smaže první položku listu
 *
 */
void DeleteFirst (tList *L)
{	
	if (L->First != NULL)
	{
		L->First = L->First->ptr;
		removeDTElem(1);
	}			
}



/**
 * @brief Zjisti jestli je char cislo
 *
 * vraci true, pokud je char z 0-9.
 * V opacnem pripade vraci false.
 */
bool isNumber(int c)
{
	return ( ( c >= '0' && c<= '9' ) ? true : false );
}

/**
 * @brief Zjisti jestli je char znak abecedy
 *
 * vraci true, pokud je char a-z nebo A-Z.
 * V opacnem pripade vraci false.
 */
bool isAlphabet(int c)
{
	return ( ( (c >= 'a' && c <= 'z') || (c >= 'A' && c<= 'Z' ) ) ? true : false );
}

/**
 * @brief Převede pole znaků na velká písmena
 *
 */
void arrayToUpper()
{
	for (int i = 0; tokenBuffer[i] != '\0' ; ++i)
	{
		tokenBuffer[i] = toupper(tokenBuffer[i]);
	}
	return;
}

/**
 * @brief Převede pole znaků na malá písmena
 *
 */
void arrayTolower()
{
	for (int i = 0; tokenBuffer[i] != '\0' ; ++i)
	{
		tokenBuffer[i] = tolower(tokenBuffer[i]);
	}
	return;
}

/**
 * @brief vloží konec řetězce
 *
 */
void insertArrayEnd( int i )
{
	if ( i >= 128 )
	{
		tokenBuffer[127] = '\0';
	}else {
		tokenBuffer[i] = '\0';
	}
	return;
}

void setSourceFile(FILE *f)
{
  source = f;
}

/**
 * @brief Nahraje do globálních proměných Další Token. 
 *
 * actToken - globální proměnná kam se ukládá token
 * 
 * Pokud se nepodaří načíst token, funkce volá fatalError()
 */
void getToken()
{
	int c;				// Aktuální znak
	stateT state = S_SPACE;		// Stav KA
	long int i = 0;		// Counter pro lepší práci s polem
	long int streamPosition;	// Pozice v souboru
	tList list;			// List pro meziuložení stringů

debug(
	if(actToken.type < TOKENS_COUNT)
		printf("%s ", tokenRewrite[actToken.type]);
	if(actToken.type == STRING)
		printf("%s\n", actToken.value.stringVal);
);

	while(true) // Konecny automat
	{

		c = getc(source);
		i++;

		switch(state)
		{

		// Výchozí stav
		case S_SPACE:

			if ( isspace(c) )
			{
				state = S_SPACE;
			}else{
				i = 0;

				if ( isAlphabet(c) || c == '_' ) // case 1, bool/id
				{
					tokenBuffer[i] = c;
					state = S_ID;
				} else if ( isNumber(c) ) // case 2, integer/real/real
				{

					tokenBuffer[i] = c;
					state = S_INTEGER;
				} else if ( c == '\'' ) // case 3, string
				{
					c = ftell(source);
					InitList(&list);
					InsertFirst(&list, c);
					state = S_STRING;

				} else if ( c == '{' ) //  comment
				{
					while( c != '}' )
					{
						c = getc(source);

						if ( c == EOF)
						{
							ungetc(c, source);	// Vrati znak, aby se mohl nacist EOF
							errCode = 1;
							fatalError("scanner error - Neukončený comment\n");
						}
					}
				} else if ( c == ':' ) // case 4, :
				{
					tokenBuffer[i] = c;
					state = S_COLON;
				} else if ( c == '>') //case 5, =
				{
					tokenBuffer[i] = c;
					state = S_MORE;
				}else if ( c == '<') //case 6, =
				{
					tokenBuffer[i] = c;
					state = S_LESS;
				}else if ( c == '.') //case 7, .
				{
					tokenBuffer[i] = c;
					state = S_DOT;
				}else if ( c == ';') // One symbol token
				{
					actToken.type = SEMICOLON;
					return;
				}else if ( c == '+') // One symbol token
				{
					actToken.type = PLUS;
					return;
				}else if ( c == '-') // One symbol token
				{
					actToken.type = MINUS;
					return;
				}else if ( c == '/') // One symbol token
				{
					actToken.type = DIVIDE;
					return;
				}else if ( c == '*') // One symbol token
				{
					actToken.type = MULTIPLY;
					return;
				}else if ( c == '(') // One symbol token
				{
					actToken.type = LEFT_PARENTHESIS;
					return;
				}else if ( c == ')') // One symbol token
				{
					actToken.type = RIGHT_PARENTHESIS;
					return;
				}else if ( c == '[') // One symbol token
				{
					actToken.type = LEFT_BRACKET;
					return;
				}else if ( c == ']') // One symbol token
				{
					actToken.type = RIGHT_BRACKET;
					return;
				}else if ( c == ',') // One symbol token
				{
					actToken.type = COMMA;
					return;
				}else if ( c == '=') // One symbol token
				{
					actToken.type = EQUAL;
					return;
				}else if ( c == EOF) // One symbol token
				{
					actToken.type = FILE_END;
					return;
				}else
				{
					errCode = 1;
					fatalError("Scanner error - nepovoleny token\n");
				}

			}

		break;

		case S_ID: // bool/id

		
			if ( isAlphabet(c) || c == '_' || isNumber(c) )
			{
				if ( i < 128 ) {
					tokenBuffer[i] = c;
				}

			} else {
				// uloz hodnotu
				insertArrayEnd(i);
				arrayTolower();

				if ( strcmp(tokenBuffer, "true" ) == 0 )
				{
					actToken.type = BOOLEAN;
					actToken.value.booleanVal = true; 
					ungetc(c, source);
					return;
				}

				if ( strcmp(tokenBuffer, "false" ) == 0 )
				{
					actToken.type = BOOLEAN;
					actToken.value.booleanVal = false; 
					ungetc(c, source);
					return;
				}
				for (int j=0; j < 24 ; ++j ) // 24 number of tokens in specialTokens
				{
					if ( strcmp(tokenBuffer, specialTokens[j] ) == 0 )
					{
						actToken.type = specialTokensT[j];
						ungetc(c, source);
						return;
					}
				}
				actToken.type = ID;
				actToken.value.idVal = tokenBuffer; 
				ungetc(c, source);
				return;				
			}

		break;

		case S_INTEGER: // number

			if ( isNumber(c) )
			{
				tokenBuffer[i] = c;
			}else if ( c == '.' )
			{
				streamPosition = ftell(source);
				tokenBuffer[i] = c;
				state = S_INTPOINT; // byla tecka
			}else if ( c == 'e' || c == 'E' )
			{
				streamPosition = ftell(source);
				tokenBuffer[i] = c;
				state = S_INTEXP;
			}else {
				// uloz hodnotu
				insertArrayEnd(i);
				ungetc(c, source);
				actToken.type = INTEGER;
				sscanf( tokenBuffer, "%d", &actToken.value.integerVal);
				return;
			}

		break;

		case S_INTPOINT: // byla tecka, potrebuji cislo

			if ( isNumber(c) )
			{
				tokenBuffer[i] = c;
				state = S_FLOAT;
			}else{
				fseek(source, streamPosition-1, SEEK_SET);
				insertArrayEnd(i-1);
				actToken.type = INTEGER;
				sscanf( tokenBuffer, "%d", &actToken.value.integerVal);
				return;
			}

		break;

		case S_INTEXP: // bylo e, potrebuji cislo nebo znamenko

			if ( isNumber(c) )
			{
				tokenBuffer[i] = c;
				state = S_FLOAT2;
			}else if (  c == '+' || c == '-' )
			{
				tokenBuffer[i] = c;
				state = S_INTEXPSIGN;
			}
			else{
				fseek(source, streamPosition-1, SEEK_SET);
				insertArrayEnd(i-1);
				actToken.type = INTEGER;
				sscanf( tokenBuffer, "%d", &actToken.value.integerVal);
				return;
			}

		break;

		case S_INTEXPSIGN: // cislo bylo e a +/-, potrebuji cislo

			if ( isNumber(c) )
			{
				tokenBuffer[i] = c;
				state = S_FLOAT2;
			}else{
				fseek(source, streamPosition, SEEK_SET);
				insertArrayEnd(i-1);
				actToken.type = INTEGER;
				sscanf( tokenBuffer, "%d", &actToken.value.integerVal);
				return;
			}
		break;


		case S_FLOAT: // byla tecka a cislo 

			if ( isNumber(c) )
			{
				tokenBuffer[i] = c;
			}else if ( c == 'e' || c == 'E')
			{
				streamPosition = ftell(source);
				tokenBuffer[i] = c;
				state = S_FLOATEXP;
			}else{
				// uloz
				ungetc(c, source);
				actToken.type = REAL;
				insertArrayEnd(i);
				sscanf( tokenBuffer, "%lf", &actToken.value.realVal) ;
				return;
			}
		break;

		case S_FLOAT2: // bylo e a cislo

			if ( isNumber(c) )
			{
				tokenBuffer[i] = c;
			}else{
				// uloz
				ungetc(c, source);
				actToken.type = REAL;
				insertArrayEnd(i);
				sscanf( tokenBuffer, "%lf", &actToken.value.realVal) ;
				return;
			}
		break;

		case S_FLOATEXP: // cislo uz je s teckou float. bylo e, potrebuji cislo nebo +/-
			if ( isNumber(c) )
			{
				tokenBuffer[i] = c;
				state = S_FLOAT2;
			}else if ( c == '+' || c == '-')
			{
				streamPosition = ftell(source);
				tokenBuffer[i] = c;
				state = S_FLOATEXPSIGN;
			}else{
				fseek(source, streamPosition-1, SEEK_SET);
				actToken.type = REAL;
				insertArrayEnd(i-1);
				sscanf( tokenBuffer, "%lf", &actToken.value.realVal);
				return;
			}

		break;

		case S_FLOATEXPSIGN: // REAL bylo e a +/-, potrebuji cislo

			if ( isNumber(c) )
			{
				tokenBuffer[i] = c;
				state = S_FLOAT2;
			}else{
				fseek(source, streamPosition, SEEK_SET);
				insertArrayEnd(i-1);
				actToken.type = REAL;
				sscanf( tokenBuffer, "%lf", &actToken.value.realVal);
				return;
			}
		break;

		case S_STRING: // uvnitr stringu

			if ( c == EOF)
			{
				ungetc(c, source);
				errCode = 1;
				fatalError("Scanner EOF uvnotr stringu\n");
			}

			if ( c <= 31)
			{
				errCode = 1;
				fatalError("scanner error - ASCI hodnota < 31\n");
			}

			if ( c == '\'' )
			{
				state = S_STRINGHASH;
				--i;
			}else{
				InsertFirst(&list, c);
			}

		break;

		case S_STRINGHASH: // dostal jsem '

			if ( c == '\'' )
			{
				InsertFirst(&list, '\'');
				state = S_STRING;
			}else if ( c == '#' )
			{
				state = S_ASCI;
			}else{ 	// znak ktery nepatri do stringu
				// Ulozeni
				actToken.type = STRING;
				char *pomString; 
				pomString = getDString(i);
				pomString[i-1] = '\0';
				for ( i = i-2 ; i >= 0 ; --i )
				{
					CopyFirst(&list, &pomString[i]);
					DeleteFirst(&list); 
				}

				actToken.value.stringVal =  pomString;
				ungetc(c, source);
				return;
			}

		break;

		case S_ASCI: // # ascii

			--i;
			int pomCount = 0;

			while ( pomCount >= 0 && pomCount <= 255 && c != '\'' )
			{
				if ( isNumber(c) )
				{
					pomCount = pomCount*10 + ( c - '0' );
					c = getc(source);
				}else{
					pomCount = 1000;
				}
				
			}
			
			if ( pomCount >= 1 && pomCount <= 255 && c == '\'')
			{
				InsertFirst(&list, pomCount);
				state = S_STRING;
			} else {
				errCode = 1;
				fatalError("Scanner - v escape characteru je vysoka hodnota\n");
			}

		break;

		case S_COLON: // :

			if ( c == '=')
			{
				actToken.type = ASSIGNMENT;
				return;
			}else{
				actToken.type = COLON;
				ungetc(c, source);
				return;
			}

		break;

		case S_MORE:

			if ( c == '=') // >='
			{
				actToken.type = MORE_OR_EQUAL;
				return;
			}else{
				actToken.type = MORE;
				ungetc(c, source);
				return;
			}

		break;

		case S_LESS:

			if ( c == '=') // <=
			{
				actToken.type = LESS_OR_EQUAL;
				return;
			}
			else if ( c == '>') // <>
			{
				actToken.type = NOT_EQUAL;
				return;
			}else{
				actToken.type = LESS;
				ungetc(c, source);
				return;
			}

		break;

		case S_DOT:

			if ( c == '.') // ..
			{
				actToken.type = DOUBLEDOT;
				return;
			}else{
				actToken.type = DOT;
				ungetc(c, source);
				return;
			}

		break;

		}	// end switch

	} // end while

} // end getNextToken

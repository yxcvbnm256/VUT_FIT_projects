/**
 * Projekt: Implementace interpretu imperativního jazyka IFJ14
 */
/**
 * @file globalvariables.c
 * @author Michal Janousek(xjanou15)
 * @brief Soubor absahující definice globálních proměnných
 */

#include "tokens.h"
#include "parserexpression.h"
#include "symboltable.h"
#include "globalvariables.h"
#include "memoryallocation2.h"
#include <stdlib.h>

/**
 * Global variables pro spravu pameti
*/
staticNodeT *staticMNPtr = NULL;
staticStringT *staticMSPtr = NULL;
staticTACT *staticMTACPtr = NULL;
staticSTFT *staticMSTFPtr = NULL;
staticSTVT *staticMSTVPtr = NULL;
dynamicOperListT * dynamicOperListPtr = NULL;
dynamicVariableInfoT* dynamicVIPtr = NULL;
dynamicParameterT * dynamicParameterPtr = NULL;
dynamicSElemT * dynamicSElemPtr = NULL;
dynamicTElemT * dynamicTElemPtr = NULL;
dynamicPLIT *dynamicPLIPtr = NULL;
dynamicCharT *dynamicCharPtr = NULL;
dynamicIntT *dynamicIntPtr = NULL;
dynamicFloatT *dynamicFloatPtr = NULL;
dynamicTAST *dynamicTASTPtr = NULL;
dynamicVTT *dynamicVTPtr = NULL;
dynamicElementT *dynamicElementPtr = NULL;
dynamicElemT *dynamicElemPtr = NULL;

int errCode = 0;

tokenT actToken;

char tokenBuffer[ 128];

char *specialTokens[] = {
   "not", "and", "or", "xor",
   "integer", "string", "real", "boolean",
   "var", "array", "of",
   "function", "forward",
   "begin", "end",
   "if", "then", "else",
   "while", "do",
   "repeat", "until",
   "write", "readln"
};

tokenTypeT specialTokensT[] = {
   NOT, AND, OR, XOR,
   INTEGER_TYPE, STRING_TYPE, REAL_TYPE, BOOLEAN_TYPE, 
   VAR, ARRAY, OF,
   FUNCTION, FORWARD,
   BEGIN, END,
   IF, THEN, ELSE,
   WHILE, DO,
   REPEAT, UNTIL,
   WRITE, READLN
};



int precedentialTable[][PRECEDENTIAL_TABLE_ROW_LEN] = {
   {LEFT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,LEFT,RIGHT,LEFT,RIGHT},
   {LEFT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,LEFT,RIGHT,LEFT,RIGHT},
   {LEFT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,LEFT,RIGHT,LEFT,RIGHT},
   {LEFT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,LEFT,RIGHT,LEFT,RIGHT},
   {LEFT,LEFT,LEFT,LEFT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,LEFT,RIGHT,LEFT,RIGHT},
   {LEFT,LEFT,LEFT,LEFT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,LEFT,RIGHT,LEFT,RIGHT},
   {LEFT,LEFT,LEFT,LEFT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,LEFT,RIGHT,LEFT,RIGHT},
   {LEFT,LEFT,LEFT,LEFT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,LEFT,RIGHT,LEFT,RIGHT},
   {LEFT,LEFT,LEFT,LEFT,LEFT,LEFT,LEFT,LEFT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,LEFT,RIGHT,LEFT,RIGHT},
   {LEFT,LEFT,LEFT,LEFT,LEFT,LEFT,LEFT,LEFT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,LEFT,RIGHT,LEFT,RIGHT},
   {LEFT,LEFT,LEFT,LEFT,LEFT,LEFT,LEFT,LEFT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,LEFT,RIGHT,LEFT,RIGHT},
   {LEFT,LEFT,LEFT,LEFT,LEFT,LEFT,LEFT,LEFT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,LEFT,RIGHT,LEFT,RIGHT},
   {LEFT,LEFT,LEFT,LEFT,LEFT,LEFT,LEFT,LEFT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,LEFT,RIGHT,LEFT,RIGHT},
   {LEFT,LEFT,LEFT,LEFT,LEFT,LEFT,LEFT,LEFT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,LEFT,RIGHT,LEFT,RIGHT},
   {LEFT,LEFT,LEFT,LEFT,LEFT,LEFT,LEFT,LEFT,LEFT,LEFT,LEFT,LEFT,LEFT,LEFT,LEFT,MIDDLE,LEFT,BLANK},
   {BLANK,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,BLANK,RIGHT,BLANK,RIGHT},
   {BLANK,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,RIGHT,BLANK,RIGHT,BLANK,RIGHT},
   {LEFT,LEFT,LEFT,LEFT,LEFT,LEFT,LEFT,LEFT,LEFT,LEFT,LEFT,LEFT,LEFT,LEFT,LEFT,FUNC,LEFT,BLANK}
};

char * expressionRuleTable[] = {
   "\x1\x34",
   "\x34\x2\x34",
   "\x34\x3\x34",
   "\x34\x4\x34",
   "\x34\x5\x34",
   "\x34\x6\x34",
   "\x34\x7\x34",
   "\x34\x8\x34",
   "\x34\x9\x34",
   "\x34\xa\x34",
   "\x34\xb\x34",
   "\x34\xc\x34",
   "\x34\xd\x34",
   "\x34\xe\x34",
   "\xf\x34\x10",
   "\x11",
   "\x6\x34"
};

STFunctionT* functionTable = NULL;
STVariableT* localVariables = NULL;
STVariableT* globalVariables = NULL;
STFunctionT* actualFunction = NULL;

threeAddressCodeT * last3ak = NULL;
threeAddressCodeT * globalThreeAddrC = NULL;

operListT* operList = NULL;


unsigned intCounter = 0;
unsigned realCounter = 0;
unsigned stringCounter = 0;

char * tokenRewrite[] = { 
   "", // posun o 1
   "NOT",
   "*", "/", "AND",
   "+", "-", "OR", "XOR",
   "=", "<", ">", "<>", "<=", ">=",
   "(", ")",
   "ID", "INTEGER", "STRING", "REAL", "BOOLEAN",
   "INTEGER_TYPE", "STRING_TYPE", "REAL_TYPE", "BOOLEAN_TYPE", 
   "VAR", ":", 
   "ARRAY", "OF", "..", 
   ";", 
   "FUNCTION", "[", "]", "FORWARD",
   "BEGIN", "END", ".",
   "IF", "THEN", "ELSE",
   "WHILE", "DO",
   "REPEAT", "UNTIL",
   ":=",
   "WRITE", "READLN", ",",

   "FILE_END"
};


FILE *source = NULL;


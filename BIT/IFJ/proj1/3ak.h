/**
 * Projekt: Implementace interpretu imperativního jazyka IFJ14
 */
/**
 * @file 3ak.h
 * @author Michal Janousek(xjanou15)
 * @brief Hlavičkový soubor pro Tříadresný kód
 */

#ifndef _3AK_H
#define _3AK_H

typedef struct threeAddressCodeT threeAddressCodeT;

typedef struct variableInfoT variableInfoT;

typedef enum instructionT
{

	I_CALL, /**< Zavolej funkci zadanou v op1(STFunctionT *) s parametrama v op2(parameters) a výsledek uloží do dest (index). */
	I_LABEL, /**< Místo, kam se skáče. Nic nedělat. */
	I_RETURN_I, /**< Konec funkce. */
	I_RETURN_S, /**< Konec funkce. */
	I_RETURN_R, /**< Konec funkce. */
	I_CONTINUE, /**< Pokud je podmínka v op1(index) pravdivá, pokračuj, jinak skoč na dest (3AK *). */
	I_SKIP, /**< Pokud je podmínka v op1(index) pravdivá, skoč na dest (3AK *) jinak pokračuj. */
	I_GOTO, /**< Skoč na dest (3AK *). */
	I_WRITE_IV, I_WRITE_SV, I_WRITE_RV, I_WRITE_BV, /**< Zapiš op1(index) do stdout. */
	I_WRITE_IC, /**< Zapiš op1(intVal) do stdout. */
	I_WRITE_SC, /**< Zapiš op1(stringVal) do stdout. */
	I_WRITE_RC, /**< Zapiš op1(realVal) do stdout. */
	I_WRITE_BC, /**< Zapiš op1(intVal) do stdout. */
	I_READLN_I, /** Načti integer z stdin do op1(index). */
	I_READLN_S, /** Načti string z stdin do op1(index). */
	I_READLN_R, /** Načti real z stdin do op1(index). */
	I_CALL_LENGTH, /**< Změř délku stringu v op1(index) a výsledek ulož do dest(index). */
	I_CALL_COPY, /**< Zkopíruje řetězec do dest(index) podle parametrů v op2(parameters) */
	I_CALL_FIND, /**< Do dest(index) ulož pozici prvního výskytu podřetězce. Parametry v op2(parameters). */
	I_CALL_SORT, /**< Do dest vložíš výseldek a v op2 jsou parametry */
	I_ASSIGN, /**< Do dest(index) ulož op1(index) */
	I_ADD, /**< Do dest(index) ulož op1(index) + op2(index) */
	I_SUB, /**< Do dest(index) ulož op1(index) - op2(index) */
	I_MULT, /**< Do dest(index) ulož op1(index) * op2(index) */
	I_DIV, /**< Do dest(index) ulož op1(index) / op2(index) */
	I_NOT, /**< Hodnotu v op1 předělá na opačnou, výsledek uloží do destination - true / false */
	I_AND, /**< dest = op1 && op2.  true/false */
	I_OR, /**< dest = op1 || op2.  true/false */
	I_XOR, /**< dest = op1 != op2. true/false */
	I_EQUAL,/**< dest = op1 == op2 (muze byt i nad retezci i bool) uklada se true/false */
	I_LESS, /**< dest = op1 < op2. (muze byt i nad retezci i bool) uklada se true/false */
	I_MORE, /**< dest = op1 > op2. (muze byt i nad retezci i bool) uklada se true/false */
	I_NOT_EQUAL, /**< dest = op1 != op2. (muze byt i nad retezci i bool) uklada se true/false */
	I_LESS_OR_EQUAL, /**< dest = op1 <= op2. (muze byt i nad retezci i bool) uklada se true/false */
	I_MORE_OR_EQUAL /**< dest = op1 >= op2. (muze byt i nad retezci i bool) uklada se true/false */

}instructionT;

typedef enum parameterTypeT
{
	INT_CONSTANT, STRING_CONSTANT, REAL_CONSTANT,
	INT_VARIABLE, STRING_VARIABLE, REAL_VARIABLE,
	INT_GLOBAL, STRING_GLOBAL, REAL_GLOBAL

} parameterTypeT;

typedef union parameterValT
{
	unsigned index;
	double realVal;
	int intVal;
	char * stringVal;
} parameterValT;

#include "symboltable.h"

typedef struct parameterT
{
	struct parameterT * next;
	variableInfoT * data;

} parameterT;

struct variableInfoT
{
	parameterValT value;
	parameterTypeT type;
};

typedef union operandT
{
	variableInfoT * varInfo;
	threeAddressCodeT * code;
	STFunctionT * function;
	parameterT * parameters;
} operandT;

struct threeAddressCodeT
{
	instructionT instruction; /**< Prováděná instrukce. */
	operandT op1; /**< První operand. */
	operandT op2; /**< Druhý operand. */
	operandT dest; /**< Cíl prováděné instrukce */
	threeAddressCodeT * next; /**< Ukazatel na další instrukci */
	
};

#endif // _3AK_H

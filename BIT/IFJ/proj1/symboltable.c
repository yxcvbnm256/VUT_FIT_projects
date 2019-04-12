/**
 * Projekt: Implementace interpretu imperativního jazyka IFJ14
 */
/**
 * @file symboltable.h
 * @author Michal Janoušek(xjanou15)
 * @brief Funkce nad tabulkou symbolů
 */

#include "symboltable.h"
#include "memoryallocation2.h"
#include "bst.h"
#include "globalvariables.h"
#include "errors.h"
#include <stdlib.h>
#include <string.h>

/**
 * @brief Inicializování funkce při jejím vkládání do stromu
 */
void insertFunctionInit(STFunctionT* function)
{
	function->intVarCount = 0;
	function->intTmpCount = 0;
	function->realVarCount = 0;
	function->realTmpCount = 0;
	function->stringVarCount = 0;
	function->stringTmpCount = 0;
	function->beginning = NULL;
	function->defined = false;
	function->declared = false;
	function->params = NULL;
}

/**
 * @brief Vloží funkci do tabulky symbolů, pokud už byla definovaná, chyba, pokud existuje globální proměnná se stejným jmánem, chyba
 */
STFunctionT * insertFunctionTable(const char * name)
{
	STFunctionT * tmp = insertFunction(name, &functionTable);
	if(tmp->defined == true)
	{
		errCode = ID_REDEFINITION;
		fatalError("Pri definici funkce:\n\tFunkce se jmenem \"%s\" jiz existuje a je definovana.\n", name);
	}
	if(findVariable(name, globalVariables) != NULL)
	{
		errCode = ID_REDEFINITION;
		fatalError("Pri definici funkce:\n\tGlobalni promenna se jmenem \"%s\" jiz existuje.\n", name);		
	}
	return tmp;
}
/**
 * @brief Vloží lokální proměnnou, pokud již existuje nebo existuje funkce stejného jména, chyba
 */
STVariableT * insertLocalVariable(const char * name)
{
	STVariableT * tmp = insertVariable(name, &localVariables);
	if(tmp == NULL)
	{
		errCode = ID_REDEFINITION;
		fatalError("Pri definici promenne:\n\tLokalni promenna se jmenem \"%s\" jiz existuje.\n", name);
	}
	if(findFunction(name, functionTable) != NULL)
	{
		errCode = ID_REDEFINITION;
		fatalError("Pri definici promenne:\n\tJiz existuje funkce se jmenem \"%s\".\n", name);		
	}
	return tmp;
}
/**
 * @brief Nastaví aktuální funkci
 */
bool setFunction(const  char * name)
{
	actualFunction = findFunction(name, functionTable);
	return true;
}

/**
 * @brief Přídá typ a index proměnné
 */
void setVariable(STVariableT * variable, paramTypeT type)
{
		variable->type = type;
		switch(variable->type)
		{
			case REAL_TYPE:
				variable->index = actualFunction->realVarCount++;
				break;
			case STRING_TYPE:
				variable->index = actualFunction->stringVarCount++;
				break;
			default:
				variable->index = actualFunction->intVarCount++;
				break;
		}
}

/**
 * @brief Vloží parametr do seznamu parametrů
 */
paramListItemT * insertParam(char* name)
{ 
	paramListItemT * tmp = getDPLI(1);
	tmp->next = NULL;
	size_t len = strlen(name) + 1;
	tmp->name = memcpy(getDString(len), name, len);
	return tmp;
}
/**
 * @brief Nastaví u parametru typ
 */
void setParam(paramListItemT * params, tokenTypeT tokenType)
{
	params->type = tokenType;
}

/**
 * @brief Inicializuje lokální tabulku proměnných podle actualFunction
 */
void initLocalTable()
{
	paramListItemT * params = actualFunction->params;
	STVariableT * variable;

	localVariables = NULL;

	variable = insertVariable(params->name, &localVariables);
	setVariable(variable, params->type);
	params->index = variable->index; 

	params = params->next;

	while(params != NULL)
	{
		variable = insertLocalVariable(params->name);
		setVariable(variable, params->type);
		params->index = variable->index; 
		params = params->next;
	}
}

/**
 * @brief Kontrola, zda jsou všechny deklarovaná funkce definované
 */
void checkDefinitions(STFunctionT * tree)
{
	if(tree != NULL)
	{
		checkDefinitions(tree->lChild);
		checkDefinitions(tree->rChild);
		if(! tree->defined)
		{
			errCode = UNDEFINED_ID;
			fatalError("Funkce \"%s\" neni definovana.\n", tree->name);
		}
	}
}

/**
 * @brief Inicializování tabulky funkcí
 */
void initTable()
{
	// vložení vestavěných funkcí
	paramListItemT * params;
	// length(s : string) : integer
	actualFunction = insertFunctionTable("length");
	actualFunction->defined = true;
	params = insertParam("length");
	actualFunction->params = params;
	setParam(params, INTEGER_TYPE);
	params->next = insertParam("s");
	params = params->next;
	setParam(params, STRING_TYPE);
	//copy(s : string; i : integer; n : integer) : string
	actualFunction = insertFunctionTable("copy");
	actualFunction->defined = true;
	params = insertParam("copy");
	setParam(params, STRING_TYPE);
	actualFunction->params = params;
	params->next = insertParam("s");
	params = params->next;
	setParam(params, STRING_TYPE);
	params->next = insertParam("i");
	params = params->next;
	setParam(params, INTEGER_TYPE);
	params->next = insertParam("n");
	params = params->next;
	setParam(params, INTEGER_TYPE);
	//find(s : string; search : string) : integer
	actualFunction = insertFunctionTable("find");
	actualFunction->defined = true;
	params = insertParam("find");
	setParam(params, INTEGER_TYPE);
	actualFunction->params = params;
	params->next = insertParam("s");
	params = params->next;
	setParam(params, STRING_TYPE);
	params->next = insertParam("search");
	params = params->next;
	setParam(params, STRING_TYPE);
	//sort(s : string) : string
	actualFunction = insertFunctionTable("sort");
	actualFunction->defined = true;
	params = insertParam("sort");
	setParam(params, STRING_TYPE);
	actualFunction->params = params;
	params->next = insertParam("s");
	params = params->next;
	setParam(params, STRING_TYPE);
	// main
	actualFunction = insertFunctionTable("");
	actualFunction->defined = true;

}

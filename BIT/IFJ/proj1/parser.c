/**
 * Projekt: Implementace interpretu imperativního jazyka IFJ14
 */
/**
 * @file parser.h
 * @author Michal Janoušek(xjanou15)
 * @brief Implementace parseru
 */

#include <stdbool.h>
#include "parser.h"
#include "parserexpression.h"
#include "globalvariables.h"
#include "tokens.h" 
#include "errors.h"
#include "scanner.h"
#include "symboltable.h"
#include "bst.h"
#include <string.h>
#include "memoryallocation2.h"
#include "3ak.h"
#include "debug.h"
#include <limits.h>

#define INITDPLI_SIZE 32
#define INITDOPERLIST_SIZE 32
#define INITDVARIABLEINFO_SIZE 16
#define INITDPARAMETER_SIZE 32
#define INITDSTF_SIZE 16
#define INITDSTV_SIZE 16

bool isVariables();
bool isVariable();
bool isArray();
bool isType();
bool isFunctions();
bool checkParams();
bool isParams(paramListItemT * param);
bool isNextParam(paramListItemT * param);
bool isFunctionBody();
bool isInstructions();
bool isInstruction();
bool isNextInstruction();
bool isElseIf(threeAddressCodeT * condition);
bool isWriteParams();
bool isNextWriteParams();

/**
 * @brief Vypíše očekávaný a příchozí token + nastavý syntax error
 */
void tokenError(tokenTypeT expected, const char * comment)
{
	errCode = SYNTAX_ERROR;
	fatalError("%s\nOcekavany token: \"%s\"\nPrichozy token: \"%s\"\n", comment, tokenRewrite[expected], tokenRewrite[actToken.type]);
}

/**
 * @brief Kontrola, zda je příchozí token daný terminál
 */
#define isTerminal(terminal, comment) ( actToken.type == (terminal) ? (getToken(), true) : (tokenError(terminal, comment), false) )

/**
 * @brief Kotrola, zda je token na vstupu ID + provádění akce, pokud je
 */
#define isId(action, comment) ( actToken.type == (ID) ? ((action) ,getToken(), true) : (tokenError(ID, comment), false) ) 

/**
 * @brief BEGIN; isInstructions(); ACTION, END;
 */
#define isSequence(action, comment) ( isTerminal(BEGIN, comment) && isInstructions() && ((action), isTerminal(END, comment)) )


/**
 * @brief Vytvoří operand z aktuálního tokenu
 */
operListT * createOper()
{
	if(actToken.type == ID)
	{
		STVariableT * var= findVariable(actToken.value.idVal, localVariables);
		int level = INT_VARIABLE;
		if(var == NULL)
		{
			var = findVariable(actToken.value.idVal, globalVariables);

			if(var == NULL)
				return NULL;

			level = INT_GLOBAL;
		}

		operListT * tmp = getDOperList(1);
		tmp->data = getDVariableInfo(1);
		tmp->type = var->type;
		tmp->data->value.index = var->index;

		tmp->data->type = ((tmp->type - INTEGER_TYPE) % 3 ) + level;
		return tmp;
	}
	else // Konstanta
	{
		operListT * tmp = getDOperList(1);
		tmp->data = getDVariableInfo(1);
		tmp->type = actToken.type - INTEGER + INTEGER_TYPE;
		tmp->data->value = actToken.value.parameterVal;
		tmp->data->type = (actToken.type - INTEGER) % 3 + INT_CONSTANT;
		return tmp;
	}
}

/**
 * @brief Vytvoří operand daného typu jako dočasnou proměnnou
 */
operListT * createOperType(paramTypeT type)
{
	operListT * tmp = getDOperList(1);
	tmp->data = getDVariableInfo(1);
	tmp->type = type;
	switch(type)
	{
		case REAL_TYPE:
			tmp->data->value.index = realCounter++;
			if(realCounter > actualFunction->realTmpCount) // nahraj vetsi hodnotu
				actualFunction->realTmpCount = realCounter;
			tmp->data->type = REAL_VARIABLE;
			return tmp;

		case STRING_TYPE:
			tmp->data->value.index = stringCounter++;
			if(stringCounter > actualFunction->stringTmpCount) // nahraj vetsi hodnotu
				actualFunction->stringTmpCount = stringCounter;
			tmp->data->type = STRING_VARIABLE;
			return tmp;

		default: // STRING_TYPE || INTEGER_TYPE
			tmp->data->value.index = intCounter++;
			if(intCounter > actualFunction->intTmpCount) // nahraj vetsi hodnotu
				actualFunction->intTmpCount = intCounter;
			tmp->data->type = INT_VARIABLE;
			return tmp;
	}
}

/**
 * @brief Vytvoří operand daného typu (musí se později naplnit)
 */
operListT * createOperTypeEmpty(paramTypeT type)
{
	operListT * tmp = getDOperList(1);	
	tmp->type = type;
	return tmp;
}


/**
 * @brief Vytvoří operand konstanty různých typů
 */
operListT * createOperBoolConst(int value)
{
	operListT * tmp = getDOperList(1);
	tmp->data = getDVariableInfo(1);
	tmp->type = BOOLEAN_TYPE;
	tmp->data->value.intVal = value ? true : false;
	tmp->data->type = INT_CONSTANT;
	return tmp;
}
operListT * createOperIntConst(int value)
{
	operListT * tmp = getDOperList(1);
	tmp->data = getDVariableInfo(1);
	tmp->type = INTEGER_TYPE;
	tmp->data->value.intVal = value;
	tmp->data->type = INT_CONSTANT;
	return tmp;
}
operListT * createOperRealConst(double value)
{
	operListT * tmp = getDOperList(1);
	tmp->data = getDVariableInfo(1);
	tmp->type = REAL_TYPE;
	tmp->data->value.realVal = value;
	tmp->data->type = REAL_CONSTANT;
	return tmp;
}
operListT * createOperStringConst(char* value)
{
	operListT * tmp = getDOperList(1);
	tmp->data = getDVariableInfo(1);
	tmp->type = STRING_TYPE;
	tmp->data->value.stringVal = value;
	tmp->data->type = STRING_CONSTANT;
	return tmp;
}

/**
 * @brief Z aktuální tokenu vytvoří kód pro čtení
 */
void insertReadln()
{
	STVariableT * var= findVariable(actToken.value.idVal, localVariables);
	int level = INT_VARIABLE;
	if(var == NULL)
	{
		var = findVariable(actToken.value.idVal, globalVariables);

		if(var == NULL)
		{
			errCode = UNDEFINED_ID;
			fatalError("Nedefinovana promenna.\n");
		}
		level = INT_GLOBAL;
	}
	if (var->type == BOOLEAN_TYPE)
	{
		errCode = TYPE_ERROR;
		fatalError("Nacitani dat ze vstupu do boolean.\n");		
	}

	last3ak->next = getDTAC(1);
	last3ak = last3ak->next;
	last3ak->op1.varInfo = getDVariableInfo(1);
	last3ak->op1.varInfo->type = var->type - INTEGER_TYPE + level;

	last3ak->op1.varInfo->value.index = var->index;

	last3ak->instruction = var->type - INTEGER_TYPE + I_READLN_I;
}



/**
 * @brief Vloží návratovou instrukci
 */
void insertReturn()
{
	last3ak->next = getDTAC(1);
	last3ak = last3ak->next;
	last3ak->instruction = ((actualFunction->params->type - INTEGER_TYPE) % 3 ) + I_RETURN_I;	
}

/**
 * @brief Vloží kód pro instrukci I_CONTINUE
 */
threeAddressCodeT * insertContinue(operListT * condition)
{
	last3ak->next = getDTAC(1);
	last3ak = last3ak->next;
	last3ak->instruction = I_CONTINUE;
	last3ak->op1.varInfo = condition->data;
	return last3ak;
}

/**
 * @brief Vloží kód pro instrukci I_GOTO
 */
threeAddressCodeT * insertGoto()
{
	last3ak->next = getDTAC(1);
	last3ak = last3ak->next;
	last3ak->instruction = I_GOTO;
	return last3ak;
}

/**
 * @brief Vloží instrukci pro write
 */
bool insertWrite()
{
	if(actToken.type == RIGHT_PARENTHESIS)
	{
		tokenError(ID, "Prazdny write");
	}
	operListT * op1 = createOperTypeEmpty(ID);
	if(!isExpression(op1))
	{
		errCode = UNDEFINED_ID;
		fatalError("Chyba pri volani write\n");		
	}

	insertCode(op1->type - INTEGER_TYPE + (op1->data->type <= REAL_CONSTANT ? I_WRITE_IC : I_WRITE_IV), op1, NULL, NULL);
	return true;	
}

/**
 * @brief Inicializuje čítače dočasných proměnných
 */
void counterInit()
{
	removeDOperList(INT_MAX);
	intCounter = actualFunction->intVarCount;
	realCounter = actualFunction->realVarCount;
	stringCounter = actualFunction->stringVarCount;
}

/**
 * @brief Incalzuje funkci
 */
void functionInit()
{
	last3ak = getDTAC(1);
	actualFunction->beginning = last3ak;
	last3ak->instruction = I_LABEL;

	actualFunction->intTmpCount = actualFunction->intVarCount;
	actualFunction->realTmpCount = actualFunction->realVarCount;
	actualFunction->stringTmpCount = actualFunction->stringVarCount;

	counterInit();
}



//----------------------------SYNTAKTICKA ANALYZA-------------------------------------------


bool isProgram()
{
	initDPLI(INITDPLI_SIZE);
	initDOperList(INITDOPERLIST_SIZE);
	initDVariableInfo(INITDVARIABLEINFO_SIZE);
	initDParameter(INITDPARAMETER_SIZE);
	initDSTF(INITDSTF_SIZE);
	initDSTV(INITDSTV_SIZE);
	initTable();
	getToken();
	return isVariables() 
		&& ( (globalVariables = localVariables), isFunctions() )
		&& ( (actualFunction = findFunction("", functionTable)) , functionInit(), localVariables = NULL, isTerminal(BEGIN, "Hlavni telo programu"))
		&& ( checkDefinitions(functionTable), isInstructions()) 
		&& (last3ak->next = NULL, isTerminal(END, "Hlavni telo programu") )
		&& isTerminal(DOT, "Na konci programu.") 
		&& (last3ak->next = NULL,isTerminal(FILE_END, "na konci programu."));
}

bool isVariables()
{
	switch(actToken.type){
		case VAR:
			getToken();
			return isVariable();
		default:
			return true;
	}
}

bool isNextVariable()
{
	switch(actToken.type){
		case ID:
			return isVariable();
		default:
			return true;
	}
}

bool isVariable()
{
	STVariableT * tmp;
	switch(actToken.type){
		case ID:
			tmp = insertLocalVariable(actToken.value.idVal);
			getToken();
			return isTerminal(COLON, "Kontrola definice promenne.") 
				&& (setVariable(tmp, actToken.type), isType())
				&& isTerminal(SEMICOLON, "Kontrola definice promenne") 
				&& isNextVariable() ;
		default:
			tokenError(ID, "Po var nenasleduje definice promenne");
	}	
}

bool isArray()
{
	switch(actToken.type){
		case ARRAY:
			getToken();
			return isTerminal(LEFT_BRACKET,"Kontrola pole.") 
				&& isTerminal(INTEGER,"Kontrola pole.") 
				&& isTerminal(DOUBLEDOT,"Kontrola pole.") 
				&& isTerminal(INTEGER,"Kontrola pole.") 
				&& isTerminal(RIGHT_BRACKET,"Kontrola pole.") 
				&& isTerminal(OF,"Kontrola pole.");
		default:
			return true;
	}
}

bool isType()
{
	switch(actToken.type){
		case INTEGER_TYPE:
		case REAL_TYPE:
		case STRING_TYPE:
		case BOOLEAN_TYPE:
			getToken();
			return true;
		default:
			tokenError(INTEGER_TYPE, "Kontrolovani typu.");
	}
}

bool isFunctions()
{
	switch(actToken.type){
		case FUNCTION:
			getToken();
			return isId((actualFunction = insertFunctionTable(actToken.value.idVal)),"Kontrola  jmena funkce.")
				&& isTerminal(LEFT_PARENTHESIS,"Kontrola funkce - zacatek parametru.") 
				&& checkParams()
				&& isTerminal(SEMICOLON, "Kontrola funkce za parametrama.") 
				&& isFunctionBody() 
				&& isTerminal(SEMICOLON, "Kontrola funkce - konec.")
				&& isFunctions();
		default:
			return true;
	}	
}


bool checkParams()
{
	paramListItemT * param;
	if(actualFunction->declared)
	{
		param = actualFunction->params->next;
		actualFunction->defined = true;

		if(param != NULL && actToken.type == RIGHT_PARENTHESIS)
		{
			errCode = ID_REDEFINITION;
			fatalError("Pokus o redefinici funkce. Nedostatecny pocet parametru.\n");
		}
		if(param == NULL)
		{
			if(actToken.type == RIGHT_PARENTHESIS)
			{
				getToken();
			}
			else
			{
				if(actToken.type == ID)
				{
					errCode = ID_REDEFINITION;
					fatalError("Pokus o redefinici funkce. Nesouhlasi pocet parametru.\n");					
				}
				tokenError(RIGHT_PARENTHESIS, "Kontrola parametru funkce - konec.");				
			}
		}
		else
		while(param != NULL)
		{
			if(actToken.type != ID) // Kontrola ID
			{
				tokenError(ID, "Kontrola parametru funkce.");
			}
			if(strcmp(param->name, actToken.value.idVal) != 0)
			{
				errCode = ID_REDEFINITION;
				fatalError("Pokus o redefinici funkce. Nesouhlasi nazvy parametru.\n");
			}

			getToken();

			if(actToken.type != COLON) // Kontrola dvojtecky
			{
				tokenError(COLON, "Kontrola parametru funkce - mezi ID a TYPEM.");				
			}
			getToken();

			if(actToken.type != INTEGER_TYPE // Kontrola typu
			&& actToken.type != REAL_TYPE 
			&& actToken.type != BOOLEAN_TYPE
			&& actToken.type != STRING_TYPE)
			{
				tokenError(INTEGER_TYPE, "Kontrola typu parametru funkce.");
			}
			if(param->type != actToken.type)
			{
				errCode = ID_REDEFINITION;
				fatalError("Pokus o redefinici funkce. Nesouhlasi typy parametru.\n");
			}

			getToken();

			param = param->next; // kontrola toho, co následuje

			if(actToken.type == SEMICOLON)
			{
				if(param == NULL)
				{
					errCode = ID_REDEFINITION;
					fatalError("Pokus o redefinici funkce. Prilis mnoho parametru.\n");						
				}
				getToken();
			}
			else if(actToken.type == RIGHT_PARENTHESIS)
			{
				if(param != NULL)
				{
					errCode = ID_REDEFINITION;
					fatalError("Pokus o redefinici funkce. Prilis malo parametru.\n");						
				}
				getToken();
			}
			else
			{
				tokenError(RIGHT_PARENTHESIS, "Kontrola parametru funkce - konec.");				
			}
		}

		if(actToken.type != COLON)
		{
			tokenError(COLON, "Kontrola funkce - mezi PARAMS a TYPE.");
		}

		getToken();

		if(actToken.type != INTEGER_TYPE // Kontrola typu
		&& actToken.type != REAL_TYPE 
		&& actToken.type != BOOLEAN_TYPE
		&& actToken.type != STRING_TYPE)
		{
			tokenError(INTEGER_TYPE, "Kontrola typu funkce.");
		}
		if(actualFunction->params->type != actToken.type)
		{
			errCode = ID_REDEFINITION;
			fatalError("Pokus o redefinici funkce. Nesouhlasi typ funkce.\n");
		}
		getToken();

		return true;
	}
	else
	{
		actualFunction->declared = true;

		actualFunction->params = insertParam(actualFunction->name);

		return isParams(actualFunction->params)
			&& isTerminal(RIGHT_PARENTHESIS, "Kontrola funkce - konec parametru.") 
			&& isTerminal(COLON,"Kontrola funkce - mezi PARAMS a TYPE") 
			&& (setParam(actualFunction->params, actToken.type), isType());
	}
}

bool isParams(paramListItemT * param)
{
	switch(actToken.type){
		case ID:
			param->next = insertParam(actToken.value.idVal);
			param = param->next;
			getToken();
			return isTerminal(COLON, "Definice funkce - ID : TYPE.") 
				&& (setParam(param, actToken.type), isType())
				&& isNextParam(param);
		default:
			return true;
	}
}

bool isNextParam(paramListItemT * param)
{
	switch(actToken.type){
		case SEMICOLON:
			getToken();
			return isId((param->next = insertParam(actToken.value.idVal), param = param->next), "Definice funkce - parametr.")
				&& isTerminal(COLON, "Definice funkce - ID : TYPE.") 
				&& (setParam(param, actToken.type), isType())
				&& isNextParam(param);
		default:
			return true;
	}
}

bool isFunctionBody()
{
	if(actToken.type == FORWARD)
	{
		getToken();
		if(actualFunction->defined)
		{
			errCode = ID_REDEFINITION;
			fatalError("Druha deklarace funkce.\n");
		}
		return true;
	}
	else
	{
		actualFunction->defined = true;
		initLocalTable();
		return isVariables()
			&& (functionInit(), isSequence(insertReturn(), "Telo funkce."));
	}
}

bool isInstructions()
{
	switch(actToken.type){
		case END:
			return true;
		default:
			return isInstruction() 
				&& isNextInstruction();
	}		
}

bool isInstruction()
{
	switch(actToken.type){
		case ID:
		{
			counterInit();
			operListT * dest = createOper();
			if(dest == NULL)
			{
				if(findFunction( actToken.value.idVal, functionTable) != NULL) // nasla se fce
				{
					errCode = SYNTAX_ERROR;
					fatalError("Fce neni L-hodnota.\n");
				}
				errCode = UNDEFINED_ID;
				fatalError("Nedefinovana promenna %s.\n", actToken.value.idVal);
			}
			operListT * op1 = createOperTypeEmpty(dest->type);
			threeAddressCodeT * start = last3ak;
			getToken();
			// if(last3ak == start) // nevygeneroval se zadny kod
			// 	insertCode(I_ASSIGN, op1, NULL, dest); // vygenruj prirazeni
			// else
			// 	last3ak->dest.varInfo = dest->data; // zamenit posledni dest
			return isTerminal(ASSIGNMENT, "Prirazeni") 
				&& isExpression(op1) && (last3ak == start ? (insertCode(I_ASSIGN, op1, NULL, dest),true) : ((last3ak->dest.varInfo = dest->data), true));
		}
		case BEGIN:
			getToken();
			return isInstructions() 
				&& isTerminal(END, "Jen tak pro nic za nic.");
		case IF:
		{
			counterInit();
			threeAddressCodeT * condition;
			operListT * dest = createOperTypeEmpty(BOOLEAN_TYPE);
			getToken();
			return isExpression(dest) 
				&& ((condition = insertContinue(dest)), isTerminal(THEN, "Podminka"))
				&& isTerminal(BEGIN, "Podmika") && isInstructions() && isTerminal(END, "Podminka")
				&& isElseIf(condition);
		}
		case WHILE:
		{
			counterInit();
			threeAddressCodeT * label1 = last3ak;
			threeAddressCodeT * continueCode;
			operListT * dest = createOperTypeEmpty(BOOLEAN_TYPE);
			getToken();
			return isExpression(dest) 
				&& (continueCode = insertContinue(dest), isTerminal(DO,"While cyklus"))
				&& isSequence(( (continueCode->dest.code = insertGoto()) , (last3ak->dest.code = label1) ), "While cyklus");
		}
		case REPEAT:
		{
			threeAddressCodeT * label1 = last3ak;
			operListT * dest;
			getToken();
			return isInstruction() 
				&& isNextInstruction() 
				&& isTerminal(UNTIL, "Repeat - Until cyklus") 
				&& ((counterInit(), dest = createOperTypeEmpty(BOOLEAN_TYPE)),isExpression(dest))
				&& (insertContinue(dest)->dest.code = label1,true);
		}
		case READLN:
			getToken();
			return isTerminal(LEFT_PARENTHESIS, "Readln(ID)") 
				&& isId(insertReadln(), "Readln(ID)") 
				&& isTerminal(RIGHT_PARENTHESIS, "Readln(ID)");
		case WRITE:
			getToken();
			return isTerminal(LEFT_PARENTHESIS, "Write(ID)") 
				&& insertWrite()
				&& isNextWriteParams() 
				&& isTerminal(RIGHT_PARENTHESIS, "Write(ID)");
		default:
			tokenError(ID, "Poznavani instrukce.");
	}		
}

bool isNextInstruction()
{
	switch(actToken.type){
		case SEMICOLON:
			getToken();
			return isInstruction() 
				&& isNextInstruction();
		default:
			return true;
	}
}

bool isElseIf(threeAddressCodeT * condition)
{
	switch(actToken.type){
		case ELSE:
			getToken();
			if(condition == last3ak) // if větev je prazdna
			{
				last3ak->instruction = I_SKIP; // skoc, pokud condition true
				// if(condition == last3ak) // prazdna else vetev
				// 	last3ak->instruction = I_LABEL; // ze skoku udela label
				// else
				// 	condition->dest.code = last3ak;
				return isSequence( ( condition == last3ak ? ((void)(last3ak->instruction = I_LABEL)): ((void)(condition->dest.code = last3ak)) ) , "ELSE pro prazdnou IF vetev");
			}
			else // if větev neni prazdna
			{
				threeAddressCodeT * skip = insertGoto();
				condition->dest.code = last3ak;
				// if(skip == last3ak) // prazdna else vetev
				// 	last3ak->instruction = I_LABEL; // ze skoku udela label
				// else
				// 	skip->dest.code = last3ak;
				return isSequence( skip == last3ak ? ((void)(last3ak->instruction = I_LABEL)): ((void)(skip->dest.code = last3ak)), "ELSE vetev");
			}
		default:
			if(condition == last3ak) // if větev je prazdna
				last3ak->instruction = I_LABEL; // ze skoku udela label
			else
				condition->dest.code = last3ak;
			return true;
	}
}

bool isNextWriteParams()
{
	switch(actToken.type){
		case COMMA:
			getToken();
			return insertWrite() 
				&& isNextWriteParams();
		default:
			return true;
	}
}
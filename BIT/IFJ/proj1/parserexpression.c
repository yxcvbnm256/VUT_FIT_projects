/**
 * Projekt: Implementace interpretu imperativního jazyka IFJ14
 */

/**
 * @file parserexpression.c
 * @author Martin Nosek (xnosek10)
 * @brief Zdrojový soubor pro parsovaní výrazu
 */

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h" 
#include "parserexpression.h"
#include "globalvariables.h"
#include "tokens.h"
#include "scanner.h"
#include "errors.h"
#include "3ak.h"
#include "bst.h"
#include "memoryallocation2.h"
#include <limits.h>

#define RULE_MAX_L 3
#define NUMBER_OF_RULES 17


typedef struct { 
    sElemPtr Act;
    sElemPtr First;
} tStack;

void oInsertFirst(operListT *O){
	
	O->next = operList;
	operList = O;

}

void oDeleteFirst(){
	
	switch(operList->data->type)
	{
		case STRING_VARIABLE:
			if (operList->data->value.index >= actualFunction->stringVarCount && operList->data->value.index < stringCounter)
				--stringCounter;
			break;
		case REAL_VARIABLE:
			if (operList->data->value.index >= actualFunction->realVarCount && operList->data->value.index < realCounter)
				--realCounter;
			break;
		case INT_VARIABLE:
			if (operList->data->value.index >= actualFunction->intVarCount && operList->data->value.index < intCounter)
				--intCounter;
			break;
		default:
			break;
	}

	operList = operList->next;
}

void eInitStack (tStack *S) {

	S->Act = NULL;
	S->First = NULL;
}

/**
 * @brief Vyprázdní zásobník
 */
void eDisposeStack (tStack *S) {

	removeDSElem(INT_MAX);

	S->First = NULL;

	S->Act = NULL;
}

/**
 * @brief Vloží prvek s hodnotou val na vrchol zásobníku
 */
void eInsertFirst(tStack *S, int val){
	
	sElemPtr newItem = getDSElem(1);

    newItem->data = val;
    newItem->ptr = S->First;
    S->First = newItem;

}

/**
 * @brief vymaže prvek zásobníku nejblíže vrcholu
 */
void eDeleteFirst (tStack *S) {

	if(S->First == NULL)
		return;
	
	S->First = S->First->ptr;

	removeDSElem(1);
}	

/**
 * @brief Vrací terminál na zásobníku nejblíže vrcholu
 */
tokenTypeT top(tStack *S){

	sElemPtr isterminal;
	isterminal = S->First;
	
	do
	{
		if((isterminal->data) > TOKENS_COUNT)
			isterminal = isterminal->ptr;
		else
			return (isterminal->data);

	}while(isterminal != NULL);
	
	errCode = SYNTAX_ERROR;
	fatalError("Syntakticka chyba\n");

}

/**
 * @brief vrátí transformovanou hodnotu tokenu pro precedenční tabulku
 */
int transformTokenIndex(int x){
 	
 	if (x<ID)
 	{
 		return x-1;
 	}else if (x<INTEGER_TYPE)
 	{
 		return ID - 1;
 	}else
 	{
 		return ID;
 	}
}

/**
 * @brief vrátí true v případě, že na zásobníku je spracovatelný podvýraz
 */
bool expressionTop(tStack *S){
 	
 	S->Act = S->First;

 	if (S->Act->data == LEFT)
 		return false;

 	do{

 		if (S->Act->ptr->data == LEFT)
 			return true;
 		if (S->Act->ptr->data == DOLLAR)
 			return false;

 		S->Act = S->Act->ptr;

 	}while(S->Act->data != DOLLAR);
 	
 	return false;

}
/**
 * @brief Vrátí true v případě, že operandy jsou kompaktibilní
 */
bool isCompatibleOper(operListT* op1, operListT* op2, int i){
	
	if(i == 1 || i == 4 || i == 5)
		return((i==4 && op1->type == STRING_TYPE && op2->type == STRING_TYPE)||((op1->type == INTEGER_TYPE || op1->type == REAL_TYPE)&&(op2->type == INTEGER_TYPE||op2->type == REAL_TYPE)));

	else if(i == 2)
		return((op1->type == INTEGER_TYPE || op1->type == REAL_TYPE)&&(op2->type == INTEGER_TYPE||op2->type == REAL_TYPE));

	else if( i>7 && i<14)
		return(op1->type == op2->type);

	else
		return((op1->type == op2->type)&&(op1->type == BOOLEAN_TYPE));

}

void insertCode(instructionT instruction, operListT *op1, operListT *op2, operListT *dest){
	last3ak->next = getDTAC(1);
	last3ak = last3ak->next;
	last3ak->instruction = instruction;
	if(op1 != NULL)
		last3ak->op1.varInfo = op1->data;
	if(op2 != NULL)
		last3ak->op2.varInfo = op2->data;
	if(dest != NULL)
		last3ak->dest.varInfo = dest->data;
}
/**
 * @brief Generuje kód pomocí funkce insertCode na základě aplikovaného pravidla
 */
void generateCode(int i){

	operListT *op1 = NULL;
	operListT *op2 = NULL;

	switch(i){
	 case 0://NOT
	 	op1 = operList;
	 	if(op1->type != BOOLEAN_TYPE)
	 	{
	 		errCode = TYPE_ERROR;
	 		fatalError("Semantic error - uncompatible types - NOT E\n");
	 	}
	 	else
	 	{
		 	operListT* dest;
		 	if(op1->data->type == INT_CONSTANT)
		 	{
		 		dest = createOperBoolConst(!op1->data->value.intVal);
		 	}
		 	else
		 	{
		 		dest = createOperType(BOOLEAN_TYPE);
		 		insertCode(I_NOT, op1, op2, dest);		 		
		 	}
		 	oDeleteFirst();
		 	oInsertFirst(dest);
	 	}
	 	return;
	 case 1://MULTIPLY
	 	op2 = operList;
	 	op1 = op2->next;
	 	if(!isCompatibleOper(op1,op2,i))
	 	{
	 		errCode = TYPE_ERROR;
	 		fatalError("Semantic error - uncompatible types - E * E\n");
	 	}
	 	else
	 	{
	 		oDeleteFirst();
	 		oDeleteFirst();

	 		operListT* dest;

	 		
	 		dest = (op1->type==REAL_TYPE||op2->type==REAL_TYPE) ? createOperType(REAL_TYPE) : createOperType(INTEGER_TYPE);
	 		insertCode(I_MULT, op1, op2, dest);
	 		
	 		oInsertFirst(dest);
	 	}
	 	return;
	 case 2://DIVIDE
	 	op2 = operList;
	 	op1 = op2->next;
	 	if(!isCompatibleOper(op1,op2,i))
	 	{
	 		errCode = TYPE_ERROR;
	 		fatalError("Semantic error - uncompatible types - E / E\n");
	 	}
	 	else
	 	{
	 		operListT* dest;
	 		oDeleteFirst();
	 		oDeleteFirst();
	 		 dest =  createOperType(REAL_TYPE);
	 		oInsertFirst(dest);
	 		insertCode(I_DIV, op1, op2, dest);
	 	}
	 	return;
	 case 3://AND
	 	op2 = operList;
	 	op1 = op2->next;
	 	if(!isCompatibleOper(op1,op2,i))
	 	{
	 		errCode = TYPE_ERROR;
	 		fatalError("Semantic error - uncompatible types - E AND E\n");
	 	}
	 	else
	 	{
	 		operListT* dest;
	 		oDeleteFirst();
	 		oDeleteFirst();
	 		 dest = createOperType(BOOLEAN_TYPE);
	 		oInsertFirst(dest);
	 		insertCode(I_AND, op1, op2, dest);
	 	}
	 	return;
	 case 4://PLUS
	 	op2 = operList;
	 	op1 = op2->next;
	 	if(!isCompatibleOper(op1,op2,i))
	 	{
	 		errCode = TYPE_ERROR;
	 		fatalError("Semantic error - uncompatible types - E + E\n");
	 	}
	 	else
	 	{
	 		operListT* dest;
	 		oDeleteFirst();
	 		oDeleteFirst();
	 		 dest = (op1->type==REAL_TYPE||op2->type==REAL_TYPE) ? createOperType(REAL_TYPE) : 
	 							( op1->type == STRING_TYPE ? createOperType(STRING_TYPE) : createOperType(INTEGER_TYPE) );
	 		oInsertFirst(dest);
	 		insertCode(I_ADD, op1, op2, dest);
	 	}
	 	return;
	 case 5://MINUS
	 	op2 = operList;
	 	op1 = op2->next;
	 	if(!isCompatibleOper(op1,op2,i))
	 	{
	 		errCode = TYPE_ERROR;
	 		fatalError("Semantic error - uncompatible types - %s - %s\n", tokenRewrite[op1->type], tokenRewrite[op2->type]);
	 	}
	 	else
	 	{
	 		operListT* dest;
	 		oDeleteFirst();
	 		oDeleteFirst();
	 		 dest = (op1->type==REAL_TYPE||op2->type==REAL_TYPE) ? createOperType(REAL_TYPE) : createOperType(INTEGER_TYPE);
	 		oInsertFirst(dest);
	 		insertCode(I_SUB, op1, op2, dest);
	 	}
		return;
	 case 6://OR
	 	op2 = operList;
	 	op1 = op2->next;
	 	if(!isCompatibleOper(op1,op2,i))
	 	{
	 		errCode = TYPE_ERROR;
	 		fatalError("Semantic error - uncompatible types - E OR E\n");
	 	}
	 	else
	 	{
	 		operListT* dest;
	 		oDeleteFirst();
	 		oDeleteFirst();
	 		 dest = createOperType(BOOLEAN_TYPE);
	 		oInsertFirst(dest);
	 		insertCode(I_OR, op1, op2, dest);
	 	}
	 	return;
	 case 7://XOR
	 	op2 = operList;
	 	op1 = op2->next;
	 	if(!isCompatibleOper(op1,op2,i))
	 	{
	 		errCode = TYPE_ERROR;
	 		fatalError("Semantic error - uncompatible types - E XOR E\n");
	 	}
	 	else
	 	{
	 		operListT* dest;
	 		oDeleteFirst();
	 		oDeleteFirst();
	 		 dest = createOperType(BOOLEAN_TYPE);
	 		oInsertFirst(dest);
	 		insertCode(I_XOR, op1, op2, dest);
	 	}
	 	return;
	 case 8://EQUAL
	 	op2 = operList;
	 	op1 = op2->next;
	 	if(!isCompatibleOper(op1,op2,i))
	 	{
	 		errCode = TYPE_ERROR;
	 		fatalError("Semantic error - uncompatible types - E = E\n");
	 	}
	 	else
	 	{
	 		operListT* dest;
	 		oDeleteFirst();
	 		oDeleteFirst();
	 		 dest = createOperType(BOOLEAN_TYPE);
	 		oInsertFirst(dest);
	 		insertCode(I_EQUAL, op1, op2, dest);
	 	}
	 	return;
	 case 9://LESS
	 	op2 = operList;
	 	op1 = op2->next;
	 	if(!isCompatibleOper(op1,op2,i))
	 	{
	 		errCode = TYPE_ERROR;
	 		fatalError("Semantic error - uncompatible types - E < E\n");
	 	}
	 	else
	 	{
	 		operListT* dest;
	 		oDeleteFirst();
	 		oDeleteFirst();
	 		 dest = createOperType(BOOLEAN_TYPE);
	 		oInsertFirst(dest);
	 		insertCode(I_LESS, op1, op2, dest);
	 	}
	 	return;
	 case 10://MORE
	 	op2 = operList;
	 	op1 = op2->next;
	 	if(!isCompatibleOper(op1,op2,i))
	 	{
	 		errCode = TYPE_ERROR;
	 		fatalError("Semantic error - uncompatible types - E > E\n");
	 	}
	 	else
	 	{
	 		operListT* dest;
	 		oDeleteFirst();
	 		oDeleteFirst();
	 		 dest = createOperType(BOOLEAN_TYPE);
	 		oInsertFirst(dest);
	 		insertCode(I_MORE, op1, op2, dest);
	 	}
	 	return;
	 case 11://NOT_EQUAL
	 	op2 = operList;
	 	op1 = op2->next;
	 	if(!isCompatibleOper(op1,op2,i))
	 	{
	 		errCode = TYPE_ERROR;
	 		fatalError("Semantic error - uncompatible types - E <> E\n");
	 	}
	 	else
	 	{
	 		operListT* dest;
	 		oDeleteFirst();
	 		oDeleteFirst();
	 		 dest = createOperType(BOOLEAN_TYPE);
	 		oInsertFirst(dest);
	 		insertCode(I_NOT_EQUAL, op1, op2, dest);
	 	}
	 	return;
	 case 12://LESS_OR_EQUAL
	 	op2 = operList;
	 	op1 = op2->next;
	 	if(!isCompatibleOper(op1,op2,i))
	 	{
	 		errCode = TYPE_ERROR;
	 		fatalError("Semantic error - uncompatible types - E <= E\n");
	 	}
	 	else
	 	{
	 		operListT* dest;
	 		oDeleteFirst();
	 		oDeleteFirst();
	 		 dest = createOperType(BOOLEAN_TYPE);
	 		oInsertFirst(dest);
	 		insertCode(I_LESS_OR_EQUAL, op1, op2, dest);
	 	}
	 	return;
	 case 13://MORE_OR_EQUAL
	 	op2 = operList;
	 	op1 = op2->next;
	 	if(!isCompatibleOper(op1,op2,i))
	 	{
	 		errCode = TYPE_ERROR;
	 		fatalError("Semantic error - uncompatible types - E >= E\n");
	 	}
	 	else
	 	{
	 		operListT* dest;
	 		oDeleteFirst();
	 		oDeleteFirst();
	 		 dest = createOperType(BOOLEAN_TYPE);
	 		oInsertFirst(dest);
	 		insertCode(I_MORE_OR_EQUAL, op1, op2, dest);
	 	}
	 	return;
	 case 16://unarni minus
	 	op1 = operList;

	 	if(op1->type != INTEGER_TYPE && op1->type != REAL_TYPE)
	 	{
	 		errCode = TYPE_ERROR;
	 		fatalError("Semantic error - uncompatible types - Unarni minus\n");
	 	}
	 	else
	 	{
		 	operListT* dest;
		 	if(op1->data->type == INT_CONSTANT||op1->data->type == REAL_CONSTANT)
		 	{
		 		dest = (op1->data->type == INT_CONSTANT) ? createOperIntConst(op1->data->value.intVal * (-1)) : createOperRealConst(op1->data->value.realVal * (-1));
		 	}
		 	else
		 	{
		 		op2 = createOperIntConst(0);
		 		dest = (op1->type == INTEGER_TYPE) ? createOperType(INTEGER_TYPE) : createOperType(REAL_TYPE);
		 		insertCode(I_SUB, op2, op1, dest);		 		
		 	}
		 	oDeleteFirst();
		 	oInsertFirst(dest);
	 	}
	 	return;
	 default://PARENTHESIS nebo ID nejaky error
	 	return;
	}
	return;
}

/**
 * @brief Vrací true v případě, že transformace výrazu je v pravidlech. Zároveň generuje kód
 */
bool isInRules(tStack *S){

 	char stack[RULE_MAX_L+1];
 	stack[RULE_MAX_L] = '\0';
 	int x=RULE_MAX_L;
 	S->Act = S->First;
 	
 	do{
 	 	--x;
 	 	stack[x] = S->Act->data;
 	 	S->Act = S->Act->ptr;
 	}while (S->Act->data!=LEFT);

 	for (int i = 0; i < NUMBER_OF_RULES; ++i)
 	{
	 	if(!strcmp(&(stack[x]),expressionRuleTable[i])){
	 	 	generateCode(i);
	 	 	return true;
		}
	 	 
 	}
 	return false;

}

/**
 * @brief Vrátí true v případě, že funkce má správný počet správných parametrů
 */
bool validParameters(paramListItemT* params){

	operListT * pom;
	
	if(params == NULL)
		pom = NULL;
	else
	{
		pom = createOperTypeEmpty(params->type);
		oInsertFirst(pom);
	}

	if(isExpression(pom))
	{	

		if(actToken.type == RIGHT_PARENTHESIS && params->next == NULL)
			return true;
		if(actToken.type == RIGHT_PARENTHESIS && params->next != NULL)
		{
			errCode = TYPE_ERROR;
			fatalError("Semantic error - malý počet parametrů\n");
		}
			

		if(actToken.type == COMMA)
		{
			getToken();
			return validParameters(params->next);
		}
		else
			return false;
	}
	else
	{
		return false;
	}

}

/**
 * @brief Vrátí true v případě, že příchozí posloupnost tokenů tvoří funkci
 */
bool isFunction(STFunctionT* function){

	getToken();

	if(actToken.type == RIGHT_PARENTHESIS) // funkce bez parametru
	{
		if(function->params->next != NULL) // pokud mela mit parametry
		{
			errCode = TYPE_ERROR;
			fatalError("Funkce se vola s nedostatecnym poctem parametru\n");
		}

		return true;
	}
	return (validParameters(function->params->next));

}

void insertCallCode(STFunctionT * function)
{

	if(!isFunction(function)){
		errCode = SYNTAX_ERROR;
		fatalError("Syntax error - chyba volani funkce\n");
	}

	//generovani kodu pro I_CALL
	last3ak->next = getDTAC(1);
	last3ak = last3ak->next;

	last3ak->op1.function = function;
	paramListItemT* params = function->params->next;
	parameterT * parameters = NULL;
	parameterT * tmpPar;
	while(params != NULL)
	{
		tmpPar = getDParameter(1);
		tmpPar->next = parameters;
		tmpPar->data = operList->data;
		oDeleteFirst();
		parameters = tmpPar;
		params = params->next;
	}
	last3ak->op2.parameters = parameters;

	if(!(strcmp(function->name, "length")))
	{
		last3ak->instruction = I_CALL_LENGTH;
		last3ak->op1.varInfo = last3ak->op2.parameters->data;
	}
	else if(!(strcmp(function->name, "sort")))
		last3ak->instruction = I_CALL_SORT;
	else if(!(strcmp(function->name, "find")))
		last3ak->instruction = I_CALL_FIND;
	else if(!(strcmp(function->name, "copy")))
		last3ak->instruction = I_CALL_COPY;
	else
		last3ak->instruction = I_CALL;

	operListT * pomOper = createOperType(function->params->type);
	last3ak->dest.varInfo = pomOper->data;

	oInsertFirst(pomOper);
	getToken(); // RIGHT_PARENTHESIS
}

//********************************************************
bool isExpression(operListT * retOper){
	
	tStack stackE;
	eInitStack(&stackE);
	eInsertFirst(&stackE, DOLLAR);
	int a,b;
	

	do{
		 a = top(&stackE);
		 b = actToken.type;

		int topTerminalS = transformTokenIndex(a); //topTerminalS hodnota terminálu na zásobníku nejblíže vrcholu pro precedenční tabulku
		int inToken = transformTokenIndex(b); //inToken hodnota tokenu pro precedencni tabulku


		if (precedentialTable[topTerminalS][inToken]==MIDDLE)//PÁROVÁNÍ ZÁVOREK
		{
			eInsertFirst(&stackE,b);
			getToken();
		}
		else if (precedentialTable[topTerminalS][inToken]==LEFT)//SHIFT
		{
			if(inToken == ID-1)
			{
				operListT * pomOper = createOper();
				
				if(pomOper==NULL)
				{
					STFunctionT * pomFunc = findFunction(actToken.value.idVal, functionTable);
					if(pomFunc == NULL)
					{
						errCode = UNDEFINED_ID;
						fatalError("Pokus o zavolani nedefinovane promenne.\n");
					}
					else
					{
						getToken();
						if(actToken.type == LEFT_PARENTHESIS)
						{	
							insertCallCode(pomFunc);
						}
						else
						{
							errCode = 5; //TD
							fatalError("Semantic error");
						}
					}
				}
				else if(b == ID)
				{
					char * name = actToken.value.idVal;
					getToken();
					if(actToken.type == LEFT_PARENTHESIS)
					{
						STFunctionT * pomFunc = findFunction(name, functionTable);
						if(pomFunc == NULL)
						{
							errCode = UNDEFINED_ID;
							fatalError("Semantic error - volam funkci \"%s\", ktera neni definovana\n", name);
						}
						else
						{
							insertCallCode(pomFunc);
						}
					}
					else
						oInsertFirst(pomOper);
				}
				else
				{
					oInsertFirst(pomOper);
					getToken();
				}
				eInsertFirst(&stackE, LEFT);
				eInsertFirst(&stackE, ID);
			}
			else
			{
				if(stackE.First->data == E)
				{
					int c = stackE.First->data;
					eDeleteFirst(&stackE);
					eInsertFirst(&stackE,LEFT);
					eInsertFirst(&stackE,c);
					eInsertFirst(&stackE,actToken.type);
				}
				else
				{
					eInsertFirst(&stackE, LEFT);
					eInsertFirst(&stackE, actToken.type);
				}	
				getToken();
				
			}
		}
		else if (precedentialTable[topTerminalS][inToken]==RIGHT)//APLIKACE PRAVIDLA
		{
			if(stackE.First->data != E && actToken.type == MINUS && stackE.First->data != ID)
			{	
				eInsertFirst(&stackE, LEFT);
				eInsertFirst(&stackE, actToken.type);
				getToken();
			}
			else
			{
				if (isInRules(&stackE))
				{
					//vypis promennou globalnipromennapravidla – ergo vypiš nejpravější derivaci
					
					while(stackE.First->data != LEFT){
						eDeleteFirst(&stackE);
					}
					stackE.First->data = E;
				}
				else
				{
					errCode = SYNTAX_ERROR;
					fatalError("Syntax error1\n");
				}
			}		
		}
		else if(precedentialTable[topTerminalS][inToken]==BLANK)//CHYBÍ PRECEDENČNÍ PRAVIDLO
		{
			errCode = SYNTAX_ERROR;
			fatalError("%s\nSyntax error2\n", tokenRewrite[actToken.type]);
		}
		else if(precedentialTable[topTerminalS][inToken]==FUNC)
		{
			break;
		}
		else
		{	
			errCode = 99;
			fatalError("Unexpected fault - Expression parser\n");
		}

	} while (top(&stackE) != TOKENS_COUNT || actToken.type < INTEGER_TYPE);

	if(retOper == NULL)
	{
		errCode = TYPE_ERROR;
		fatalError("Semantic error - nepsravny počet parametrů\n");
	}

	if(retOper->type == ID) // muze to byt jakykoliv typ
	{
		retOper->type = operList->type;
	}
	else if(operList->type !=retOper->type)
	{
		errCode = TYPE_ERROR;
		fatalError("Nestejne typy pro prirazeni\n ocekavan: %s\n zadan: %s\n", tokenRewrite[retOper->type], tokenRewrite[operList->type]);
	}
	retOper->data = operList->data; // prehodim data u operandu
	oDeleteFirst();

	return true;

}

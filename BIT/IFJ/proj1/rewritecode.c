/**
 * Projekt: Implementace interpretu imperativního jazyka IFJ14
 */
/**
 * @file rewrite.c
 * @author Michal Janousek(xjanou15)
 * @brief Funkce pro projiti a vypsani kodu.
 */

#include "3ak.h"
#include <stdio.h>
#include "rewritecode.h"

void writeSep()
{
	printf(" | ");
}

void writeNL()
{
	printf("\n");
}

#define possibleBreak if(code->instruction >= I_RETURN_I && code->instruction <= I_RETURN_R) break

void writeInstruction(threeAddressCodeT * code)
{
	char * rewriteTable[] = {
		"I_CALL         ",
		"I_LABEL        ",
		"I_RETURN_I     ",
		"I_RETURN_S     ",
		"I_RETURN_R     ",
		"I_CONTINUE     ",
		"I_SKIP         ",
		"I_GOTO         ",
		"I_WRITE_IV     ",
		"I_WRITE_SV     ",
		"I_WRITE_RV     ",
		"I_WRITE_BV     ",
		"I_WRITE_IC     ",
		"I_WRITE_SC     ",
		"I_WRITE_RC     ",
		"I_WRITE_BC     ",
		"I_READLN_I     ",
		"I_READLN_S     ",
		"I_READLN_R     ",
		"I_CALL_LENGTH  ",
		"I_CALL_COPY    ",
		"I_CALL_FIND    ",
		"I_CALL_SORT    ",
		"I_ASSIGN       ",
		"I_ADD          ",
		"I_SUB          ",
		"I_MULT         ",
		"I_DIV          ",
		"I_NOT          ",
		"I_AND          ",
		"I_OR           ",
		"I_XOR          ",
		"I_EQUAL        ",
		"I_LESS         ",
		"I_MORE         ",
		"I_NOT_EQUAL    ",
		"I_LESS_OR_EQUAL",
		"I_MORE_OR_EQUAL"
	};
	printf("%s",rewriteTable[code->instruction]);
}



void writeIntConst(variableInfoT * var)
{
	printf("IC : %d",var->value.intVal);
}
void writeStringConst(variableInfoT * var)
{
	printf("SC : %s",var->value.stringVal);
}
void writeRealConst(variableInfoT * var)
{
	printf("RC : %g",var->value.realVal);
}

void writeIntVar(variableInfoT * var)
{
	printf("IV : %u",var->value.index);
}
void writeStringVar(variableInfoT * var)
{
	printf("SV : %u",var->value.index);
}
void writeRealVar(variableInfoT * var)
{
	printf("RV : %u",var->value.index);
}

void writeIntGlob(variableInfoT * var)
{
	printf("IG : %u",var->value.index);
}
void writeStringGlob(variableInfoT * var)
{
	printf("SG : %u",var->value.index);
}
void writeRealGlob(variableInfoT * var)
{
	printf("RG : %u",var->value.index);
}

typedef void (*writeVarFunctionT)(variableInfoT *);

writeVarFunctionT writeVarFunction[] = {
	&writeIntConst, &writeStringConst, &writeRealConst,
	&writeIntVar, &writeStringVar, &writeRealVar,
	&writeIntGlob, &writeStringGlob, &writeRealGlob
};

void writeVar(variableInfoT * var)
{
	(writeVarFunction[var->type])(var);
}

void writeParams(threeAddressCodeT * code)
{
	parameterT * param = code->op2.parameters;
	while(param != NULL)
	{
		writeVar(param->data);
		if(param->next != NULL)
			printf(" ; ");
		param = param->next;
	}
}

void writeFunction(threeAddressCodeT * code)
{
	printf("%s",code->op1.function->name);
}

void writeOp1(threeAddressCodeT * code)
{
	writeVar(code->op1.varInfo);
}
void writeOp2(threeAddressCodeT * code)
{
	writeVar(code->op2.varInfo);
}
void writeDest(threeAddressCodeT * code)
{
	writeVar(code->dest.varInfo);
}



void writeOnlyInstruction(threeAddressCodeT * code)
{
	writeInstruction(code);
	writeNL();
}
void writeCall(threeAddressCodeT * code)
{
	writeInstruction(code);
	writeSep();
	writeFunction(code);
	writeSep();
	writeParams(code);
	writeSep();
	writeDest(code);
	writeNL();
	writeCode(code->op1.function->beginning);
}
void writeWithOp1(threeAddressCodeT * code)
{
	writeInstruction(code);
	writeSep();
	writeOp1(code);
	writeNL();
}
void writeWithOp1Dest(threeAddressCodeT * code)
{
	writeInstruction(code);
	writeSep();
	writeOp1(code);
	writeSep();
	writeDest(code);
	writeNL();
}
void writeComplet(threeAddressCodeT * code)
{
	writeInstruction(code);
	writeSep();
	writeOp1(code);
	writeSep();
	writeOp2(code);
	writeSep();
	writeDest(code);
	writeNL();
}

typedef void (*writeCodeT)(threeAddressCodeT *);

void writeCode(threeAddressCodeT * code)
{
	writeCodeT callTable[] = {
		writeCall,
		writeOnlyInstruction,
		writeOnlyInstruction,
		writeOnlyInstruction,
		writeOnlyInstruction,
		writeWithOp1,
		writeWithOp1,
		writeOnlyInstruction,
		writeWithOp1,
		writeWithOp1,
		writeWithOp1,
		writeWithOp1,
		writeWithOp1,
		writeWithOp1,
		writeWithOp1,
		writeWithOp1,
		writeWithOp1,
		writeWithOp1,
		writeWithOp1,
		writeWithOp1Dest,
		writeCall,
		writeCall,
		writeCall,
		writeWithOp1Dest,
		writeComplet,
		writeComplet,
		writeComplet,
		writeComplet,
		writeWithOp1Dest,
		writeComplet,
		writeComplet,
		writeComplet,
		writeComplet,
		writeComplet,
		writeComplet,
		writeComplet,
		writeComplet,
		writeComplet
	};
	while(code != NULL)
	{
		(callTable[code->instruction])(code);
		possibleBreak;
		code = code->next;
	}

}



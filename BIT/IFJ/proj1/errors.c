/**
 * Projekt: Implementace interpretu imperativního jazyka IFJ14
 */
/**
 * @file errors.c
 * @author Michal Janousek(xjanou15)
 * @brief Error funkce
 */

#include "globalvariables.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "errors.h"
#include "memoryallocation2.h"

void warning(const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);

    vfprintf(stderr, fmt, args);
    va_end(args);
}

void fatalError(const char *fmt, ...)
{

    va_list args;

    va_start(args, fmt);

    vfprintf(stderr, fmt, args);
    va_end(args);
    releaseAll();
    exit(errCode);
}

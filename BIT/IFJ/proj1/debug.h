/**
 * Projekt: Implementace interpretu imperativního jazyka IFJ14
 */
/**
 * @file debug.h
 * @author Michal Janousek(xjanou15)
 * @brief Hlavičkový soubor pro debug
 */

#ifndef _DEBUG_H
#define _DEBUG_H

#ifdef _DEBUG_

#define debug(code)  code

#else

#define debug(code) 

#endif


#endif // _DEBUG_H

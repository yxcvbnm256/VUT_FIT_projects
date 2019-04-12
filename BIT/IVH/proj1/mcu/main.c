/*******************************************************************************
   user_main: main for LED diode test
   Copyright (C) 2009 Brno University of Technology,
                      Faculty of Information Technology

   LICENSE TERMS

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the
      distribution.
   3. All advertising materials mentioning features or use of this software
      or firmware must display the following acknowledgement:

        This product includes software developed by the University of
        Technology, Faculty of Information Technology, Brno and its
        contributors.

   4. Neither the name of the Company nor the names of its contributors
      may be used to endorse or promote products derived from this
      software without specific prior written permission.

   This software or firmware is provided ``as is'', and any express or implied
   warranties, including, but not limited to, the implied warranties of
   merchantability and fitness for a particular purpose are disclaimed.
   In no event shall the company or contributors be liable for any
   direct, indirect, incidental, special, exemplary, or consequential
   damages (including, but not limited to, procurement of substitute
   goods or services; loss of use, data, or profits; or business
   interruption) however caused and on any theory of liability, whether
   in contract, strict liability, or tort (including negligence or
   otherwise) arising in any way out of the use of this software, even
   if advised of the possibility of such damage.

   $Id$


*******************************************************************************/



#include <fitkitlib.h>  

#define BRAM_BASE 0


unsigned int iter; //globalni promenna 

/*******************************************************************************
 * Vypis uzivatelske napovedy (funkce se vola pri vykonavani prikazu "help")
*******************************************************************************/
void print_user_help(void)
{
  term_send_str("FLASH ERASE ALL... Smazani FLASH pameti"); //Vypis retezce na terminal 
  term_send_crlf();
}

/*******************************************************************************
 * Dekodovani uzivatelskych prikazu a jejich vykonavani
*******************************************************************************/
unsigned char decode_user_cmd(char *cmd_ucase, char *cmd)
{
  if (strcmp(cmd_ucase,"FLASH ERASE ALL") == 0) {
      FLASH_EraseAll();       
    }
  return (CMD_UNKNOWN);
}

/*******************************************************************************
 * Inicializace periferii/komponent po naprogramovani FPGA
*******************************************************************************/
void fpga_initialized() {}

/*******************************************************************************
 * Hlavni funkce
*******************************************************************************/
int main(void)
{ 
  short counter = 0;
  iter = 0;
  initialize_hardware();
  term_send_str("  SID PLAYER - WELCOME");
  term_send_crlf(); 
 
  unsigned int offset = 0;
  unsigned int page = FLASH_USER_DATA_PAGE;
  unsigned int reg = 0, buf;
   
  delay_ms(10);
                                     
  set_led_d6(1);  //rozsvitit LED D6
  while (1)    
  {
    
    //Nacitani dat z FLASH pameti
    FLASH_ReadData((unsigned char *)&buf, 1, page, offset);  
   
    
    offset += 1;
    if (offset >= PAGE_SIZE) 
    {
      offset -= PAGE_SIZE;
      page += 1;
      
      if (FLASH_isPageBlank(page) == 1)
      {
        page = FLASH_USER_DATA_PAGE;
        offset = reg = buf = 0;
        continue;
      }     
    }

    //Posilani nacteneho bloku z FLASH do FPGA
    FPGA_SPI_RW_AN_DN(SPI_FPGA_ENABLE_WRITE, reg, (unsigned char *) &buf, 1, 1);    
    if (++reg == 25)
    {
      terminal_idle();  // obsluha terminalu
      delay_ms(20);
      reg = 0;
    }
  }
}


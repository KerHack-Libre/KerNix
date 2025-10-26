// SPDX-License-Identifier : GPL-2.0+
/* 
 * Un clone de la commande 'clear' present sur la pluspart
 * des systemes d'exploitation unix. 
 * 
 * Copyright (C) 2025  KerHack-Libre 
 * Author:   Umar BA <jUmarB@protonmail.com> 
 *
 */


#include <curses.h> 
#include <stdlib.h>
#include <stdio.h>
#include <term.h>

#define  __STR(__x) #__x 
#define _STRINGIFY(__DEFINE)                        \
  __STR(__DEFINE)  

#if !defined(CLS_VERSION)
# define CLS_VERSION_STR                            \
  _STRINGIFY(NCURSES_VERSION_MAJOR) "."             \
  _STRINGIFY(NCURSES_VERSION_MINOR) "."             \
  _STRINGIFY(NCURSES_VERSION_PATCH) 
#else 
# define CLS_VERSION_STR  _STRINGIFY(CLS_VERSION) 
#endif 

#define PRINT_VERSION(...)  \
  printf("cls version %s by KerHack-Libre version \012", CLS_VERSION_STR ) 

#define CLS_USAGE \
  "Usage : %s [OPTION]\012\012"                     \
  "   -h    \011Show this help\012"                 \
  "   -x [n]\011do not try to clear scrollback\012"


static void show_usage(char * const *av)   
{
   char *basename = (char *)*av ; 
   fprintf(stdout , CLS_USAGE, basename)  ;
   puts("") ; 
} 

static void prevent_clearing_scrolback_buffer(const int  nlines) 
{
   putp(tparm(parm_index ,nlines)); 
   putp(tparm(change_scroll_region,  0 , nlines));  
}

static void clearing_section(const int nlines)  
{
   putp(tparm(parm_index,nlines)) ;
}

int main(int ac , char * const *av) 
{ 

  int pstatus=EXIT_SUCCESS ,
      erret = 0,
      nrows=0;   
  char *flags=00; 
  
  setvbuf(stdout ,(void *)0 ,  _IONBF , 0) ;  

  if(ERR == setupterm((void *)0 ,  1 , &erret)) 
  {
     switch(erret)  
     {
       case ~0 : fprintf(stderr, "Cannot find terminfo database\012");break; 
       case  0 : fprintf(stderr, "No Enought data found to perform operation\012");break; 
       case  1 : fprintf(stderr, "Can't use Curses\012"); break ; 
     }

     pstatus^=EXIT_FAILURE; 
     goto  _eplg; 
  }
 
  if(!(ac &~(1)))
  {
     putp(clear_screen); 
     goto _eplg ;
  }
  
  flags  = *(av+1) ; 

  if( *(flags) ^ '-')
  {
    show_usage(av) ; 
    goto _eplg; 
  }

  /* Supported flags */
  switch( *(flags+1) & 0xff )  
  {
     case  'x': 
       nrows = lines; 
       if(ac >=3 )  
       {
         nrows = strtol(*(av+2) ,00, 10) ;  
         nrows = nrows?  nrows : lines;
       } 

       if(!(lines ^ nrows) ||  nrows >  lines) 
         prevent_clearing_scrolback_buffer(nrows); 
       else 
         clearing_section(nrows);  
       
       break; 
     case  'V':
       PRINT_VERSION() ;  
       break;
     case 'h': 
     default : 
       show_usage(av) ; 
       PRINT_VERSION(); break ; 
  }

 
  (void) reset_shell_mode ;

_eplg: 
  return pstatus ; 
}

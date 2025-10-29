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

/*  LIST OF USED CAPABILITIES  */
#define CSR   3
#define CLS   5 
#define PDL   106  
#define PIX   109 
#define PUC   114 

#define cap(capid) \
  ((TERMTYPE*)cur_term)->Strings[capid]  

#define tp(capstring , ...)  \
  tparm(capstring , __VA_ARGS__) 

#define PRINT_VERSION(...)  \
  printf("cls version %s by KerHack-Libre version \012", CLS_VERSION_STR ) 

#define CLS_USAGE \
  "Usage : %s [OPTION]...[NUMBER]\012\012"          \
  "   -h    \011Show this help\012"                 \
  "   -v    \011Current version\012"                \
  "   -g [n]\011Insert an empty gap between\012"


static void show_usage(char * const *av)   
{
   char *basename = (char *)*av ; 
   fprintf(stdout , CLS_USAGE, basename)  ;
   puts("") ; 
} 

static void prevent_clearing_scrolback_buffer(const int  nlines) 
{
   putp(tp(cap(PIX) ,nlines)) ; 
   putp(tp(cap(CSR) ,0 ,nlines));  
}

static void clearing_gap(const int nlines)  
{
   putp(tp(cap(PIX),  nlines)) ; 
}

static void  clearing_back(const int nlines)  
{
  putp(tp(cap(PUC), nlines)) ;
  putp(tp(cap(PDL), nlines)) ; 
}

int main(int ac , char * const *av) 
{ 

  int pstatus=EXIT_SUCCESS ,
      erret = 0,
      nrows=0;   
  char *flags=00; 
  

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
     putp(cap(CLS));  
     goto _restore_shell ; 
  }

  flags  = *(av+1) ; 

  if ( !((*flags) ^'-'))  
  {
    switch( *(flags+1) & 0xff )  
    {
       case  'g': 
         nrows = lines; 
         if(ac >=3 )  
         {
           nrows = strtol(*(av+2) ,00, 10) ;  
           nrows = nrows?  nrows : lines;
         } 
         if(!(lines ^ nrows) ||  nrows >  lines)
           prevent_clearing_scrolback_buffer(nrows); 
         else 
           /* Placing gap between  previous output */
           clearing_gap(nrows);  
       
        break;
       case  'v':
        PRINT_VERSION() ;  
        break;
       case 'h': 
       default : 
        show_usage(av) ,PRINT_VERSION(); break ; 
    }
    
  }else{
    /*When a number was given as argument, a clearing back will be  performed  */ 
    int query_backlines= strtol(flags , (void *)00 ,  10 ) ; 
    if(!query_backlines) 
    {
       show_usage(av) ; 
       goto _restore_shell ;
    }
    clearing_back(query_backlines); 
  }


_restore_shell: 
  (void) reset_shell_mode ;

_eplg: 
  return pstatus ; 
}

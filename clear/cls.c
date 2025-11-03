// SPDX-License-Identifier : GPL-2.0+
/* 
 * Un clone de la commande 'clear' present sur la pluspart
 * des systemes d'exploitation unix avec  quelques option supplementaire  
 * 
 * Copyright (C) 2025  KerHack-Libre 
 * Author:   Umar BA <jUmarB@protonmail.com> 
 *
 */


#include <curses.h> 
#include <stdlib.h>
#include <stdio.h>
#include <term.h>
#include <unistd.h> 

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
#define CSR   3                 /* Change scroll region */
#define CLS   5                 /* Clear  screen        */
#define PDL   106               /* Parm delete line     */
#define PIX   109               /* Parm index           */ 
#define PUC   114               /* Parm up cursor       */ 



#define cap(capid) \
  *( ((TERMTYPE*)cur_term)->Strings+capid) 

#define tp(capstring , ...)  \
  tparm(capstring , __VA_ARGS__) 

#define PRINT_VERSION(...)  \
  printf("cls version %s by KerHack-Libre version \012", CLS_VERSION_STR ) 

#define CLS_USAGE \
  "Usage : %s [OPTION]...[NUMBER]\012\012"          \
  "   -h    \011Show this help\012"                 \
  "   -v    \011Current version\012"                \
  "   -r    \011Restore default shell behavior\012" \
  "   -g [n]\011Insert an empty gap between\012"    \
  "   -s [n]\011Create a sticky area\012"           \

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

static void insert_padding_gap(const int nlines)  
{
   putp(tp(cap(PIX),  nlines)) ; 
}

static void  clearing_back(const int nlines)  
{
  putp(tp(cap(PUC), nlines)) ;
  putp(tp(cap(PDL), nlines)) ; 
}

/* @fn  sticky_zone(int nline  ,  int orientation) */
static  int  sticky_zone(int nlines)  
{
  putp(tp(cap(CSR),  0 ,nlines)) ; 
  clearing_back(nlines) ; 
  putp(tp(cap(PDL), 0)) ; 
  return nlines ;  
}


static void restore_shell_default_behavior(void)  
{
  (void) reset_shell_mode; 
}

int main(int ac , char * const *av , char  * const * env)   
{ 

  int pstatus=EXIT_SUCCESS ,
      erret = 0,
      nrows=0,
      sz_spawn=0;  /*  sticky zone spawn */
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
       
    prevent_clearing_scrolback_buffer(lines);  
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
           insert_padding_gap(nrows);  
       
        break;
       case  's': 
        if(ac < 3 ) 
        {
           show_usage(av), PRINT_VERSION(); 
           break ; 
        }
        nrows = strtol(*(av+2), 00 , 10 ) ; 
        if(!nrows) 
          break ;  
        
        sz_spawn = sticky_zone(nrows) ; 
        break ;
       case  'r': 
        (void) reset_shell_mode; 
        break ; 
         
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
  restore_shell_default_behavior();  

_eplg: 
  return pstatus ; 
}

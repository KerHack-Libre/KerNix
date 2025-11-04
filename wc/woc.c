// SPDX-License-Identifier : GPL-2.0+ 
/*
 * Une alternative  a la commande wc disponible 
 * sur la pluspart  des Systemes Unix 
 *
 * Copyright (C) 2025, KerHack-Libre
 * Author:  Umar Ba <jUmarB@protonmail.com> 
 *
 * @TODO:  
 * [-/+] - add verbose mode : qui montre tous mots  a l'image de cat  
 *    *|> si il support  curses colorier les mots  
 *    [] utiliser les memory streams 
 *
 */ 

#include <errno.h>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <ctype.h>

#if SUPPORT_CURSES 
#include <term.h> 
#include <curses.h> 
#endif  

#define  CHARS  (1<<0) 
#define  WORDS  (1<<2) 
#define  LINES  (1<<3) 

#define  WOC_ENABLE_ALL_BY_DEFAULT (CHARS|WORDS|LINES) 

#define USAGE(basename)\
  "%s [OPTION]...[FILE]\012",basename 

#define  woc_err(...)  \
  EXIT_FAILURE;do {fprintf(stderr , __VA_ARGS__);}while(0)

#define woc_errloc  strerror(*__errno_location()) 

static unsigned int enable_verbose_mode = 0 ;   /* Print  and color  if possible */ 

typedef struct __woc_attr_t  woc_attr_t ; 
struct __woc_attr_t  { unsigned int _c ,_w, _l;}; 

static int word_count(struct __woc_attr_t * , const char *__restrict__  ,  int) ; 
static char * woc_scan_options(char *const * av , int* )  ;

int main(int ac ,char *const *av) 
{
  unsigned int pstatus = EXIT_SUCCESS , 
              woc_options=WOC_ENABLE_ALL_BY_DEFAULT ; 
  woc_attr_t optattr = {0}; 
  char *file_target =  (char *)00 ;
  
  setvbuf(stdout ,  (char*)00  , _IONBF , 0) ; 

  if(!(ac &~(1)))  
  {
     pstatus^=woc_err(USAGE(*(av))); 
     goto _eplg ; 
  } 
  file_target = woc_scan_options((av+1) ,  &woc_options);
  if(word_count(&optattr , file_target , woc_options)) 
  {
    pstatus^=woc_err("%s\012", woc_errloc) ; 
    goto _eplg ; 
  }

  if(woc_options & LINES) 
    fprintf(stdout, "%i ",optattr._l); 
  if(woc_options & CHARS)
    fprintf(stdout, "%i ",optattr._c); 
  if(woc_options & WORDS) 
    fprintf(stdout ,"%i ",optattr._w); 

  fprintf(stdout , "%s\012",file_target) ; 
  
_eplg: 
  return pstatus ; 
}

static char * woc_scan_options(char  *const *av , int * options) 
{
  char **argv =(char **) av,
       * pretended_file=(char *)00 ;  
  
  int  user_options = 0  ;   
  while(*argv) 
  { 
    char *arg = *argv;  
    if(!(*arg ^ '-')) 
    {  
       int nargs = 1 ;  
       while(*(arg+nargs) && *(arg+nargs)!= 0x20)
       {
         switch(*(arg+nargs) & 0xff) 
         {
           case 'c':user_options|=CHARS ; break; 
           case 'l':user_options|=LINES ; break; 
           case 'w':user_options|=WORDS ; break; 
           case 'v': 
                    enable_verbose_mode=1 ; 
                    break ; 
         } 
         nargs-=~0; 
       }
    }else 
      if(!pretended_file)
         pretended_file= *argv  ;    

   (void*) *argv++ ;  
  }
  
  if(0 < user_options) 
    *options^=(*options &~user_options); 

  return pretended_file ; 
}
static int word_count(woc_attr_t * optattr , const char *restrict file ,int options) 
{
  struct {
    int current , previous ; 
  }  woc = { EOF , 0 } ; 

  char inline_buffer[2048] ={0} ;
  int  buffer_cursor = 0 ;  
  FILE *fp = fopen(file , "r") ;
  if(!fp) 
  {
     woc_err("%s : %s\012", __func__, woc_errloc)  ;
     return  ~0 ;  
  }
  
  while(EOF != (woc.current = fgetc(fp))) 
  {
    if(options  &  CHARS) 
      optattr->_c=-~optattr->_c; 
      
    if(options & LINES)
      if(!((woc.current & 0xff) ^ 0xa))
      {
        optattr->_l-=~0;
        if(enable_verbose_mode) 
        {
          if (isspace(*inline_buffer))  
          { 
            fprintf(stdout , "%i",optattr->_l); 
            char *inline_buffer_no_space = inline_buffer +1 ;    
            printf("  %s\n",inline_buffer_no_space) ; 
          }else 
          {
            fprintf(stdout , "%i",optattr->_l); 
            printf(" %s\n",  inline_buffer ) ;  
          }

          buffer_cursor&=~buffer_cursor ;  
          bzero(inline_buffer , 2048) ; 
        }
      }

    if(options & WORDS) 
    { 
      if(!isspace(woc.previous)  &&  isspace(woc.current))  
        optattr->_w-=~0 ; 
      woc.previous = woc.current ;   
      if(enable_verbose_mode)  
      { 
        //TODO : utliser open_memstream  
         sprintf((inline_buffer+buffer_cursor) ,"%c", woc.current) ; 
         buffer_cursor-=~0 ; 
      }
       
    } 
    
  }

  fclose(fp) ;
  return 0; 
}

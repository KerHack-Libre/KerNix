// SPDX-License-Identifier : GPL-2.0+ 
/*
 * Une alternative  a la commande wc disponible 
 * sur la pluspart  des Systemes Unix 
 *
 * Copyright (C) 2025, KerHack-Libre
 * Author:  Umar Ba <jUmarB@protonmail.com> 
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

#define  CHARS      (1<<0) 
#define  WORDS      (1<<2) 
#define  LINES      (1<<3) 
#define  LINESTRIP  (1<<4)

#define  WOC_ENABLE_ALL_BY_DEFAULT (CHARS|WORDS|LINES|LINESTRIP) 

#define USAGE(basename)\
  "%s [OPTION]...[FILE]\012",basename 

#define  woc_err(...)  \
  EXIT_FAILURE;do {fprintf(stderr , __VA_ARGS__);}while(0)

#define woc_errloc  strerror(*__errno_location()) 

static unsigned int enable_verbose_mode = 0 ; 
static char * wordchars  = 0 ; 

typedef struct __io_memory_buffer_t  io_mem_buffer_t ; 
typedef struct __woc_attr_t  woc_attr_t ; 

struct __woc_attr_t  { unsigned int _c ,_w, _l , _ls;}; 
struct __io_memory_buffer_t{ 
  char *buff_ptr ; 
  FILE *memory_stream ;  
  size_t sizeloc_membuf; 
}; 



static int    woc_word_count(struct __woc_attr_t * , const char *__restrict__  ,  int) ; 
static char * woc_scan_options(char ** , int,  int* )  ;
static void   woc_verbosity(unsigned char  ,io_mem_buffer_t *__restrict__);  

int main(int ac ,char *const *av) 
{
  unsigned int pstatus = EXIT_SUCCESS , 
              woc_options=WOC_ENABLE_ALL_BY_DEFAULT ; 
  woc_attr_t optattr = {0}; 

  char *file_target =  (char *)00; 
  setvbuf(stdout ,  (char*)00  , _IONBF , 0) ; 

  if(!(ac &~(1)))  
  {
     pstatus^=woc_err(USAGE(*(av))); 
     goto _eplg ; 
  } 
  file_target = woc_scan_options((char **)(av+1) ,ac,  &woc_options); 

  if(woc_word_count(&optattr , file_target , woc_options)) 
  {
    pstatus^=woc_err("%s\012", woc_errloc) ; 
    goto _eplg ; 
  }

  /*TODO: 
   * [] create a dedicated function for  showing result
   * */
  if(woc_options & LINES) 
    fprintf(stdout, "%i ",optattr._l); 
  if(woc_options & CHARS)
    fprintf(stdout, "%i ",optattr._c); 
  if(woc_options & WORDS) 
    fprintf(stdout ,"%i ",optattr._w);
  
  /* empty lines */
  if(woc_options & LINESTRIP)
  {
    fprintf(stdout , "%i ", optattr._ls) ;  
    /*lines that contains something aka real lines  */ 
    fprintf(stdout ,"%i " ,abs(optattr._ls -  optattr._l)) ; 
  }

  


  fprintf(stdout , "%s\012",file_target) ; 
  
_eplg: 
  return pstatus ; 
}

static char * woc_scan_options(char **av, int ac ,int * options)
{
  char **argv =(char **) av,
       * pretended_file=(char *)00; 
  
  int  user_options = 0  ;   

  if( 3 <= ac) 
    wordchars = *(av+(ac-2)) ; 

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
           case 's':
                    /*The strip flags depend on  LINES flags to be enable */
                    user_options|=LINES ;  
                    user_options|=LINESTRIP;break; 
           case 'v': 
                    enable_verbose_mode=1;  
                    break ; 
         } 
         nargs-=~0; 
       }
    }else 
    {  
      if(!pretended_file)   
        pretended_file =  *argv ; 
       
    }
   (void*) *argv++ ;  
  }

  if(0 < user_options) 
    *options^=(*options &~user_options); 

  return pretended_file ; 
}


static int woc_word_count(woc_attr_t * optattr , const char *restrict file ,int options) 
{
  struct {
    int current , previous ; 
  }  woc = { EOF , 0 } ; 

  int  previous_line =0,

       woc_score     =0,
       idx           =0; 

  char word_collection[0x1000]={0}; 

  io_mem_buffer_t stream_membuf ; 
  if(enable_verbose_mode) 
  {
    stream_membuf.memory_stream=open_memstream(&stream_membuf.buff_ptr, &stream_membuf.sizeloc_membuf); 
    if(!stream_membuf.memory_stream)
      enable_verbose_mode=0 ;/* disable verbose mode  */ 
  } 

  FILE *fp = fopen(file , "r") ;
  if(!fp) 
  {
     woc_err("*%s : %s\012", __func__, woc_errloc)  ;
     return  ~0 ;  
  }
  
  while(EOF != (woc.current = fgetc(fp))) 
  {
    if(options  &  CHARS) 
      optattr->_c=-~optattr->_c; 
      
    if(options & LINES)
    {
      if(options & LINESTRIP) 
        if(!(0xa  ^(previous_line & 0xff)) && 
           !(0xa  ^(woc.current & 0xff))) 
          optattr->_ls-=~0; 
      
      if(!((woc.current & 0xff) ^ 0xa)) 
      {
        optattr->_l-=~0; 
        previous_line=woc.current; 
      }else 
         previous_line  = woc.current;  
      
    }

    if(options & WORDS) 
    {  
      if(wordchars  &&  1 == strlen(wordchars) ) 
         woc_score-= !(*wordchars ^ woc.current)?~0:0; 

      if(!isspace(woc.previous)  &&  isspace(woc.current)) 
      {
        optattr->_w-=~0 ; 
        if(wordchars && strlen(wordchars)>1) 
        {
          woc_score-=strstr(word_collection,wordchars)? ~0: 0 ;  
          bzero(word_collection , 0xff) , idx =0 ;  
        }
      } 
        
      if(strlen(wordchars)>1 && !isspace(woc.current)) 
        *(word_collection +idx)=woc.current,idx-=~0 ; 
    
      woc.previous = woc.current ;  
    }
    
    if(enable_verbose_mode) 
      woc_verbosity(woc.current , &stream_membuf) ;  
    
  }

  fclose(fp) ;
  
  if(enable_verbose_mode) 
  {
    fclose(stream_membuf.memory_stream);   
    fprintf(stdout , "%s"  , stream_membuf.buff_ptr) ; 
  } 

  if(wordchars) 
  {
    printf("score for %s  is %i \n", wordchars ,  woc_score)  ; 
  }
  return 0; 
}

void woc_verbosity(unsigned char byte_char , io_mem_buffer_t * membuf) 
{ 
  static unsigned int eol =0,
                      lines=1; 
  if(!eol) 
    fprintf(membuf->memory_stream , "%i\011",lines) ,
      lines-=~0,eol^=1 ; 

  if(!((0xa) ^ (byte_char & 0xff))  && eol > 0 ) 
    eol^=1 ;

  fprintf(membuf->memory_stream , "%c" , byte_char) ;  
  
}

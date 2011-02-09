////////////////////////////////////////////////////////////////////////
//                                                                    //
// converter.h                                                        //
//                                                                    //
// converts a string into substrings (those seperated by spaces)      //
//                                                                    //
// (c) 1995 D. Bolier, A. Eliens                                      //
//                                                                    //
////////////////////////////////////////////////////////////////////////

#ifndef __CONVERTER_H
#define __CONVERTER_H

#include <stdio.h>
#include <string.h>

enum {NOFARGS = 24};
 
class converter 
{
public:
  converter(const char *s) 
  {
    int i = 0;
    
    // initialize
    strcpy(buf,s);  // right size s
    char* b = buf;
  
    // skip spaces
    while (*b == ' ') 
      b++;
    char* p = b;

    // convert substrings seperated by spaces
    while ( *p != '\0' ) 
    {
      i++;
      while ( *p != '\0' && *p != ' ' ) 
        p++;
      if ( *p == '\0' ) 
        strcpy(_argvb[i],b);
      else if (p) 
      {
        *p = '\0';
        strcpy(_argvb[i],b);
        p++;
        b = p;
      }
    }
    _argc = i;
  }
 
  int argc() 
    { return _argc; }

  char* operator[](int i) 
    { return _argvb[i]; }
 
private :
  int _argc;
  char _argvb[NOFARGS][BUFSIZ];
  char buf[BUFSIZ]; 

};

#endif

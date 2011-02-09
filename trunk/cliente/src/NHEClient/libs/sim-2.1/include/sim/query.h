////////////////////////////////////////////////////////////////////////
//                                                                    //
// query.h                                                            //
//                                                                    //
// definition of the query class                                      //
//                                                                    //
// (c) 1995 D. Bolier, A. Eliens                                      //
//                                                                    //
////////////////////////////////////////////////////////////////////////

#ifndef __QUERY_H
#define __QUERY_H


#include <sim/screen.h>
class entryhandler;


class query : public screen
{
public :
  query(char* p,char* options = "");
  query(widget* w,char* p,char* options = "");
  virtual ~query();
  char* get();

private :
#ifdef HUSH
  entryhandler* eh;
#endif
  void create(char* options);
};

#endif

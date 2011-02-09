////////////////////////////////////////////////////////////////////////
//                                                                    //
// simulationclock.h                                                  //
//                                                                    //
// definition of the clock widget                                     //
//                                                                    //
// (c) 1995 D. Bolier, A. Eliens                                      //
//                                                                    //
////////////////////////////////////////////////////////////////////////

#ifndef __SIMULATIONCLOCK_H
#define __SIMULATIONCLOCK_H

#include <sim/screen.h>

class simulationclock : public screen
{
public :
  simulationclock(char* p,char* options = "");
  simulationclock(widget* w,char* p,char* options = "");
  virtual ~simulationclock();
  void setclock(double cl);

#ifdef HUSH
private :
  char* foreground;    // foreground options
  void create(char* options);
#endif
};

#endif

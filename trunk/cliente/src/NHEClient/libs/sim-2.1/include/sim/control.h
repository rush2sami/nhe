////////////////////////////////////////////////////////////////////////
//                                                                    //
// control.h                                                          //
//                                                                    //
// definition of the control widget                                   //
//                                                                    //
// (c) 1995 D. Bolier, A. Eliens                                      //
//                                                                    //
////////////////////////////////////////////////////////////////////////

#ifndef __CONTROL_H
#define __CONTROL_H


#include <sim/screen.h>

class buttonhandler;
class scalehandler;


class control : public screen
{
public :
  control(char* p,char* options = "");
  control(widget* w,char* p,char* options = "");
  virtual ~control();
  void factor(double fct);
  void reset();
#ifdef HUSH
  int check();
  double speed();

private :
  scalehandler* sh;    // handler for the scale widget
  buttonhandler* bh;   // handler for this button
  char* foreground;    // foreground color
  void create(char* options);
#endif
};

#endif

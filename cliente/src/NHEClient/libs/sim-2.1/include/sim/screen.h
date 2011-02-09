////////////////////////////////////////////////////////////////////////
//                                                                    //
// screen.h                                                           //
//                                                                    //
// implementation of the screen class, it is identical to the canvas  //
// except for the use of world coordinates.			      //
//                                                                    //
// (c) 1995 D. Bolier, A. Eliens                                      //
//                                                                    //
////////////////////////////////////////////////////////////////////////

#ifndef __SCREEN_H
#define __SCREEN_H

#include <sim/hush.h>


#ifdef HUSH
//AE class screen : public canvas
class screen : public widget
#else
class screen : public widget
#endif
{
public :
  screen(const char* p, const char* options = "");
  screen(const widget* w, const char* p, const char* options = "");
  virtual ~screen();
  int line(int x1,int y1,int x2,int y2,char* options = "");
  int line(char* linespec,char* options = "");
  int oval(int x1,int y1,int x2,int y2,char* options = "");
  int circle(int x,int y,int r,char* options = "");
  int polygon(char* linespec,char* options = "");
  int rectangle(int x1,int y1,int x2,int y2,char* options = "");
  int square(int x,int y,int r,char* options = "");
  int bitmap(int x,int y,char* bitmap,char* options = "");
  int window(int x,int y,widget* w,char* options = "");
  int text(int x,int y,char* txt,char* options = "");
  void move(int id,int x,int y);
  void move(char* tg,int x,int y);
  void del(int id);
  void del(char* tg);
  void tag(int id,char* tg);
  char* tags(int id);
  int overlapping(int x,int y);
  char* items(int x,int y);
  void postscript(char* fn);

#ifdef HUSH
protected :
  void configure(const char* options);   // to configure the screen after

private :
  canvas* c;
  double factor;    // world / screen coordinates
  char* font;
  char* parse(const char* options);
#endif
};

#endif

////////////////////////////////////////////////////////////////////////
//                                                                    //
// hush.h                                                             //
//                                                                    //
// to make the simulation programs compatible                         //
//                                                                    //
// (c) 1995 D. Bolier, A. Eliens                                      //
//                                                                    //
////////////////////////////////////////////////////////////////////////

#ifndef __HUSH_H
#define __HUSH_H

#ifdef HUSH  // HUSH definitions

#ifdef NOSIM
#undef NOSIM
#endif

#include <hush/hush.h>
#include <widgets/widgets.h>

#else // ASCII definitions, functionality = 0

#include <sim/defs.h>

class widget
{
public :
  widget(const char* p)
  {
    //p = p;
  }

  widget(const widget* w, const char* p)
  {
    //w = w; p = p;
  }

  widget* pack(const widget* w, const char* opt = "")
  {
    return (widget*) w;
  }

  widget* pack(const char* opt="") { return this; }

  void destroy()
    {}
};

class kit
{
public :
  kit()
    {}

  widget* pack(const widget* w, const char* options = "")
  {
    options = options;
    return (widget*) w;
  }

  void update()
    {}
};

inline kit* thekit() { return new kit; }


class toplevel : public widget
{
public :
  toplevel(const char* p, const char* options = "") : widget(p)
  {
    options = options;
  }

  toplevel(const widget* w, const char* p, const char* options = "") 
						: widget(w,p)
  {
    options = options;
  }
};


class session
{
public :
  session(int _argc,char** _argv,char* _name = NULL)
  {
    tk = new kit();
    name = _name;
  }

  virtual void prelude() { }

  virtual int main( ) { return 0; }    // used to have args: kit*, int, char**

  int run()
  {
    return main();
  }

protected :
  kit* tk;
  char* name;
};

#endif

#endif

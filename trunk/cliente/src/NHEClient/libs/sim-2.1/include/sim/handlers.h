////////////////////////////////////////////////////////////////////////
//                                                                    //
// handlers.h                                                         //
//                                                                    //
// definition of the diverse handlers                                 //
//                                                                    //
// (c) 1995 D. Bolier, A. Eliens                                      //
//                                                                    //
////////////////////////////////////////////////////////////////////////

#ifndef __HANDLERS_H
#define __HANDLERS_H

#ifdef HUSH

#include <hush/handler.h>

class buttonhandler : public handler
{
public :
  buttonhandler();
  virtual int operator()();
  int check()
    { return start; }
  void check(int s)
    { start = s; }

private :
  int start;
};

#else

class buttonhandler
{
public :
  buttonhandler();
};

#endif

#ifdef HUSH

#include <sim/control.h>

class quithandler : public handler
{
public :
  quithandler(control* co);
  virtual int operator()();

private :
  control* c;
};

#else

class quithandler
{
public :
  quithandler();
};

#endif

#ifdef HUSH

class exithandler : public handler
{
public :
  exithandler(control* co);
  virtual int operator()();

private :
  control* c;
};

#else

class exithandler
{
public :
  exithandler();
};

#endif

#ifdef HUSH

class entry;

class entryhandler : public handler
{
public :
  entryhandler(entry* en);
  virtual int operator()();
  char* get()
    { return input; }

private :
  entry* e;        // the entry widget
  char* input;     // the entry value
};

#else

class entryhandler
{
public :
  entryhandler();
};

#endif

#ifdef HUSH

class scale;

class scalehandler : public handler
{
public :
  scalehandler(scale* s,double f);
  virtual int operator()();
  double speed()
    { return sp; }
  void factor(double fct)
    { fc = fct; }

private :
  double sp;  // the speed
  double fc;  // the factor
  scale* sc;  // the scale widget
};

#else

class scalehandler
{
public :
  scalehandler();
};

#endif

#endif

////////////////////////////////////////////////////////////////////////
//                                                                    //
// handlers.c                                                         //
//                                                                    //
// implementation of the diverse handlers                             //
//                                                                    //
// (c) 1995 D. Bolier, A. Eliens                                      //
//                                                                    //
////////////////////////////////////////////////////////////////////////

#include <sim/handlers.h>

#ifdef HUSH

#include <sim/hush.h>
#include <sim/defs.h>


// constructor, initializes the start variabele.

buttonhandler::buttonhandler()
{
  start = OK;
}


// redefines the handler::operator()() function, it sets the start
// variabele to its opposite

int buttonhandler::operator()()
{
  if (start)
    start = FALSE;
  else 
    start = OK;
  return OK;
}

#else

buttonhandler::buttonhandler()
{
}

#endif

#ifdef HUSH

#include <sim/simulation.h>

quithandler::quithandler(control* co)
{
  c = co;
}


// redefines the handler::operator()() function, it invokes the simulation::
// quit function

int quithandler::operator()()
{
  // simulation pointer exists
  thesimulation() -> quit();
  return 0;
}

#else

quithandler::quithandler()
{
}

#endif

#ifdef HUSH

exithandler::exithandler(control* co)
{
  c = co;
}


// redefines the handler::operator()() function, it invokes kit::quit
// function

int exithandler::operator()()
{
  // simulation pointer exists
  thekit() -> quit();
  return 0;
}

#else

exithandler::exithandler()
{
}

#endif

#ifdef HUSH

// constructor, initializes the speed variabele, the factor and the
// scale widget

scalehandler::scalehandler(scale* s,double f)
{
  sc = s;
  sp = 0;
  fc = f;
}


// redefines the handler::operator()() function, it sets the fraction
// of the delay time. The simulation::selectnext function will sleep
// for this fraction times the interactivation time in seconds.

int scalehandler::operator()()
{
  sp = (double)(100 - sc -> get()) / 100 * fc;
  return OK;
}

#else

scalehandler::scalehandler()
{
}

#endif

#ifdef HUSH

#include <stdlib.h>

// constructor, initializes the entry value and widget

entryhandler::entryhandler(entry* en)
{
  e = en;
  input = NULL;
}


// redefines the handler::operator()() function, it sets the entry
// value

int entryhandler::operator()()
{
  input = e -> get();
  return OK;
}

#else

entryhandler:: entryhandler()
{
}

#endif

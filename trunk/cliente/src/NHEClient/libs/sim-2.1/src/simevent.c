////////////////////////////////////////////////////////////////////////
//                                                                    //
// simevent.c                                                         //
//                                                                    //
// implementation of the simulation event                             //
//                                                                    //
// (c) 1995 D. Bolier, A. Eliens                                      //
//                                                                    //
////////////////////////////////////////////////////////////////////////


#include <stdlib.h>
#include <string.h>
#include <sim/defs.h>
#include <sim/error.h>
#include <sim/event.h>
#include <sim/reports.h>
#include <sim/simulation.h>



// constructor taking the queuing and scheduling priority. It
// initializes the state to PASSIVE.

#ifdef HUSH
simevent::simevent(int qp,int sp,int k)  : event(this)
#else
event::event(int qp,int sp,int k)
#endif
{
  lp = NULL;
  mp = NULL;
  rp = NULL;
  pp = NULL;
  kf = k;
  tm = 0.0;
  queuingprior = qp;
  schedulingprior = sp;
  st = PASSIVE;
  intv = 0.0;
  tp = NULL;
  erpt = 0;
  wrpt = 0;

  // created an event, so
  simulation* sim = thesimulation();
  if (sim)
    sim -> created();           // no error, session is derived from event
}
  

// dereference pointers

#ifdef HUSH
simevent::~simevent()
#else
event::~event()
#endif
{
  lp = NULL;
  mp = NULL;
  rp = NULL;
  pp = NULL;
  tp = NULL;
}


// generate a report on the time an event has been in the simulation

#ifdef HUSH
int simevent::report(histogram* hi,double interv)
#else
int event::report(histogram* hi,double interv)
#endif
{
  // histogram should exist and be of type FREQUENCY
  if (!hi)
    err(NOHISTO);
  if (hi -> histogramkind() != FREQUENCY)
  {
    warn(HFREQUENCY);
    hi -> histogramkind(FREQUENCY);
  }
  tp = hi -> histogramtitle();

  // set time the report member is invoked
  simulation* sim = thesimulation();
  if (!sim)
    err(INITERR);
  erpt = sim -> clock();

  // if a report of given type (the histogram title) already exists
  // do nothing
  if (!(sim -> findeventreport(tp)))
  {
    // add the report
    eventreport* e = new eventreport(hi);
    if (!(sim -> addeventreport(e)))
      return FALSE;

    // if specified print the report each 'interv' time units
    if (interv)
    {
      coutreport* crep = new coutreport(hi,interv);
      sim -> schedule(crep,interv);
    }
  }
  return OK;
}
   

// add a time stamp (for the timespent member)

#ifdef HUSH
void simevent::stamp()
#else
void event::stamp()
#endif
{
  simulation* sim = thesimulation();
  if (!sim)
    err(INITERR);

  intv = sim -> clock();
}


// return the time since the event received a time stamp 

#ifdef HUSH
double simevent::timespent()
#else
double event::timespent()
#endif
{
  simulation* sim = thesimulation();
  if (!sim)
    err(INITERR);
  return (sim -> clock() - intv);
}

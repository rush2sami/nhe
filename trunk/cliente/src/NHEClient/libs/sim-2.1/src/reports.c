////////////////////////////////////////////////////////////////////////
//                                                                    //
// reports.c                                                          //
//                                                                    //
// implementation of the report handling functions                    //
//                                                                    //
// (c) 1995 D. Bolier, A. Eliens                                      //
//                                                                    //
////////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <sim/event.h>
#include <sim/defs.h>
#include <sim/error.h>
#include <sim/simulation.h>
#include <sim/histogram.h>
#include <sim/reports.h>


////////////////////////////////////////////////////////////////////////
//                                                                    //
// eventreport                                                        //
//                                                                    //
// generates a report on the lifetime of an event                     //
//                                                                    //
////////////////////////////////////////////////////////////////////////


// constructor, sets the histogram and the type that is equal to the
// title of the histogram

eventreport::eventreport(histogram* hi)
{
  tp = hi -> histogramtitle();
  h = hi;
}


// destructor dereferences the pointers

eventreport::~eventreport()
{
  h = NULL;
  tp = NULL;
}


// function operator, makes a sample of the time the event e has been
// in the simulation.

int eventreport::operator()(event* e)
{
  simulation* sim = thesimulation();
  if (!sim)
    err(INITERR);
  double tm = sim -> clock() - e -> eventreporttime();
  h -> sample(tm);
  return OK;
}


// overloaded operator<< function, makes it possible to write the 
// histogram to standard output

ostream& operator<<(ostream& os,eventreport& e)
{
  histogram* hi = e.h;

  os << (*hi);
  return os;
}


////////////////////////////////////////////////////////////////////////
//                                                                    //
// behaviorreport                                                     //
//                                                                    //
// implementation of the behaviorreport class, it maintains a SERIES  //
// histogram that is filled with the current moving average of the    //
// histogram to be analyzed. The moving average is adjusted by        //
// the function operator.                                             //
//                                                                    //
////////////////////////////////////////////////////////////////////////


// constructor, taking the histogram to be analyzed, the SERIES 
// histogram to be filled with the values of the moving average and the
// size of the buffer

behaviorreport::behaviorreport(histogram* a,histogram* hi,int size)
{
  if ( (!hi) || (!a) )
    err(NOHISTO);
  if (hi -> histogramkind() != SERIES)
  {
    warn(HSERIES);
    hi -> histogramkind(SERIES);
  }
  h = hi;
  ah = a;
  buffersize = size;
  buffer = new double[buffersize];
  for (int i=0;i<buffersize;i++)
    buffer[i] = 0.0;
  movingaverage = 0.0;
  index = 0;
}


// dereference pointers and delete the ring buffer

behaviorreport::~behaviorreport()
{
  h = NULL;
  ah = NULL;
  delete buffer;
}


// adjust and add a sample of the moving average to the ring buffer.

int behaviorreport::operator()(double a)
{
  // determine index, current index modulo the buffer size
  int i = index % buffersize;

  // adjust moving average and the buffer and add the sample
  movingaverage -= buffer[i];
  buffer[i] = a / buffersize;
  movingaverage += buffer[i];
  index++;
  h -> sample(1,movingaverage);
  return OK;
}
  

// overloaded operator<<, put the SERIES histogram in the output stream

ostream& operator<<(ostream& os,behaviorreport& behavior)
{
  histogram* hi = behavior.h;

  os << (*hi);
  return os;
}


////////////////////////////////////////////////////////////////////////
//                                                                    //
// coutreport                                                         //
//                                                                    //
// takes a histogram and prints this histogram each 'i' time units    //
//                                                                    //
////////////////////////////////////////////////////////////////////////


// constructor, taking the histogram and the interval. It is derived
// from the class event, so it can be activated

coutreport::coutreport(histogram* hi,double i) : event()
{
  h = hi;
  intval = i;
}


// destructor dereferences the histogram pointer

coutreport::~coutreport()
{
  h = NULL;
}


// function operator, prints the histogram and waits for the interval
// to be activated again

int coutreport::operator()()
{
  simulation* sim = thesimulation();    // sim exists
  cout << (*h);
  sim -> wait(intval);
  return OK;
}

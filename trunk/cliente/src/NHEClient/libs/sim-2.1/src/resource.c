////////////////////////////////////////////////////////////////////////
//                                                                    //
// resource.c                                                         //
//                                                                    //
// implementation of the resource. The class provides members for     //
// maintaining a resource                                             //
//                                                                    //
// (c) 1995 D. Bolier, A. Eliens                                      //
//                                                                    //
////////////////////////////////////////////////////////////////////////


#include <iostream.h>
#include <stdio.h>
#include <string.h>
#include <sim/defs.h>
#include <sim/simulation.h>
#include <sim/resource.h>
#include <sim/histogram.h>
#include <sim/reports.h>
#include <sim/error.h>


// constructor, setting the initial and current amount to the given 
// parameter

resource::resource(double am)
{
  amount = initial = am;
  h = NULL;
}


// generate a report and print this report each intv time units

int resource::report(histogram* hi,double intv)
{
  // histogram should exist and be of type WEIGHTED
  if (!hi)
    err(NOHISTO);
  if (hi -> histogramkind() != WEIGHTED)
  {
    warn(HWEIGHTED);
    hi -> histogramkind(WEIGHTED);
  }
  h = hi;

  // add the report to the simulation object
  simulation* sim = thesimulation();
  if (!sim)
    err(INITERR);
  if (!(sim -> addresourcehistogram(hi)))
    return FALSE;

  // if specified print it each intv time units
  if (intv)
  {
    coutreport* crep = new coutreport(hi,intv);
    if (!crep)
      return FALSE;
    sim -> schedule(crep,intv);
  }
  return OK;
}


// reset the current amount to the initial and reset the report, if it 
// exists

void resource::reset()
{
  amount = initial;
  if (h)
    h -> reset();
}


// acquire the given amount

void resource::acquire(double am)
{
  if (h)
    h -> sample(occupied());
  if ((amount-am) < 0.0)
    warn(RESOURCE);
  else
    amount -= am;
}


// release the given amount

void resource::release(double am)
{
  if (h)
    h -> sample(occupied());
  if ((amount+am) > initial)
    warn(RESOURCE);
  else
    amount += am;
}


// put the initial and current amount, the report, if it exists, and
// the occupation of the resource in the overloaded operator<<

ostream& operator<<(ostream& os,resource& r)
{
  char* buf = new char[BUFSIZ];
  os << "-------------------------------------------------------------------" << endl;
  os << "DUMPING RESOURCE :" << endl;
  sprintf(buf,"%f",r.initial);
  os << "initial amount : " << buf << endl;
  sprintf(buf,"%f",r.amount);
  os << "amount left : " << buf << endl;
  sprintf(buf,"%f",r.occupied());
  os << "occupied : " << buf << "%" << endl;
  delete buf;
  return os;
}

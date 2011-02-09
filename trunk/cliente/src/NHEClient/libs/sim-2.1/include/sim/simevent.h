////////////////////////////////////////////////////////////////////////
//                                                                    //
// simevent.h                                                         //
//                                                                    //
// header for the simulation event class			      //
//                                                                    //
// (c) 1995 D. Bolier, A. Eliens                                      //
//                                                                    //
////////////////////////////////////////////////////////////////////////


#ifndef __SIMEVENT_H
#define __SIMEVENT_H

#include <sim/defs.h>

class histogram;

// define the different states an event can be in
enum {CONDITIONAL,ACTIVE,PASSIVE,PENDING,QUEUED,TERMINATED,CLOSED};


#ifdef HUSH
class simevent : public event 
#else
class event
#endif
{
#ifdef HUSH
public :
  simevent(int =0,int =0,int =0);
  virtual ~simevent();
#else
  // constructor, it is protected so it can only be used by a sub event
protected :

  event(int qp=0,int sp=0,int k=0);

public :
  virtual ~event();
  virtual int verify() const
    { return OK; }
  virtual int operator()()
    { return FALSE; }
#endif
  int report(histogram* hi,double interv = 0);
  void stamp();
  double timespent();

  // various inline members to manipulate the data of an event
  char* reporttitle()
    { return tp; }
  double time() const
    { return tm; }
  void time(double t)
    { tm = t; }
  int schedulingpriority()
    { return schedulingprior; }
  void schedulingpriority(int p)
    { schedulingprior = p; }
  int queuingpriority()
    { return queuingprior; }
  void queuingpriority(int p)
    { queuingprior = p; }
  double interval()
    { return intv; }
  void interval(double i)
    { intv = i; }
  int state()
    { return st; }
  void state(int s)
    { st = s; }
  int killflag()
    { return kf; }
  event* leftpointer()
    { return lp; }
  void leftpointer(event* l)
    { lp = l; }
  event* middlepointer()
    { return mp; }
  void middlepointer(event* m)
    { mp = m; }
  event* rightpointer()
    { return rp; }
  void rightpointer(event* r)
    { rp = r; }
  event* parentpointer()
    { return pp; }
  void parentpointer(event* p)
    { pp = p; }
  double waitingreporttime()
    { return wrpt; }
  void waitingreporttime(double w)
    { wrpt = w; }
  double eventreporttime()
    { return erpt; }
  int active()
    { return (st == ACTIVE); }
  int pending()
    { return (st == PENDING); }
  int conditional()
    { return (st == CONDITIONAL); }
  int closed()
    { return (st == CLOSED); }
  int passive()
    { return (st == PASSIVE); }
  int queued()
    { return (st == QUEUED); }

protected :
  int schedulingprior;    // scheduling priority
  int queuingprior;       // queuing priority
  char* tp;               // report title
  double tm;              // time, it will be activated
  double intv;            // time of the time stamp
  int st;                 // state it is in
  int kf;                 // kill-flag, if set to OK, we decrease the 
                          // termination count
  double wrpt;            // time stamp for report on waiting time
  double erpt;            // time stamp for report on total time

  // event will be in a tertiary tree, so we define :
  event* lp;              // leftpointer;
  event* mp;              // middlepointer
  event* rp;              // rightpointer
  event* pp;              // parentpointer

};


#endif

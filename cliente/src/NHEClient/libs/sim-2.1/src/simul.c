////////////////////////////////////////////////////////////////////////
//                                                                    //
// simulation.c                                                       //
//                                                                    //
// the simulation class, contains the main simulation routine, that   //
// is responsible for the activation of the pending and conditional   //
// events, and the methods for manipulating the simulation environment//
//                                                                    //
// (c) 1995 D. Bolier, A. Eliens                                      //
//                                                                    //
////////////////////////////////////////////////////////////////////////


// GNU forgot so
//extern "C" int gettimeofday(...);

#include <stdio.h>
#include <iostream.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>

#ifdef BORLAND
#define TIMEZONE __timezone
#else
#define TIMEZONE timezone
#endif

#ifdef HUSH

#include <sim/hush.h>

extern kit* thekit();

enum {SCREENWIDTH = 400,TEXTHEIGHT = 20};

//AE
extern void set_simevent_fun(void*);
#endif


#include <sim/control.h>
#include <sim/clock.h>
#include <sim/event.h>
#include <sim/defs.h>
#include <sim/simulation.h>
#include <sim/error.h>
#include <sim/scheduler.h>
#include <sim/conditionallist.h>
#include <sim/tertiarytree.h>
#include <sim/reports.h>
#include <sim/histogram.h>
#include <sim/converter.h>

#ifdef HUSH
static event* sim_event(int q, int s, int p) {
	return new simevent(q,s,p);
	}
#endif

static simulation* sim = 0;   // the simulation object

// invoked in other classes of the library

extern simulation* thesimulation()
{
  return sim;
}
  

// constructor, allocate and initialize the various data members

simulation::simulation()
{
  sim = this;
  simclock = 0.0;
  running = FALSE;
  act = FALSE;
  allflag = OK;
  reportsflag = OK;
  statisticsflag = OK;
  tim = 0.0;
  termcount = 0;
  countset = FALSE;
  countreached = FALSE;
  numbercreated = 0;
  numberterminated = 0;
  numberactivated = 0;
  resourceindex = 0;
  queueindex = 0;
  waitingindex = 0;
  waitingallindex = 0;
  eventindex = 0;
  behaviorindex = 0;
  _speed = 0; // 0 = plain mode
  gettimeofday(&inittime,NULL);
  for (int i=0;i<MAXREPORT;i++)
  {
    resources[i] = NULL;
    queues[i] = NULL;
    waitings[i] = NULL;
    waitingsall[i] = NULL;
  }
  events = new eventreportpointer[MAXREPORT];
  behaviors = new behaviorreportpointer[MAXREPORT];
  sched = new scheduler();
  condlist = new conditionallist();
  curr = NULL;
#ifdef HUSH
  scr = NULL;
  cnt = NULL;
  ck = NULL;
  textoptions = new char[BUFSIZ];
  sprintf(textoptions,"-fill black");
  //AE
  set_simevent_fun( sim_event );
#endif
}
  

// destructor, dereference histograms and deallocate memory

simulation::~simulation()
{
  delete sched;
  delete condlist;
  curr = NULL;
  for (int i=0;i<MAXREPORT;i++)
  {
    resources[i] = NULL;
    queues[i] = NULL;
    waitings[i] = NULL;
    waitingsall[i] = NULL;
  }
  if (events)
//    delete events;  // NO      <----------
    delete[] events;  // YES     <----------
  if (behaviors)
//    delete behaviors;  // NO      <----------
    delete[] behaviors;  // YES     <----------
#ifdef HUSH
  if (scr)
    delete scr;
  cnt = NULL;
  ck = NULL;
  delete textoptions;
#endif
}


// format screen

screen* simulation::format(char* p,char* options)
{
#ifdef HUSH
  scr = new screen(p,parse(options));
  return scr;
#else
  return NULL;
#endif
}
  
screen* simulation::format(widget* w,char* p,char* options)
{
#ifdef HUSH
  scr = new screen(w,p,parse(options));
  return scr;
#else
  return NULL;
#endif
}


#ifdef HUSH

// sets the realtime mode and speed, the scan flag
// and the screen options

char* simulation::parse(char* options)
{
  int bg,fg,wd,hg;

  // standard options
  fg = bg = 0;
  wd = SCREENWIDTH;
  hg = 6*TEXTHEIGHT;

  // parse options
  converter* cnv = new converter(options);
  for (int i=1; i <= cnv -> argc(); i++)
  {
    if ((!strcmp((*cnv)[i],"-foreground")) && (i < cnv -> argc()))
      fg = i+1; 
    else if ((!strcmp((*cnv)[i],"-background")) && (i < cnv -> argc()))
      bg = i+1; 
    else if ((!strcmp((*cnv)[i],"-width")) && (i < cnv -> argc()))
      wd = atoi((*cnv)[i+1]); 
    else if ((!strcmp((*cnv)[i],"-height")) && (i < cnv -> argc()))
      hg = atoi((*cnv)[i+1]); 
  }

  // format
  if (fg)
    sprintf(textoptions,"-fill %s",(*cnv)[fg]);
 
  char* screenoptions = new char[BUFSIZ];
  if (bg)
    sprintf(screenoptions,"-width %d -height %d -worldx %d -worldy %d -background %s",wd,hg,SCREENWIDTH,6*TEXTHEIGHT,(*cnv)[bg]);
  else
    sprintf(screenoptions,"-width %d -height %d -worldx %d -worldy %d -background white",wd,hg,SCREENWIDTH,6*TEXTHEIGHT);
  delete cnv;
  return screenoptions;
}

#endif


// initialize

void simulation::init(char* options)
{
  // parse options
  converter* cnv = new converter(options);
  for (int i=1; i <= cnv -> argc(); i++)
  {
    if ((!strcmp((*cnv)[i],"-realtime")) && (i < cnv -> argc()))
      _speed = atof((*cnv)[i+1]);
    else if ((!strcmp((*cnv)[i],"-scan")) && (i < cnv -> argc()))
      if (!strcmp((*cnv)[i+1],"yes"))
        allflag = FALSE;
      else if (!strcmp((*cnv)[i+1],"no"))
        allflag = OK;
      else
        err(SCAN);
    else if ((!strcmp((*cnv)[i],"-suppress")) && (i < cnv -> argc()))
      if (!strcmp((*cnv)[i+1],"none"))
      {
        reportsflag = OK;
        statisticsflag = OK;
      }
      else if (!strcmp((*cnv)[i+1],"reports"))
      {
        reportsflag = FALSE;
        statisticsflag = OK;
      }
      else if (!strcmp((*cnv)[i+1],"statistics"))
      {
        reportsflag = OK;
        statisticsflag = FALSE;
      }
      else if (!strcmp((*cnv)[i+1],"both"))
      {
        reportsflag = FALSE;
        statisticsflag = FALSE;
      }
      else
        err(SUPPRESS);
  }
  delete cnv;
}


// defaults for the initialisation

void simulation::suppress()
{
  reportsflag = statisticsflag = FALSE;
}

void simulation::scan()
{
  allflag = OK;
}

void simulation::realtime(double sp)
{
  _speed = sp;
}


// returns real time elapsed

double simulation::realtime() 
{
  // set actual starting time
  struct timeval tv;
  struct TIMEZONE tz;
  gettimeofday(&tv,&tz);
  long start = tv.tv_sec;
  long delta_sec = tv.tv_sec - inittime.tv_sec;
  long delta_usec = tv.tv_usec - inittime.tv_usec;
  double curacttime =  (double) delta_sec + 
    (double) delta_usec / (double) 1000000L;
  return curacttime;
}
 

// run until no events left

void simulation::run()
{
  // running flag
  running = OK;

  // set actual starting time
  struct timeval tv;
  struct TIMEZONE tz;
  gettimeofday(&tv,&tz);
  long start = tv.tv_sec;
  double st = tim;

  // main routine, run until no events left or until quit is called
  if (_speed)  // realtime
  {
    double curacttime = realtime();
    while ( (!(sched -> empty())) && (running) && ((1.0 / _speed) * sched -> giveroot() -> time() <= curacttime ))
    {
#ifdef HUSH
      // check the start/stop button
      if ( (!cnt) || cnt -> check() )
      {
#endif
        selectnext();
        execute();
        scanconditional();
#ifdef HUSH
      }
      else if (cnt)
        cnt -> check();
#endif
    }
  }
  else
  {
    while ( (!(sched -> empty())) && (running) )
    {
#ifdef HUSH
      // check the start/stop button
      if ( (!cnt) || cnt -> check() )
      {
#endif
        selectnext();
        execute();
        scanconditional();
#ifdef HUSH
      }
      else if (cnt)
        cnt -> check();
#endif
    }
  }

  gettimeofday(&tv,&tz);
  long end = tv.tv_sec;

  // how did we stop ?
  if (sched -> empty())
    reports(NOLEFT,end-start,st);
  else if (!running)
    reports(QUIT,end-start,st);
  else
    reports(REALTIME,end-start,st);

  // may be a next run, so
  numbercreated = 0;
  numberterminated = 0;
  numberactivated = 0;
}


// run the simulation for t time units

void simulation::run(double t)
{
  // we'd like to run it for positive values
  if (t < 0.0)
    err(NDURATION);
  
  // set duration, running flag
  dur = t;
  t = t + simclock;
  running = OK;

  // set actual starting time
  struct timeval tv;
  struct TIMEZONE tz;
  gettimeofday(&tv,&tz);
  long start = tv.tv_sec;
  double st = tim;

  // main routine, run for t time units or until no events left or 
  // until quit is called
  if (_speed)  // realtime
  {
    double curacttime = realtime();
    while ( (tim < t) && (!(sched -> empty())) && (running) && ((1.0 / _speed) * sched -> giveroot() -> time() <= curacttime ))
    {
#ifdef HUSH
      // check the start/stop button
      if ( (!cnt) || cnt -> check() )
      {
#endif
        selectnext();
        execute();
        scanconditional();
#ifdef HUSH
      }
      else if (cnt)
        cnt -> check();
#endif
    }
  }
  else
  {
    while ( (tim < t) && (!(sched -> empty())) && (running) )
    {
#ifdef HUSH
      // check the start/stop button
      if ( (!cnt) || cnt -> check() )
      {
#endif
        selectnext();
        execute();
        scanconditional();
#ifdef HUSH
      }
      else if (cnt)
        cnt -> check();
#endif
    }
  }

  gettimeofday(&tv,&tz);
  long end = tv.tv_sec;

  // how did we stop ?
  if (tim >= t)
    reports(END,end-start,st);
  else if (sched -> empty())
    reports(NOLEFT,end-start,st);
  else if (!running)
    reports(QUIT,end-start,st);
  else
    reports(REALTIME,end-start,st);

  // may be a next run, so
  numbercreated = 0;
  numberterminated = 0;
  numberactivated = 0;
}


// run the simulation for count terminated events.

void simulation::run(int count)
{
  // we'd like to run it for positive values
  if (count < 0) 
    err(NCOUNT);
  
  countset = OK;
  termcount = count;
  countreached = FALSE;

  // running flag and statistics on numbers
  running = OK;

  // set actual starting time
  struct timeval tv;
  struct TIMEZONE tz;
  gettimeofday(&tv,&tz);
  long start = tv.tv_sec;
  double st = tim;

  // main routine, run for count teminated events or until no events 
  // left or until quit is called
  if (_speed)  // realtime
  {
    double curacttime = realtime();
    while ( (!countreached) && (!(sched -> empty())) && (running) && ((1.0 / _speed) * sched -> giveroot() -> time() <= curacttime ))
    {
#ifdef HUSH
      // check the start/stop button
      if ( (!cnt) || cnt -> check() )
      {
#endif
        selectnext();
        execute();
        scanconditional();
#ifdef HUSH
      }
      else if (cnt)
        cnt -> check();
#endif
    }
  }
  else
  {
    while ( (!countreached) && (!(sched -> empty())) && (running) )
    {
#ifdef HUSH
      // check the start/stop button
      if ( (!cnt) || cnt -> check() )
      {
#endif
        selectnext();
        execute();
        scanconditional();
#ifdef HUSH
      }
      else if (cnt)
        cnt -> check();
#endif
    }
  }

  // how did we stop ?
  gettimeofday(&tv,&tz);
  long end = tv.tv_sec;

  if (sched -> empty())
    reports(NOLEFT,end-start,st);
  else if (countreached)
    reports(COUNT,end-start,st);
  else if (!running)
    reports(QUIT,end-start,st);
  else
    reports(REALTIME,end-start,st);

  // may be a next run, so
  numbercreated = 0;
  numberterminated = 0;
  numberactivated = 0;
}


// print the reports and a screen with statistics (if HUSH/tk are defined)
// depending on the flags

void simulation::reports(stoptype type,long tm,double start)
{
if (statisticsflag) // suppressed ?
{
#ifndef HUSH
  start = start;  
#endif

  // statistics of actual time taken, number terminated, etc..
  char* buf = new char[BUFSIZ];
#ifdef HUSH
  if (scr)
    scr -> del("simulation");
  else
  {
    toplevel* tl = new toplevel(".simulation");
    scr = new screen(tl,".screen","-background white");
    scr -> geometry(SCREENWIDTH,6*TEXTHEIGHT);
    thekit() -> pack(scr);
  }
#endif

  sprintf(buf,"Simulation started at : %f",start);

#ifdef HUSH
  int id = scr -> text(SCREENWIDTH/2,TEXTHEIGHT/2,buf,textoptions);
  scr -> tag(id,"simulation");
#else
  cout << endl << buf << endl;
#endif

  switch (type)
  {
    case NOLEFT :
      sprintf(buf,"Simulation stopped - no scheduled events left at : %f",tim);
      break;
    case QUIT :
      sprintf(buf,"Simulation stopped - quit called at : %f",tim);
      break;
    case END :
      sprintf(buf,"Simulation stopped - end time reached at : %f",tim);
      break;
    case COUNT :
      sprintf(buf,"Simulation stopped - zero termination count at : %f",tim);
      break;
    case REALTIME :
      sprintf(buf,"Simulation stopped - simulation time > actual time %f",tim);
      break;
  }

#ifdef HUSH
  id = scr -> text(SCREENWIDTH/2,3*TEXTHEIGHT/2,buf,textoptions);
  scr -> tag(id,"simulation");
#else
  cout << buf << endl;
#endif

  sprintf(buf,"Created - %d events",numbercreated);

#ifdef HUSH
  id = scr -> text(SCREENWIDTH/2,5*TEXTHEIGHT/2,buf,textoptions);
  scr -> tag(id,"simulation");
#else
  cout << endl << buf << endl;
#endif

  sprintf(buf,"Activated - %d events",numberactivated);

#ifdef HUSH
  id = scr -> text(SCREENWIDTH/2,7*TEXTHEIGHT/2,buf,textoptions);
  scr -> tag(id,"simulation");
#else
  cout << buf << endl;
#endif

  sprintf(buf,"Terminated - %d events",numberterminated);

#ifdef HUSH
  id = scr -> text(SCREENWIDTH/2,9*TEXTHEIGHT/2,buf,textoptions);
  scr -> tag(id,"simulation");
#else
  cout << buf << endl;
#endif

  sprintf(buf,"Actual time taken - %d seconds",tm);

#ifdef HUSH
  id = scr -> text(SCREENWIDTH/2,11*TEXTHEIGHT/2,buf,textoptions);
  scr -> tag(id,"simulation");
#else
  cout << endl << buf << endl;
#endif

//  delete buf;  // NO      <----------
  delete[] buf;  // YES     <----------

}  // end suppressed ?

  // print reports

  if (reportsflag)   // suppressed ?
  {
    int i = 0;
    while (i < resourceindex)
    {
      cout << (*resources[i]);
      i++;
    }
    i = 0;
    while (i < queueindex)
    {
      cout << (*queues[i]);
      i++;
    }
    i = 0;
    while (i < waitingindex)
    {
      cout << (*waitings[i]);
      i++;
    }
    i = 0;
    while (i < waitingallindex)
    {
      cout << (*waitingsall[i]);
      i++;
    }
    i = 0;
    while (i < eventindex)
    {
      cout << (*events[i]);
      i++;
    }
    i = 0;
    while (i < behaviorindex)
    {
      cout << (*behaviors[i]);
      i++;
    }
  }  // end suppressed ?
  
#ifdef HUSH
  thekit() -> update();
#endif
}


// quit the simulation, just set the flag

void simulation::quit()
{
  running = FALSE;
}


// terminate the given event, it should be PASSIVE or ACTIVE

int simulation::terminate(event* e)
{
  // e should exist and should be PASSIVE or ACTIVE
  if (!e)
    err(NULLEV);
  if ( (e -> active()) || (e -> passive()) )
  {
    if (e == curr)
    {
      act = FALSE;
      curr = NULL;
    }

    // if kill-flag decrease count and possibly quit
    if (e -> killflag())
      decreasecount();

    // if it has a report on the time spent, add a sample
    eventreport* erep = findeventreport(e -> reporttitle());
    if (erep)
      (*erep)(e);
    delete e;
    numberterminated++;

    return OK;
  }
  else
    warn(TERMINATE);
  return FALSE;
}
  

// schedule an event e to be activated over t time units

int simulation::schedule(event* e,double t)
{
  // e should exist and should be PASSIVE
  if (!e)
    err(NULLEV);
  if (e -> passive()) 
  {
    if (curr == e)
      act = FALSE;
    return (sched -> insert(t,e));
  }
  else
    warn(SCHEDULE);
  return FALSE;
}


// schedule the current to be activated over t time units

int simulation::wait(double t)
{
  // the current should exist and should still be ACTIVE
  if (!curr)
    err(NULLEV);
  if (curr -> active())
  {
    act = FALSE;
    return (sched -> insert(t,curr));
  }
  else
    warn(WAIT);
  return FALSE;
}
    

// passivate the given event, extract it from the conditional list or
// pending tree or just make it PASSIVE

int simulation::passivate(event* e)
{
  // e should exist
  if (!e)
    err(NULLEV);
  // remove it from the conditional list
  if (e -> conditional() || e -> closed())
    {
    condlist -> remove(e);
    e -> state(PASSIVE);
    if (curr == e)
      act = FALSE;

    return OK;
  }
  // remove it from the pending tree
  else if (e -> pending()) 
  {
    sched -> extract(e);
    e -> state(PASSIVE);
    return OK;
  }
  // just make it passive
  else if (e -> active()) 
  {
    e -> state(PASSIVE);
    act = FALSE;
    return OK;
  }

  else
    warn(PASSIVATE);
  return FALSE;
}


// activate the given event, extract it from the conditional list or
// the pending tree and schedule it to be activated immediately

int simulation::activate(event* e)
{
  // e should exist
  if (!e)
    err(NULLEV);
  // on conditional list ?
  if (e -> conditional() || e -> closed()) 
  {
    condlist -> remove(e);
    if (curr == e)
      curr -> state(ACTIVE);
    sched -> insert(0.0,e);
    return OK;
  }
  // just passive ?
  else if (e -> passive()) 
  {
    sched -> insert(0.0,e);
    return OK;
  }
  // on the pending tree and time to be activated OK?
  else if ((e -> pending()) && (e -> time() > simclock)) 
  {
    sched -> extract(e);
    sched -> insert(0.0,e);
    return OK;
  }
  else
    warn(ACTIVATE);
  return FALSE;
}


// add the event to the conditional list

int simulation::hold(event* e)
{
  // e should exist and should be PASSIVE or ACTIVE
  if (!e)
    err(NULLEV);
  if (e -> passive()) 
  {
    condlist -> add(e);
    return OK;
  }
  else if ( e -> active())
  {
    condlist -> add(e);
    act = FALSE;
    return OK;
  }
  else
    warn(HOLD);
  return FALSE;
}


// it stays on the conditional list, but it's state is chanced to CLOSED
// so it can't be scanned

int simulation::withdraw(event* e)
{
  // e should exist and should be CONDITIONAL
  if (!e)
    err(NULLEV);
  if (e -> conditional())
  {
    e -> state(CLOSED);
    return OK;
  }
  else
    warn(WITHDRAW);
  return FALSE;
}


// enable the event to be scanned again

int simulation::reinstate(event* e)
{
  // e should exist and should be CLOSED
  if (!e)
    err(NULLEV);
  if (e -> closed())
  {
    e -> state(CONDITIONAL);
    return OK;
  }
  else
    warn(REINSTATE);
  return FALSE;
}


// cancel all events that receive OK from the event::verify method
// return the number of terminated events

int simulation::cancel()
{
  int i = sched -> cancel();
  i += condlist -> cancel();
  return i;
}


// reset the simulation environment, as if it was just created

void simulation::reset()
{
  delete sched;
  delete condlist;
  sched = new scheduler();
  condlist = new conditionallist();
  act = FALSE;
  curr = NULL;
  simclock = 0.0;
  running = FALSE;
  tim = 0.0;
  termcount = 0;
  countset = FALSE;
  countreached = FALSE;
  numbercreated = 0;
  numberterminated = 0;
  numberactivated = 0;
  gettimeofday(&inittime,NULL);
  int i = 0;
  while (i < resourceindex)
  {
    resources[i] -> reset();
    i++;
  }
  i = 0;
  while (i < queueindex)
  {
    queues[i] -> reset();
    i++;
  }
  i = 0;
  while (i < waitingindex)
  {
    waitings[i] -> reset();
    i++;
  }
  i = 0;
  while (i < waitingallindex)
  {
    waitingsall[i] -> reset();
    i++;
  }
  i = 0;
  while (i < eventindex)
  {
    events[i] -> reset();
    i++;
  }
  i = 0;
  while (i < behaviorindex)
  {
    behaviors[i] -> reset();
    i++;
  }
#ifdef HUSH
  if (cnt)
    cnt -> reset();
  if (ck)
    ck -> setclock(0.00);
  if (scr)
    scr -> del("simulation");
#endif
}


// add a report on the occupation of a resource

int simulation::addresourcehistogram(histogram* h)
{
  if (resourceindex >= MAXREPORT)
  {
    warn(REPORT);
    return FALSE;
  }
  resources[resourceindex] = h;
  resourceindex++;
  return OK;
}


// add a report on the size of a queue

int simulation::addqueuehistogram(histogram* h)
{
  if (queueindex >= MAXREPORT)
  {
    warn(REPORT);
    return FALSE;
  }
  queues[queueindex] = h;
  queueindex++;
  return OK;
}


// add a report on waiting times

int simulation::addwaitinghistogram(histogram* h)
{
  if (waitingindex >= MAXREPORT)
  {
    warn(REPORT);
    return FALSE;
  }
  waitings[waitingindex] = h;
  waitingindex++;
  return OK;
}


// add a report on waiting times

int simulation::addwaitingallhistogram(histogram* h)
{
  if (waitingallindex >= MAXREPORT)
  {
    warn(REPORT);
    return FALSE;
  }
  waitingsall[waitingallindex] = h;
  waitingallindex++;
  return OK;
}


// add a report on the time events spent in the simulation

int simulation::addeventreport(eventreport* e)
{
  if (eventindex >= MAXREPORT)
  {
    warn(REPORT);
    return FALSE;
  }
  events[eventindex] = e;
  eventindex++;
  return OK;
}


// add a report on behavior

int simulation::addbehaviorreport(behaviorreport* s)
{
  if (behaviorindex >= MAXREPORT)
  {
    warn(REPORT);
    return FALSE;
  }
  behaviors[behaviorindex] = s;
  behaviorindex++;
  return OK;
}


// return the behavior report with the same histogram as the one given 
// as parameter, if any

behaviorreport* simulation::findbehaviorreport(histogram* h)
{
  int i=0;
  while (i < behaviorindex )
  {
    if (h == behaviors[i] -> analyzed())
      return behaviors[i];
    i++;
  }
  return NULL;
}


// return the event report with the same title as the one given as
// parameter, if any

eventreport* simulation::findeventreport(char* tp)
{
  int i=0;
  if (tp == NULL)
    return NULL;
  while ( i < eventindex )
  {
    if (strcmp(tp,(events[i] -> histogramtitle())) == 0)
      return events[i];
    i++;
  }
  return NULL;
}
  

// select the next events from the pending tree, if any.

void simulation::selectnext()
{
#ifdef HUSH
  unsigned int delayfactor;
#endif

  // adjust the state of the event that had just been activated
  if (curr) 
  {
    if (curr -> active())
      curr -> state(PASSIVE);
  }

  // get the first event(s), depending on the all flag
  curr = sched -> front(allflag);

#ifdef HUSH
  // sleep for 'delayfactor' milliseconds and update the clock
  // if the control and clock widgets exist
  if (cnt)
  {
    delayfactor = (unsigned int)(1000 * cnt -> speed() * (curr -> time() - simclock));
    //AE thekit() -> after(delayfactor);
  }
  if (ck)
    ck -> setclock(curr -> time());
#endif

  // adjust simulation clock and the time counter
  simclock = curr -> time();
  tim = simclock;
}


// execute the current events

void simulation::execute()
{
  event* next = curr -> middlepointer();

  // adjust state and invoke the function operator
  act = OK;
  curr -> state(ACTIVE);
  (void)(*curr)();
  numberactivated++;

  // execute events with the same activation time,
  // the middlepointer of the last event of this list points to the
  // first event of this list 
  if (next) 
  {
    event* final = next;
    do 
    {
      curr = next;
      act = OK;
      curr -> state(ACTIVE);
      next = curr -> middlepointer();
      (void)(*curr)();
      numberactivated++;
    } while (final != next);
  }
}


// scan the conditional list

void simulation::scanconditional()
{
  event* conditionalHd = condlist -> head();
  // the list should exist
  if (conditionalHd != NULL)
  {
    event* next = conditionalHd -> rightpointer();

    // adjust the state of the current if it exists, the activated
    // event stays conditional
    act = OK;
    if (curr) 
    {
      if (curr -> active()) 
        curr -> state(PASSIVE);
    }

    // invoke the function operator of the head, if not CLOSED
    curr = conditionalHd;
    if (curr -> conditional())
    {
      (void)(*curr)();
      numberactivated++;
    }

    // while a next CONDITIONAL event, activate it
    while (next) 
    {
      curr = next;
      next = curr -> rightpointer();
      act = OK;
      if (curr -> conditional()) 
      {
        (void)(*curr)();
        numberactivated++;
      }
    }
  }
}


// adjust the counter and possibly quit

void simulation::decreasecount()
{
  if (countset)
  {
    termcount--;
    if (termcount < 1)
      countreached = OK;
  }
}


// generate a postscript of the screen with statistics

void simulation::postscript(char* fn)
{
#ifdef HUSH
  if (scr)
    scr -> postscript(fn);
#else
  fn = fn;
#endif
}


// put the simulation object as the scheduler and the conditional list
// in an output stream

ostream& operator<<(ostream& os,simulation& s)
{
  scheduler* sc = s.sched;
  conditionallist* c = s.condlist;
  os << (*sc);
  os << (*c);
  return os;
}

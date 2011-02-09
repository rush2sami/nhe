////////////////////////////////////////////////////////////////////////
//                                                                    //
// simulation.h                                                       //
//                                                                    //
// header for the simulation class, that represents the simulation    //
// environment						              //
//                                                                    //
// (c) 1995 D. Bolier, A. Eliens                                      //
//                                                                    //
////////////////////////////////////////////////////////////////////////


#ifndef __SIMULATION_H
#define __SIMULATION_H

#include <iostream.h>
#include <sys/time.h>
 
typedef enum {NOLEFT,QUIT,END,COUNT,REALTIME} stoptype;
enum {MAXREPORT = 100};

#ifdef HUSH
class control;
class simulationclock;
#endif
class screen;
class widget;
class event;
class histogram;
class conditionallist;
class scheduler;
class eventreport;
class behaviorreport;
class simulation;

typedef eventreport* eventreportpointer;
typedef behaviorreport* behaviorreportpointer;

// returns pointer to the simulation object. To be used in other classes
// of the library
extern simulation* thesimulation();


class simulation
{
public :
  simulation();  
  virtual ~simulation();
  void init(char* options = "");
  screen* format(char*p,char* options = "");
  screen* format(widget* w,char*p,char* options = "");
  void run();
  void run(double t);
  void run(int count);
  void reports(stoptype type,long tm,double start);
  void quit();
  int terminate(event* e);
  int schedule(event* e,double t);
  int wait(double t);
  int passivate(event* e);
  int activate(event* e);
  int hold(event* e);
  int withdraw(event* e);
  int reinstate(event* e);
  int cancel();
  void reset();
  int addresourcehistogram(histogram* h);
  int addqueuehistogram(histogram* h);
  int addwaitinghistogram(histogram* h);
  int addwaitingallhistogram(histogram* h);
  int addeventreport(eventreport* e);
  int addbehaviorreport(behaviorreport* s);
  behaviorreport* findbehaviorreport(histogram* h);
  eventreport* findeventreport(char* tp);
  void decreasecount();
  friend ostream& operator<<(ostream& os,simulation& s);
  double duration (double d) {double tmp = dur; dur=d; return tmp; }
  double duration()
    { return dur; }
  double clock()
    { return simclock; }
  event* current()
    { return curr; }
  int active()
    { return act; }
  void active(int a)
    { act = a; }
  void created()
    { numbercreated++; }
  void terminated()
     { numberterminated++; }
  void postscript(char* fn);
#ifdef HUSH
  void setcontrol(control* c)
    { cnt = c; }
  void setclock(simulationclock* c)
    { ck = c; }
#endif

// additional for soft realtime processing

  double time() { return realtime(); }	            //  the time
  double speed(double s) { return _speed = s; }
  double speed() { return _speed; }
  void suppress();
  void scan();
  void realtime(double sp);

protected:
  double realtime();                 // secs from start

private :
  double simclock;                  // simulation clock
  event* curr;                      // current events
  int act;                          // flag activity current
  int running;                      // simulation running ?
  int allflag;                      // get all events with same time ?
  int reportsflag;                  // suppress reports
  int statisticsflag;               // suppress statistics
  double tim;                       // current time
  double dur;                       // simulation duration
  int numberactivated;              // number of activated events
  int termcount;                    // termination count
  int countset;                     // specified ?
  int countreached;                 // reached the count ?
  int numbercreated;                // number of created events
  int numberterminated;             // number of terminated events
  int suppressflag;                 // suppress reports
  int resourceindex;                // index in resource reports
  int queueindex;		    // index in queue size reports
  int waitingindex; 		    // index in waiting time reports
  int waitingallindex; 		    // index in waiting time reports
  int eventindex;  	   	    // index in time spent reports
  int behaviorindex;    	    // index in behavior reports
  struct timeval inittime;          // initial real time               
  histogram* resources[MAXREPORT];  // resource reports
  histogram* queues[MAXREPORT];     // queue size reports
  histogram* waitings[MAXREPORT];   // waiting time reports
  histogram* waitingsall[MAXREPORT];   // waiting time reports (all prior)
  eventreportpointer* events;       // time spent reports
  behaviorreportpointer* behaviors; // behavior reports
  double _speed;	            // if set, we are in real time
#ifdef HUSH
  control* cnt;                     // control pointer
  simulationclock* ck;              // clock pointer
  screen* scr;                      // the statistics screen
  char* textoptions;                // and its text color
  char* parse(char* options);       // parses user defined options
#endif

  scheduler* sched;                 // the scheduler
  conditionallist* condlist;        // the conditional list

  void selectnext();                // next events to be activated
  void execute();                   // execute current events
  void scanconditional();           // scan conditional list
};

#endif

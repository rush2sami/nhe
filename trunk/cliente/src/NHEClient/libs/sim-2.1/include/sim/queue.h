////////////////////////////////////////////////////////////////////////
//                                                                    //
// queue.h                                                            //
//                                                                    //
// the definition of the queue class, it provides members to maintain //
// a priority queue of events					      //
//                                                                    //
// (c) 1995 D. Bolier, A. Eliens                                      //
//                                                                    //
////////////////////////////////////////////////////////////////////////



#ifndef __QUEUE_H
#define __QUEUE_H

#include <iostream.h>
#include <sim/tertiarytree.h>
#include <sim/simulation.h>
#include <sim/histogram.h>

class event;

ostream& queuetree(ostream& os,event* node,int d);

class queue : public tertiarytree
{
public :
  queue();
  virtual ~queue();
  void reset();
  int reportsize(histogram* hi,double intv = 0);
  int reportwaiting(histogram* hi,double intv = 0,int qp = 0);
  int reportwaitingall(histogram* hi,double intv = 0);
  int prepend(event* e);
  int append(event* e);
  event* removefront();
  event* front();
  event* removeback();
  event* back();
  int extract(event* e);
  int terminatefront();
  int terminateback();
  int cancel();
  histogram* findhistogram(int qp);
  friend ostream& operator<<(ostream& os,queue& q);
  int size()
    { return sz; }
  int empty()
    { return(sz == 0); }

private :
  int sz;                            // size of the queue
  histogram* hsize;                  // histogram on the size
  histogram* hwaitingall;            // histogram on waiting times
  histogram* hwaiting[MAXREPORT];    // histograms on waiting times
  int index;                         // index in priority array
  int* priority;                     // priority array
};

#endif

////////////////////////////////////////////////////////////////////////
//                                                                    //
// scheduler.h                                                        //
//                                                                    //
// definition of the scheduler, it keeps a tertiary tree of pending   //
// events							      //
//                                                                    //
// (c) 1995 D. Bolier, A. Eliens                                      //
//                                                                    //
////////////////////////////////////////////////////////////////////////

#ifndef __SCHEDULER_H
#define __SCHEDULER_H

#include <sim/tertiarytree.h>

class event;

ostream& pendingtree(ostream& os,event* node,int d);

class scheduler : public tertiarytree
{
public :
  scheduler();
  virtual ~scheduler();
  int insert(double t,event* e);
  void extract(event* e);
  event* front(int allflag);
  int empty();
  int cancel();
  event* giveroot();
  friend ostream& operator<<(ostream& os,scheduler& s);
};

#endif

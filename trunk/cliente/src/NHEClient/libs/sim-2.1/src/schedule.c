////////////////////////////////////////////////////////////////////////
//                                                                    //
// scheduler.c                                                        //
//                                                                    //
// implementation of the scheduler, it maintains the pending tree and //
// provides members to extract and add events to the tree. Adding an  //
// event will be on time or (if equal activation time) in priority    //
// order with highest scheduling priority first.		      //
//                                                                    //
// (c) 1995 D. Bolier, A. Eliens                                      //
//                                                                    //
////////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <iostream.h>
#include <sim/defs.h>
#include <sim/error.h>
#include <sim/event.h>
#include <sim/tertiarytree.h>
#include <sim/simulation.h>
#include <sim/scheduler.h>


// just call the constuctor of the underlying tertiary tree

scheduler::scheduler() : tertiarytree()
{
}


// destructor, calls the destroy function from the tertiary tree

scheduler::~scheduler()
{
  if (root)
    destroy(root);
  root = NULL;
}


// insert an event with given activation time

int scheduler::insert(double t,event* e)
{
  // we can't turn back time, so
  if (t < 0.0) 
    err(NDELAY);

  // adjust the event
  e -> time((sim -> clock()) + t);
  t = e -> time();
  e -> rightpointer(NULL);
  e -> leftpointer(NULL);
  e -> parentpointer(NULL);
  e -> middlepointer(NULL);
  e -> state(PENDING);

  // if this is the first event
  if (!root) 
  {
    root = e;
    return OK;
  }
  else
  {
    event* child = root;
    event* parentpointer = NULL;

    while (child) 
    {
      // search for the right place, first on activation time then on scheduling
      // priority
      parentpointer = child;
      if (t < (parentpointer -> time()))
        child = parentpointer->leftpointer();
      else if (t > (parentpointer -> time()))
        child = parentpointer->rightpointer();
      else if (e -> schedulingpriority() > (parentpointer -> schedulingpriority()))
        child = parentpointer->leftpointer();
      else if (e -> schedulingpriority() < (parentpointer -> schedulingpriority()))
        child = parentpointer->rightpointer();
      else 
      {
        // equal activation time and scheduling priority, so add it to the back
        // of the middle list
        listappend(child,e);
        return OK;
      }
    }

    // adjust pointers
    e->parentpointer(parentpointer); 
    if (t < (parentpointer -> time())) 
      parentpointer->leftpointer(e); 
    else if (t > (parentpointer -> time()))
      parentpointer->rightpointer(e);
    else if (e -> schedulingpriority() > (parentpointer -> schedulingpriority()))
      parentpointer->leftpointer(e); 
    else if (e -> schedulingpriority() < (parentpointer -> schedulingpriority()))
      parentpointer->rightpointer(e);

    return OK;
  }
}


// remove event e from the pending tree

void scheduler::extract(event* e)
{
  remove(e);
}


// get the first event from the pending tree. If allflag is equal to 
// TRUE, also get all events with the same activation time, else only 
// the first event.

event* scheduler::front(int allflag)
{
  return first(allflag);
}


// OK if empty, FALSE otherwise

int scheduler::empty()
{
  return (root == NULL);
}


// returns the root

event* scheduler::giveroot()
{
  return root;
}


// cancel every event that gets OK from the event::verify method
// return the number of terminated events

int scheduler::cancel()
{
  if (!root)
    return 0;
  else
    return verify(root);
}


// return the pending tree as output stream

ostream& operator<<(ostream& os,scheduler& s)
{
  os << endl << "-------------------------------------------------------------------" << endl;
  char* buf = new char[BUFSIZ];
  sprintf(buf,"DUMPING ALL PENDING EVENTS IN TIME/SCHEDULING ORDER root=%p\n",s.root);
  os << buf;
  sprintf(buf,"[depth]    <node> <sched prior> <time>\n");
  os << buf;
//  delete buf;   // jpa
  delete[] buf;
  pendingtree(os,s.root,1);
  return os;
}


// get the pending tree, starting at node and at a depth of d

ostream& pendingtree(ostream& os,event* node,int d)
{
  char* buf = new char[BUFSIZ];
  if (node)
  {
    // left tree
    if (node -> leftpointer())
      pendingtree (os,node -> leftpointer(),d+1);

    // d spaces and the node
    for (int i = 0;i<d;i++)
      os << ' ';
    sprintf(buf,"[%3d]      %p  %3d        %f\n",d,node,node -> schedulingpriority(),node -> time());
    os << buf;

    // middle list
    if (node -> middlepointer()) 
    {
      event* finish = node -> middlepointer();
      event* nd = finish;
      do 
      {
        // d spaces
        for (int i = 0;i<d;i++)
          os << ' ';
        sprintf(buf,"[%3d]      %p  %3d        %f\n",d,nd,nd -> schedulingpriority(),nd -> time());
        os << buf;
        nd = nd -> middlepointer();
      } while (nd != finish);
    }

    // right tree
    if (node -> rightpointer())
      pendingtree(os,node -> rightpointer(),d+1);
  }
  return os;
}

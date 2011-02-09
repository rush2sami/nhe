////////////////////////////////////////////////////////////////////////
//                                                                    //
// conditionallist.c                                                  //
//                                                                    //
// implements the list of conditional events, members for removing    //
// and for adding an event are provided. Adding an event will be in   //
// scheduling priority order.                                         //
//                                                                    //
// (c) 1995 D. Bolier, A. Eliens                                      //
//                                                                    //
////////////////////////////////////////////////////////////////////////



#include <stddef.h>
#include <iostream.h>
#include <stdio.h>
#include <sim/event.h>
#include <sim/error.h>
#include <sim/conditionallist.h>
#include <sim/tertiarytree.h>
#include <sim/simulation.h>



// constructor, sets head and tail to the null pointer

conditionallist::conditionallist()
{
  hd = NULL;
  tl = NULL;
}


// destructor, deletes every event on the list

conditionallist::~conditionallist()
{
  // possibly head = tail or both NULL
  if ( (hd == NULL) || (tl == NULL) )
    return;
  simulation* sim = thesimulation();
  // one event
  if ( hd == tl )
  {
    if (sim)
    {
      sim -> terminated();
      // possibly quit the simulation
      if (hd -> killflag())
        sim -> decreasecount();
    }
    delete hd;
    return;
  }

  // none of these
  event* next = hd -> rightpointer();
  event* nd = hd -> rightpointer();
  while (next != tl)
  {
    next = nd -> rightpointer();
    if (sim)
    {
      sim -> terminated();
      // possibly quit the simulation
      if (nd -> killflag())
        sim -> decreasecount();
    }
    delete nd;
    nd = next;
  }
  delete hd;
  delete tl;
}


// adds an event e in scheduling priority order, the event with the
// highest priority will always be at the front of the list.
// Events are added in LIFO order

void conditionallist::add(event* e)
{
  // make sure no left or right pointer and adjust the state
  e -> leftpointer(NULL);
  e -> rightpointer(NULL);
  e -> state(CONDITIONAL);

  // if empty
  if ( (hd == NULL) || (tl == NULL) ) 
    hd = tl = e;

  // if e has the highest priority
  else if ( e -> schedulingpriority() > hd -> schedulingpriority() )
  {
    e -> rightpointer(hd);
    hd -> leftpointer(e);
    hd = e;
  }

  // if e has the lowest priority
  else if ( e -> schedulingpriority() <= tl -> schedulingpriority() )
  {
    tl -> rightpointer(e);
    e -> leftpointer(tl);
    tl = e;
  }

  // none of these
  else
  {
    event* y = hd;
    // search right place, there is always a rightpointer
    while (e -> schedulingpriority() <= y -> schedulingpriority())
      y = y -> rightpointer();
    // insert
    y -> leftpointer() -> rightpointer(e);
    e -> leftpointer(y -> leftpointer());
    y -> leftpointer(e);
    e -> rightpointer(y);
  }
}


// remove event e from the conditional list
// notice that we don't have to dereference the tail pointer, because
// we adjust the head and tail, if one of them is equal to NULL.

void conditionallist::remove(event* e)
{
  // if e is the head
  if (hd == e) 
  {
    hd = hd -> rightpointer();   // possibly NULL
    if (hd)
      hd -> leftpointer(NULL);
  }

  // if e is the tail
  else if (tl == e) 
  {
    tl = tl -> leftpointer();    // possibly NULL
    if (tl)
      tl -> rightpointer(NULL);
  }

  // none of these
  else 
  {
    // cut links
    (e->leftpointer())->rightpointer(e->rightpointer());
    (e->rightpointer())->leftpointer(e->leftpointer());
  }
}


// cancel all events that receive OK from the event::verify method
// return the number of terminated events

int conditionallist::cancel()
{
  simulation* sim = thesimulation();
  if (!sim)
    err(INITERR);
  if (!hd)
    return 0;
  event* e = hd;
  // scan the list
  event* next;
  int i=0;
  do
  {
    next = e -> rightpointer();
    if (e -> verify())
    {
      // possibly quit the simulation
      if (e -> killflag())
        sim -> decreasecount();
      // remove and delete
      remove(e);
      delete e;
      // sim always exists
      sim -> terminated();
      i++;
    }
    e = next;
  }
  while (next);
  return i;
}

    
// overloads the operator<<. A stream of the conditional list is 
// returned

ostream& operator<<(ostream& os,conditionallist& c)
{
  os << endl << "-------------------------------------------------------------------" << endl;
  os << "DUMPING CONDITIONAL EVENTS" << endl << "[position] <event>   <priority>" << endl;
  list(os,c.hd,1);
  return os;
}


// returns a output stream of a list with as head event p

ostream& list(ostream& os,event* p,int n)
{
  while (p)
  {
    char* buf = new char[BUFSIZ];
    sprintf(buf,"[%3d]       %p     %d\n",n++,p,p -> schedulingpriority());
    os << buf;
    p = p -> rightpointer();
    delete buf;
  }
  return os;
}

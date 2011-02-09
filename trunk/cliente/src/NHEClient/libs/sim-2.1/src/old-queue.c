////////////////////////////////////////////////////////////////////////
//                                                                    //
// queue.c                                                            //
//                                                                    //
// the implementation of the queue class, it provides members to      //
// maintain a priority queue of events                                //
//                                                                    //
// (c) 1995 D. Bolier, A. Eliens                                      //
//                                                                    //
////////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <string.h>
#include <sim/event.h>
#include <sim/defs.h>
#include <sim/error.h>
#include <sim/queue.h>
#include <sim/tertiarytree.h>
#include <sim/reports.h>
#include <sim/simulation.h>


// The underlying structure is a tertiary tree

queue::queue() : tertiarytree()
{
  sz = 0;
  hsize = NULL;
  hwaitingall = NULL;

  // create the priorities array, it stays synchronized with the 
  // histograms on waiting times
  priority = new int[MAXREPORT];
  for (int i=0;i<MAXREPORT;i++)
  {
    hwaiting[i] = NULL;
    priority[i] = 0;
  }
  index = 0;
}


// destructor, dereferences the histogram pointers and deallocates
// the tertairy tree and the priorities array

queue::~queue()
{
  if (root)
    destroy(root);
  if (hsize)
    hsize = NULL;
  if (hwaitingall)
    hwaitingall = NULL;
  for (int i=0;i<MAXREPORT;i++)
    hwaiting[i] = NULL;
  delete priority;
}


// reset the the reports and remove every queued event

void queue::reset()
{
  // reset tertiary tree
  if (root)
    destroy(root);
  root = NULL;

  // reset histogram arrays
  if (hsize)
    hsize -> reset();
  if (hwaitingall)
    hwaitingall -> reset();
  int i = 0;
  while (i < index)
  {
    hwaiting[i] -> reset();
    i++;
  }
  sz = 0;
}


// generate a report on the size of the queue each intv time units. 
// The report gets the histogram given as parameter to the method

int queue::reportsize(histogram* hi,double intv)
{
  // histogram should exist and be of type WEIGHTED
  if (!hi)
    err(NOHISTO);
  if (hi -> histogramkind() != WEIGHTED)
  {
    warn(HWEIGHTED);
    hi -> histogramkind(WEIGHTED);
  }
  hsize = hi;

  // add it to the simulation object
  if (!(sim -> addqueuehistogram(hi)))
    return FALSE;

  // if specified create a coutreport 
  if (intv)
  {
    coutreport* crep = new coutreport(hi,intv);
    if (!crep)
      return FALSE;
    sim -> schedule(crep,intv);
  }
  return OK;
}


// generate a report on the waiting times of the events of given
// queuing priority each intv time units. 
// The report gets the histogram given as parameter to the method

int queue::reportwaiting(histogram* hi,double intv,int qp)
{
  // the histogram should exist and be of type FREQUENCY
  if (!hi)
    err(NOHISTO);
  if (hi -> histogramkind() != FREQUENCY)
  {
    warn(HFREQUENCY);
    hi -> histogramkind(FREQUENCY);
  }
  
  // set the histogram and priority, if possible
  if (index < MAXREPORT)
  {
    hwaiting[index] = hi;
    priority[index] = qp;
    index++;
  }
  else
    warn(REPORT);

  // add it to the simulation object
  if (!(sim -> addwaitinghistogram(hi)))
    return FALSE;

  // if specified create a coutreport
  if (intv)
  {
    coutreport* crep = new coutreport(hi,intv);
    if (!crep)
      return FALSE;
    sim -> schedule(crep,intv);
  }
  return OK;
}


// generate a report on the waiting times of the events regardless of
// their queuing priority and each intv time units. 
// The report gets the histogram given as parameter to the method

int queue::reportwaitingall(histogram* hi,double intv)
{
  // the histogram should exist and be of type FREQUENCY
  if (!hi)
    err(NOHISTO);
  if (hi -> histogramkind() != FREQUENCY)
  {
    warn(HFREQUENCY);
    hi -> histogramkind(FREQUENCY);
  }
  
  hwaitingall = hi;

  // add it to the simulation object
  if (!(sim -> addwaitingallhistogram(hi)))
    return FALSE;

  // if specified create a coutreport
  if (intv)
  {
    coutreport* crep = new coutreport(hi,intv);
    if (!crep)
      return FALSE;
    sim -> schedule(crep,intv);
  }
  return OK;
}


// add it in priority order and to the front (if equal priorities)
// if reports exist adjust the reports

int queue::prepend(event* e)
{
  // event should exist
  if (!e)
    err(ADDTOQ);

  // event should be active or passive
  if ((e -> active()) || (e -> passive())) 
  {
    // if a report on the size exists add a sample
    if (hsize)
      hsize -> sample(sz);

    // if a report on waiting times exists, add a time stamp
    if (findhistogram(e -> queuingpriority()))
      e -> waitingreporttime(sim -> clock());

    // if a report on waiting times (regardless of priority)
    // exists, add a time stamp (may be done allready)
    if (hwaitingall)
      e -> waitingreporttime(sim -> clock());

    // dereferences pointers
    e->leftpointer(NULL);
    e->rightpointer(NULL);
    e->middlepointer(NULL);

    // set state to QUEUED
    if (e -> active())
      sim -> active(FALSE) ;
    e -> state(QUEUED);

    // if it is the first event
    if (sz < 1) 
    {
      root = e;
      sz = 1;
    }
    else 
    {
      sz++;
      event* child = root;
      event* parentpointer = NULL;

      while (child) 
      {

        // search the right place
        parentpointer = child;
        if (child->queuingpriority() > e->queuingpriority())
          child = child->rightpointer();
        else if (child->queuingpriority() < e->queuingpriority())
          child = child->leftpointer();
        else 
        {
          // same priorities, add it to the front of the list
          listprepend(child,e);
          if (child == root)
            root = e;
          return OK;
        }
      }
      
      // adjust pointers
      e->parentpointer(parentpointer);
      if (e->queuingpriority() < parentpointer->queuingpriority())
        parentpointer->rightpointer(e);
      else 
        parentpointer->leftpointer(e);
    }
    return OK;
  }
  return FALSE;
}


// add it in priority order and to the back (if equal priorities)
// if reports exist adjust the reports

int queue::append(event* e)
{
  // event should exist and be PASSIVE or ACTIVE
  if (!e) 
    err(ADDTOQ);
  if ((e -> active()) || (e -> passive())) 
  {
    // if report is set add sample on the size and/or a time stamp
    if (hsize)
      hsize -> sample(sz);
    if (findhistogram(e -> queuingpriority()))
      e -> waitingreporttime(sim -> clock());

    // if a report on waiting times (regardless of priority)
    // exists, add a time stamp (may be done allready)
    if (hwaitingall)
      e -> waitingreporttime(sim -> clock());

    // derefernce pointers
    e->leftpointer(NULL);
    e->rightpointer(NULL);
    e->middlepointer(NULL);

    // adjust state
    if (e -> active())
      sim -> active(FALSE);
    e->state(QUEUED);

    // if the first event then
    if (sz < 1) 
    {
      root = e;
      sz = 1;
    }
    else 
    {
      sz++;
      event* child  = root;
      event* parentpointer = NULL;
      while (child) 
      {
        // search for the right place to insert the event
        parentpointer = child;
        if (child->queuingpriority() < e->queuingpriority())
          child = child->leftpointer();
        else if (child->queuingpriority() > e->queuingpriority())
          child = child->rightpointer();
        else 
        {
          // same priorities, add to the back of the list
          listappend (child,e);
          return OK;
        }
      }

      // adjust pointers
      e->parentpointer(parentpointer);
      if (e->queuingpriority() > parentpointer->queuingpriority())
        parentpointer->leftpointer(e);
      else
        parentpointer->rightpointer(e);
    }
    return OK;
  }
  return FALSE;
}


// return and remove the front of the queue

event* queue::removefront()
{
  // should be possible
  if (sz < 1) 
  {
    sz = 0;
    err(EMPTYQ);
    return NULL;
  }
  else
  {
    // extract from the tree and adjusts state of the event
    event* e = first(FALSE);
    e -> state(PASSIVE);

    // add a sample on the size and the waiting times, if the reports 
    // exist and if it was appended when the report existed
    if (hsize)
      hsize -> sample(sz);
    histogram* h = findhistogram(e -> queuingpriority());
    if (h && e -> waitingreporttime())
      h -> sample(sim -> clock() - e -> waitingreporttime());
    if (hwaitingall && e -> waitingreporttime())
      hwaitingall -> sample(sim -> clock() - e -> waitingreporttime());

    // adjust size. 
    sz--;

    return(e);
  }
}


// just return the front of the queue

event* queue::front()
{
  return (first());
}


// return and remove the back of the queue

event* queue::removeback()
{
  // should be possible
  if (sz < 1) 
  {
    sz = 0;
    err(EMPTYQ);
    return NULL;
  }
  else
  {
    // extract from the tree and adjusts state of the event
    event* e = last(FALSE);
    e -> state(PASSIVE);

    // add a sample on the size and the waiting times, if the reports 
    // exist and if it was appended when the report existed
    if (hsize)
      hsize -> sample(sz);
    histogram* h = findhistogram(e -> queuingpriority());
    if (h && e -> waitingreporttime())
      h -> sample(sim -> clock() - e -> waitingreporttime());
    if (hwaitingall && e -> waitingreporttime())
      hwaitingall -> sample(sim -> clock() - e -> waitingreporttime());

    // adjust size
    sz--;

    return(e);
  }
}


// just return the back of the queue

event* queue::back()
{
  return (last());
}


// extract an event e from the queue, return OK on sucess, FALSE on
// failure, i.e. if it is not queued or not in the tree

int queue::extract(event* e)
{
  // event should exist
  if (e == NULL)
    err(NULLEV);

  // empty queue ?
  if (sz < 1)
    return FALSE;

  // e should be queued and in the tree
  if (e -> queued() && member(root,e))
  {
    e -> state(PASSIVE);

    // add samples, if the reports exist
    if (hsize)
      hsize -> sample(sz);
    histogram* h = findhistogram(e -> queuingpriority());
    if (h && e -> waitingreporttime())
      h -> sample(sim -> clock() - e -> waitingreporttime());
    if (hwaitingall && e -> waitingreporttime())
      hwaitingall -> sample(sim -> clock() - e -> waitingreporttime());

    // remove from the tree and if it is the root, set the root on NULL
    if (sz > 1) 
    {
      sz--;
      remove(e);
      return OK;
    }
    else if (root == e) 
    {
      root = NULL;
      e->leftpointer(NULL);
      e->rightpointer(NULL);
      e->middlepointer(NULL);
      e->parentpointer(NULL);
      sz--;
      return OK;
    }
  }
  return FALSE;
}


// terminate the front, without adding a sample

int queue::terminatefront()
{
  if (sz < 1)
    return FALSE;

  // extract and delete the event
  event* e = first(FALSE);

  // possibly quit the simulation
  if (e -> killflag())
    sim -> decreasecount();
 
  delete e;
  sim -> terminated();
  sz--;

  return OK;
}


// terminate the back, without adding a sample

int queue::terminateback()
{
  if (sz < 1)
    return FALSE;

  // extract and delete
  event* e = last(FALSE);

  // possibly quit the simulation
  if (e -> killflag())
    sim -> decreasecount();
 
  delete e;
  sim -> terminated();
  sz--;

  return OK;
}


// cancel every event that gets OK from the event::verify method
// return the number of terminated events

int queue::cancel()
{
  int i = verify(root);
  sz -= i;
  return i;
}


// find the histogram with the given priority, if possible

histogram* queue::findhistogram(int qp)
{
  int i=0;
  while (i<index)
  {
    if (priority[i] == qp)
      return hwaiting[i];
    i++;
  }
  return NULL;
}


// overloaded operator<<, prints the size and the tree structure

ostream& operator<<(ostream& os,queue& q)
{
  // the header
  os << endl << "-------------------------------------------------------------------" << endl;
  char* buf = new char[BUFSIZ];
  sprintf(buf,"DUMPING QUEUE root=%p, size=%d\n",q.root,q.sz);
  os << buf;
  sprintf(buf,"[depth]    <node> <queue prior> <time>\n");
  os << buf;
  delete buf;

  // the tree
  queuetree(os,q.root,1);

  return os;
}


// returns the tree structure of the queue, starting at node and at a
// depth of d

ostream& queuetree(ostream& os,event* node,int d)
{
  char* buf = new char[BUFSIZ];
  if (node)
    {
    // left tree
    if (node -> leftpointer())
      queuetree (os,node -> leftpointer(),d+1);

    // spaces and this node
    for (int i=0;i<d;i++)
      os << ' ';
    sprintf(buf,"[%3d]      %p  %3d        %f\n",d,node,node -> queuingpriority(),node -> time());
    os << buf;

    // middle list
    if (node -> middlepointer()) 
    {
      event* finish = node -> middlepointer();
      event* nd = finish;
      do 
      {
        // d spaces
        for (int i=0;i<d;i++)
          os << ' ';
        sprintf(buf,"[%3d]      %p  %3d        %f\n",d,nd,nd -> queuingpriority(),nd -> time());
        os << buf;
        nd = nd -> middlepointer();
      } while (nd != finish);
    }

    // right tree
    if (node -> rightpointer())
      queuetree(os,node -> rightpointer(),d+1);
  }
  return os;
}

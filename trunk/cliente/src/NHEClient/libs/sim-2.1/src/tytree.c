////////////////////////////////////////////////////////////////////////
//                                                                    //
// tertiarytree.c                                                     //
//                                                                    //
// implementation of the tertiary tree, that is used by the scheduler //
// and the queue. An event in a tertiary tree has pointers to a left  //
// and a right child, a pointer to it's parent and a middle pointer   //
// to a doubly linked list. The class provides methods to remove and  //
// and to return an event. The methods to add an event to the tree    //
// are implemented in the classes for the scheduler and queue, since  //
// they use different strategies. The root and methods are all        //
// protected, so they will be private to the scheduler and the queue  //
//                                                                    //
// (c) 1995 D. Bolier, A. Eliens                                      //
//                                                                    //
////////////////////////////////////////////////////////////////////////



#include <stdio.h>
#include <sim/event.h>
#include <sim/defs.h>
#include <sim/tertiarytree.h>
#include <sim/simulation.h>
#include <sim/error.h>

  
// set the root and the simulation object

tertiarytree::tertiarytree()
{
  root = NULL;
  sim = thesimulation();
  if (!sim)
    err(INITERR);
}


// destroy the tree if it exists

tertiarytree::~tertiarytree()
{
  if (root)
    destroy(root);
  sim = NULL;
}


// remove an event e form the tertiary tree, assuming it is in the tree

void tertiarytree::remove(event* e)
{
  event* x = e->middlepointer();

  // if e in a list
  if (e->leftpointer() == e)
    listremove(e,x);

  // if e has a existing middle list
  else if (x) 
  {
    if (x->middlepointer() != x) 
    {
      // there is more then one child
      event* y = x->middlepointer();
      (x->parentpointer())->middlepointer(y);
      y->parentpointer(x->parentpointer());
      y->rightpointer(x);
    }
    else
      x->middlepointer(NULL);

    // update pointers
    x->parentpointer(e->parentpointer());
    x->leftpointer(e->leftpointer()); 
    if (x->leftpointer())
      (x->leftpointer())->parentpointer(x);
    x->rightpointer(e->rightpointer());

    if (x->rightpointer())
      (x->rightpointer())->parentpointer(x);

    // adjust the root
    if (root == e)
      root = x;
    else if ((e->parentpointer())->leftpointer() == e)
      (e->parentpointer())->leftpointer(x);
    else
      (e->parentpointer())->rightpointer(x);
  }
  // just remove it from the tree
  else 
    treeremove(e);
}


// get the first (i.e. most left) event from the tree. If allflag
// get also it's middle list

event* tertiarytree::first(int allflag)
{
  event* child = root;
  event* parentpointer = NULL;

  // search for the first event
  while (child->leftpointer()) 
  {
    parentpointer = child;
    child  = child->leftpointer();
  }

  // get it all ?
  if (allflag) 
  {
    if (child == root)
      root = child->rightpointer();
    else 
    {
      parentpointer->leftpointer(child->rightpointer());
      if (child->rightpointer())
        (child->rightpointer())->parentpointer(parentpointer);
    }
    return child;
  }
  // if not, and if it has a middle list, update pointers
  else if (child->middlepointer()) 
  {
    event* x = child->middlepointer();

    if (x->middlepointer() == x) 
    {
      x->middlepointer(NULL);
    }
    else 
    {
      (x->parentpointer())->middlepointer(x->middlepointer());
      (x->middlepointer())->parentpointer(x->parentpointer());
      (x->middlepointer())->rightpointer(x);
    }

    x->parentpointer(child->parentpointer());
    x->leftpointer(child->leftpointer());
    x->rightpointer(child->rightpointer());
    if (x->rightpointer())
      (x->rightpointer())->parentpointer(x);

    if (child == root)
      root = x;
    else if (x->parentpointer())
      (x->parentpointer())->leftpointer(x);
  }
  
  // update the root, if nessecary
  else if (child == root) 
  {
    root = child->rightpointer();
    if (root)
      (root)->parentpointer(NULL);
  }
  else 
  {
    // move right child to the parent's left
    parentpointer->leftpointer(child->rightpointer());
    if (child->rightpointer())
      (child->rightpointer())->parentpointer(parentpointer);
  }

  // no need for a middle list, so
  child->middlepointer(NULL);

  return child;
}


// get the last (i.e. most right) event from the tree. If allflag
// get also it's middle list

event* tertiarytree::last(int allflag)
{
  event* child = root;
  event* parentpointer = NULL;

  // search for the last event
  while (child->rightpointer()) 
  {
    parentpointer = child;
    child = child->rightpointer();
  }

  // get them all ?
  if (allflag) 
  {
    if (child == root)
      root = child->leftpointer();
    else 
    {
      parentpointer->rightpointer(child->leftpointer());
      if (child->leftpointer())
        (child->leftpointer())->parentpointer(parentpointer);
    }
    return child;
  }
  else if (child->middlepointer()) 
  {
    // just return the last child form the list
    event* x = child->middlepointer();
    child = x->parentpointer();
    listremove(child,x);
  }
  else if (child == root) 
  {
    // check if we need to update the root
    root = child->leftpointer();
    if (root)
      (root)->parentpointer(NULL);
  }
  else 
  {
    // move left child to parent's right
    parentpointer->rightpointer(child->leftpointer());
    if (child->leftpointer())
      (child->leftpointer())->parentpointer(parentpointer);
  }
  child->middlepointer(NULL);
  return child;
}


// just return the first (i.e. most left) event

event* tertiarytree::first()
{
  if (!root)
    return NULL;
  event* child = root;
  while (child->leftpointer()) 
    child = child->leftpointer();
  return child;
}


// just return the last (i.e. most right) event

event* tertiarytree::last()
{
  if (!root)
    return NULL;
  event* child = root;
  while (child->rightpointer()) 
    child = child->rightpointer();

  // last event from the middle list
  if (child->middlepointer())
    return (child->middlepointer()->parentpointer());

  return child;
}


// remove an event e from the tree, assuming it is not in a list,
// it has no middle list and it is in the tree

void tertiarytree::treeremove(event* e)
{
  // work out which event to reposition
  event* y;
  if (e->leftpointer() && e->rightpointer()) 
  {
    for (y = e->rightpointer();y->leftpointer();y = y->leftpointer())
      ;
  }
  else
    y = e;

  // set x to an existing child of y or NULL, if there are no children
  event* x;
  if (y->leftpointer())
    x = y->leftpointer();
  else
    x = y->rightpointer();

  // set the pointers for x en y
  if (x)
    x->parentpointer(y->parentpointer());
  if (root == y)
    root = x;
  else if (y == (y->parentpointer())->leftpointer())
    (y->parentpointer())->leftpointer(x);
  else
    (y->parentpointer())->rightpointer(x);

  // insert y in place of event e
  if (y != e) 
  {
    y->parentpointer(e->parentpointer());
    y->leftpointer(e->leftpointer());
    y->rightpointer(e->rightpointer());

    if (e->leftpointer())
      (e->leftpointer())->parentpointer(y);
    if (e->rightpointer())
      (e->rightpointer())->parentpointer(y);

    // update the root or the parent
    if (root == e)
      root = y;
    else if ((e->parentpointer())->leftpointer() == e)
      (e->parentpointer())->leftpointer(y);
    else
      (e->parentpointer())->rightpointer(y);
  }

  // invalidate e's old pointers
  e->rightpointer(NULL);
  e->leftpointer(NULL);
  e->parentpointer(NULL);
  e->middlepointer(NULL);
}


// return OK, if e in the tree rooted at y

int tertiarytree::member(event* y,event* e)
{
  if (y == NULL)
    return FALSE;
  else if (y == e)
    return OK;
  else if (member(y->leftpointer(),e))
    return OK;
  else if (member(y->rightpointer(),e))
    return OK;
  else if (onlist(y,e))
    return OK;
  else
    return FALSE;
}
     
    
// remove the event node from the list, next is the event following node

void tertiarytree::listremove(event* node,event* next)
{
  if (node == next) 
  {
    // nothing else in the list
    (node->rightpointer())->middlepointer(NULL);
    return;
  }
  else if (node->rightpointer())
  {
    // e is at the head of the list
    (node->rightpointer())->middlepointer(next);
    next->rightpointer(node->rightpointer());
  }
  (node->parentpointer())->middlepointer(next);
  next->parentpointer(node->parentpointer());
}


// add event e to the tree in front of event x

void tertiarytree::listprepend(event* x,event* e)
{
  e->rightpointer(x->rightpointer());
  e->leftpointer(x->leftpointer());
  e->middlepointer(x);
  e->parentpointer(x->parentpointer());

  // update pointers from the parent of x
  if (x->parentpointer()) 
  {
    if ((x->parentpointer())->leftpointer() == x)
      (x->parentpointer())->leftpointer(e);
    else
      (x->parentpointer())->rightpointer(e);
  }

  x->rightpointer(e);

  // flag that child is now in middle list
  x->leftpointer(x);

  event* y;
  if (x->middlepointer())
  {
    y = x->middlepointer();
    x->parentpointer(y->parentpointer());
    (y->parentpointer())->middlepointer(x);
    y->parentpointer(x);
    y->rightpointer(NULL);
  }
  else
  {
    x->parentpointer(x);
    x->middlepointer(x);
  }
}


// add event e to the end of the list

void tertiarytree::listappend(event* x,event* e)
{
  // flag that it is in a middle list now
  e->leftpointer(e);

  event* y;
  if (x->middlepointer())
  {
    y = x->middlepointer();
    e->parentpointer(y->parentpointer());
    (y->parentpointer())->middlepointer(e);
    y->parentpointer(e);
    e->middlepointer(y);
    e->rightpointer(NULL);
  }
  else
  {
    // first event
    x->middlepointer(e);
    e->rightpointer(x);
    e->parentpointer(e);
    e->middlepointer(e);
  }
}


// return OK if e on the middle list of y

int tertiarytree::onlist(event* y,event* e)
{
  event* finish,*next;
  if (y->middlepointer())
  {
    finish = y -> middlepointer();
    if (finish == e)
      return OK;
    next = finish -> middlepointer();
    while (next != finish)
    {
      if (next == e)
        return OK;
      next = next -> middlepointer();
    }
  }
  return FALSE;
}


// terminate every event on the tree rooted at rt that gets OK from 
// the event::verify method, return the number of terminated events
// we assume rt exists

int tertiarytree::verify(event* rt)
{
  // scan tree
  int i=0;
  if (rt -> leftpointer())
    i += verify(rt -> leftpointer());
  if (rt -> rightpointer())
    i += verify(rt -> rightpointer());
  if (rt -> middlepointer())
    i += verifylist(rt -> middlepointer());
  if (rt -> verify())
  {
    // possibly quit the simulation
    if (rt -> killflag())
      sim -> decreasecount();
    // remove and delete
    remove(rt);
    delete rt;
    // sim always exists
    sim -> terminated();
    i++;
  }
  return i;
}


// terminate every event on the list rooted at x that gets OK from 
// the event::verify method, return the number of terminated events
// we assume x exists

int tertiarytree::verifylist(event* x)
{
  event* nd, *finish, *next;
  int i=0;
  if (x->middlepointer())
  {
    finish = x -> middlepointer();
    nd = finish -> middlepointer();
    next = nd;
    while (next != finish)
    {
      next = nd -> middlepointer();
      if (nd -> verify())
      {
        // possibly quit the simulation
        if (nd -> killflag())
          sim -> decreasecount();
        // remove and delete
        remove(nd);
        delete nd;
        // sim always exists
        sim -> terminated();
        i++;
      }
      nd = next;
    }
    if (finish -> verify())
    {
      // possibly quit the simulation
      if (finish -> killflag())
        sim -> decreasecount();
      // remove and delete
      remove(finish);
      delete finish;
      // sim always exists
      sim -> terminated();
      i++;
    }
  }
  return i;
}


// destroy the tree rooted at rt, assuming rt exists

void tertiarytree::destroy(event* rt)
{
  // scan the tree
  if (rt->leftpointer())
    destroy(rt->leftpointer());
  if (rt->rightpointer())
    destroy(rt->rightpointer());
  if (rt->middlepointer())
    destroylist(rt);
  // does sim exist ?
  if (sim)
  {
    sim -> terminated();
    // possibly quit the simulation
    if (rt -> killflag())
      sim -> decreasecount();
  }
  // remove and delete
  remove(rt);
  delete rt;
}


// destroy the middle list of x, assuming x exists

void tertiarytree::destroylist(event* x)
{
  event* nd, *finish, *next;
  if (x->middlepointer())
  {
    finish = x -> middlepointer();
    nd = finish -> middlepointer();
    next = nd;
    // scan the list
    while (next != finish)
    {
      next = nd -> middlepointer();
      // does sim exist ?
      if (sim)
      {
        sim -> terminated();
        // possibly quit the simulation
        if (nd -> killflag())
          sim -> decreasecount();
      }
      // remove and delete
      remove(nd);
      delete nd;
      nd = next;
    }
    // does sim exist ?
    if (sim)
    {
      sim -> terminated();
      // possibly quit the simulation
      if (finish -> killflag())
        sim -> decreasecount();
    }
    // remove and delete
    remove(finish);
    delete finish;
  }
}

////////////////////////////////////////////////////////////////////////
//                                                                    //
// tertiarytree.h                                                     //
//                                                                    //
// abstract data type for a tertiary tree                             //
//                                                                    //
// (c) 1995 D. Bolier, A. Eliens                                      //
//                                                                    //
////////////////////////////////////////////////////////////////////////


#ifndef __TERTIARYTREE_H
#define __TERTIARYTREE_H

class event;
class simulation;

class tertiarytree
{
protected :
  event* root;                          // the root of the tree
  simulation* sim;                      // the simulation object

  tertiarytree();
  virtual ~tertiarytree();
  void remove(event* e);
  void treeremove(event* e);
  int member(event* y,event* x);
  void listappend(event* x,event* e);
  void listprepend(event* x,event* e);
  void listremove(event* node,event* next);
  event* first(int allflag );
  event* last(int allflag );
  event* first();
  event* last();
  int onlist(event* y,event* x);
  int verify(event* rt);
  int verifylist(event* x);
  void destroy(event* rt);
  void destroylist(event* x);
};

#endif

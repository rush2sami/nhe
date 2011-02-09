////////////////////////////////////////////////////////////////////////
//                                                                    //
// entity.h                                                           //
//                                                                    //
// header for entity.c, that models the concept of a process in the   //
// simulation.                                                        //
//                                                                    //
// (c) 1995 D. Bolier, A. Eliens                                      //
//                                                                    //
////////////////////////////////////////////////////////////////////////


#ifndef __ENTITY_H
#define __ENTITY_H

#include <sim/event.h>

class entity : public event
{
public :
  int phase()
    { return phas; }
  void phase(int p)
    { phas = p; }

protected :
  int phas;                                     // phase of the entity
  entity(int ph=0,int qp=0,int sp=0,int kf=0);  // protected, so
                              // it can only be used by a derived class
};

#endif

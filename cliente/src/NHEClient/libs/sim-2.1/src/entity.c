////////////////////////////////////////////////////////////////////////
//                                                                    //
// entity.c                                                           //
//                                                                    //
// implements the concept of a process in the simulation.             //
//                                                                    //
// (c) 1995 D. Bolier, A. Eliens                                      //
//                                                                    //
////////////////////////////////////////////////////////////////////////


#include <sim/event.h>
#include <sim/entity.h>


// constructor taking the phase, queuing and scheduling priority of the
// entity. The base event is created with the both priorities.

entity::entity(int ph,int qp,int sp,int k) : event(qp,sp,k)
{
  phas = ph;
}

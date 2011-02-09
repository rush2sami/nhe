////////////////////////////////////////////////////////////////////////
//                                                                    //
// sim.h                                                            //
//                                                                    //
// definition of the library, it includes the relevant headers and    //
// the main headers from the standard library                         //
//                                                                    //
// (c) 1995 D. Bolier, A. Eliens                                      //
//                                                                    //
////////////////////////////////////////////////////////////////////////
 
 
#ifndef __SIM_H
#define __SIM_H
 
#ifndef ASCII
#ifndef HUSH
#define HUSH
#endif
#endif
 
#include <stdio.h>
#include <iostream.h>
#include <iomanip.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
 
#include <sim/analysis.h>
#include <sim/defs.h>
#include <sim/entity.h>
#include <sim/event.h>
#include <sim/generator.h>
#include <sim/histogram.h>
#include <sim/queue.h>
#include <sim/resource.h>
#include <sim/simulation.h>
#include <sim/screen.h>
#include <sim/control.h>
#include <sim/clock.h>
#include <sim/query.h>
#include <sim/hush.h>
 
#endif

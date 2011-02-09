////////////////////////////////////////////////////////////////////////
//                                                                    //
// error.c                                                            //
//                                                                    //
// contains the error and warning functions                           //
//                                                                    //
// (c) 1995 D. Bolier, A. Eliens                                      //
//                                                                    //
////////////////////////////////////////////////////////////////////////


#include <iostream.h>
#include <stdio.h>
#include <stdlib.h>
#include <sim/simulation.h>
#include <sim/error.h>



// prints an error message tand exits

void err(errortype n)
{
  static char* errmsg[] =
  {
    "negative simulation duration",
    "negative count",
    "scheduled events list empty",
    "cannot have negative time delay",
    "cannot add NULL event to the queue",
    "cannot remove an event from an empty queue",
    "illegal number of columns in histogram",
    "illegal histogram mode",
    "probability values must be in range 0 to 1",
    "must have upper limit > lower limit in uniform distribution",
    "negative standard deviation in normal distribution",
    "negative standard deviation in log-normal distribution", 
    "mode out of range in triangular distribution",
    "value of arguments less than zero",
    "argument equal to zero in beta function",
    "negative mean for exponential",
    "negative mean for erlang",
    "poulation size less then number drawn in hypergeometric",
    "NULL event",
    "NULL histogram",
    "simulation object not created or created after other objects",
    "illegal analysis mode",
    "illegal histogram form",
    "cannot open the file",
    "cannot find a file",
    "fraction world / screen differs between width and height",
    "wrong confidence level",
    "wrong initialization of scan flag",
    "wrong initialization of suppress flag",
    "seeds for the generator out of range"
  };

  cerr << endl << "run-time error : " << errmsg[n] << endl;
  exit(0);
}


// prints a warning message

void warn(warningtype n)
{
  static char* warnmsg[] = 
  {
    "negative standard deviation in statistics",
    "not enough samples",
    "less than 1 degree of freedom",
    "resource amount below zero or greater then initial",
    "too many reports",
    "delay between 0.0 and 1.0",
    "no output-structures left",
    "no fit possible",
    "changed histogram type to weighted",
    "changed histogram type to frequency",
    "changed histogram type to series",
    "took maximum value in the weibull fit",
    "couldn't perform a hyperexponential fit",
    "can only terminate events that are passive or active",
    "can only schedule events that are passive",
    "can only schedule an active current",
    "passivated event is passive or queued",
    "activated event is active or queued",
    "can only hold a passive event",
    "can only withdraw a conditional event",
    "can only reinstate a closed event",
  };

  // just print the message
  cerr << endl << "warning : " << warnmsg[n] << endl;
}

////////////////////////////////////////////////////////////////////////
//                                                                    //
// error.h                                                            //
//                                                                    //
// definitions for the error and warning numbers and headers for the  //
// error and warning functions                                        //
//                                                                    //
// (c) 1995 D. Bolier, A. Eliens                                      //
//                                                                    //
////////////////////////////////////////////////////////////////////////


#ifndef __ERROR_H
#define __ERROR_H


typedef enum {NDURATION,NCOUNT,EMPTY,NDELAY,ADDTOQ,EMPTYQ,HNCLMS,HMODE,PROBVAL,ERRUNIFORM,ERRNORMAL,ERRLOGNORM,ERRTRIANG,ERRGAMMA,ERRBETA,ERREXPONENT,ERRERLANG,ERRHYPGEO,NULLEV,NOHISTO,INITERR,AMODE,HFORM,ERRFILE,SAMPLE,FRACTION,LEVEL,SCAN,SUPPRESS,SEED} errortype;

typedef enum {NEGSDSTATS,LOWNUMSTATS,LT1DOF,RESOURCE,REPORT,DELAY,OUTAN,NOFIT,HWEIGHTED,HFREQUENCY,HSERIES,MAXWEIBULL,HYPEREXP,TERMINATE,SCHEDULE,WAIT,PASSIVATE,ACTIVATE,HOLD,WITHDRAW,REINSTATE} warningtype;

void err(errortype n);
void warn(warningtype n);

#endif

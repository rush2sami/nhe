////////////////////////////////////////////////////////////////////////
//                                                                    //
// reports.h                                                          //
//                                                                    //
// definitions of the report methods          			      //
//                                                                    //
// (c) 1995 D. Bolier, A. Eliens                                      //
//                                                                    //
////////////////////////////////////////////////////////////////////////


#ifndef __REPORTS_H
#define __REPORTS_H

#include <sim/event.h>
#include <sim/histogram.h>
#include <iostream.h>


class eventreport
{
public :
  eventreport(histogram* hi);
  virtual ~eventreport();
  virtual int operator()(event* e);
  char* histogramtitle()
    { return tp; }
  void reset()
    { h -> reset(); }
  friend ostream& operator<<(ostream& os,eventreport& e);

private :
  histogram* h;   // histogram to be filled
  char* tp;       // type of the event
};
  

class behaviorreport
{
public :
  behaviorreport(histogram* a,histogram* hi,int size);
  virtual ~behaviorreport();
  int operator()(double a);
  void reset()
    { h -> reset(); }
  friend ostream& operator<<(ostream& os,behaviorreport& behavior);
  histogram* analyzed()
    { return ah; }

private :
  histogram* h;           // SERIES histogram for the moving average
  histogram* ah;          // histogram to be analyzed
  int buffersize;         // size ringbuffer
  int index;              // index in ringbuffer
  double* buffer;         // ring buffer
  double movingaverage;   // moving average
};


class coutreport : public event
{
public :
  coutreport(histogram* hi,double i);
  virtual ~coutreport();
  virtual int operator()();

private :
  histogram* h;  // h is the histogram to be printed
  double intval; // intval is the interval
};


#endif

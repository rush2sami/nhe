////////////////////////////////////////////////////////////////////////
//                                                                    //
// histogram.h                                                        //
//                                                                    //
// the definition of the histogram class, it provides three           //
// different histogram types and four different forms of output       //
//                                                                    //
// (c) 1995 D. Bolier, A. Eliens                                      //
//                                                                    //
////////////////////////////////////////////////////////////////////////


#ifndef __HISTOGRAM_H
#define __HISTOGRAM_H

#include <iostream.h>
#include <fstream.h>
#include <stdio.h>

class simulation;

#include <sim/screen.h>

typedef enum {WEIGHTED,FREQUENCY,SERIES} histogramtype;

typedef enum {GRAPH,CUMULATIVE,TABLE,STATISTICS} formtype;

#ifndef HUSH
ostream& line(ostream& os);
ostream& bar(ostream& os,double value,double max);
#endif


class histogram : public screen
{
public :
  histogram(char* p,char* options = "");
  histogram(widget* w,char* p,char* options = "");
  virtual ~histogram();
  void sample(double x,double y = 1.0);
  void reset();
  ofstream* file(char* fn);
  void closefile();
  double mean();
  double variance();
  double standarddeviation();
  double sumfrequencies()
    { return sumfreq; }
  double minimum()
    { return minval; }
  double maximum()
    { return maxval; }
  
  void postscript(char* fn);

  char* name() const
    { return filename; }
  int columns()
    { return numbercolumns; }
  double startvalue()
    { return start; }
  double widthcolumn()
    { return width; }
  histogramtype histogramkind()
    { return kind; }
  void histogramkind(histogramtype k)
    { kind = k; }
  formtype histogramform()
    { return form; }
  double* values()
    { return data; }
  char* histogramtitle()
    { return title; }
  friend ostream& operator<<(ostream& os,histogram& h);

private :
  simulation* sim;           // the simulation (for the clock etc..)
  double* data;              // to record the frequencies per column
  char* title;               // title
  int first;                 // flag for first value
  double sumfreq;            // sum of frequencies (y-values)
  double sumxfreq;           // sum of x * y
  double sumxxfreq;          // sum of x^2 * y
  double width;              // width of a column
  int numbercolumns;         // number of columns
  double start;              // start value
  double minval;             // minimal x value
  double maxval;             // maximal y value
  double timelastrecording;  // time last sample was made
  histogramtype kind;        // type of the histogram
  formtype form;             // form of the histogram
  ofstream* fp;              // the file for the histogram data
  char* filename;            // and its name

#ifdef HUSH
  void graph();
  void cumulative();
  void table();
  void statistics();
  void bar(double value,double max,int y);
  char* fg;                  // color of the bars and text
#else
  friend ostream& graph(ostream& os,histogram& h);
  friend ostream& cumulative(ostream& os,histogram& h);
  friend ostream& table(ostream& os,histogram& h);
  friend ostream& statistics(ostream& os,histogram& h);
#endif

  void create(char* options);  // initialize
};


#endif

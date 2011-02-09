////////////////////////////////////////////////////////////////////////
//                                                                    //
// histogram.c							      //
//                                                                    //
// the implementation of the histogram class, it provides three       //
// different histogram types and four different forms of output       //
//                                                                    //
// (c) 1995 D. Bolier, A. Eliens                                      //
//                                                                    //
////////////////////////////////////////////////////////////////////////


#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream.h>
#include <fstream.h>
#include <math.h>

#include <sim/hush.h>
#include <sim/histogram.h>
#include <sim/simulation.h>
#include <sim/error.h>
#include <sim/reports.h>
#include <sim/converter.h>

#ifdef HUSH

// Hush constants
enum {BORDERWIDTH = 50,BARWIDTH = 40,BORDERHEIGHT = 50,BARHEIGHT = 260,TEXTHEIGHT = 20,TABLEWIDTH = 600,STATISTICSWIDTH = 400,COLUMN1 = 100,COLUMN2 = 230,COLUMN3 = 310,COLUMN4 = 410,COLUMN5 = 500};

extern kit* thekit();

#else

// ASCII constants
#define PAGEWIDTH 80
#define SCALEFACTOR 0.95

#endif

// constructor, taking the start value, the width of a column, the
// number of columns, the type, the form and the title.
// the HUSH form also takes the path of the screen widget.

histogram::histogram(char* p,char* options) : screen(p)
{
  create(options);
}

histogram::histogram(widget* w,char* p,char* options) : screen(w,p)
{
  create(options);
}


void histogram::create(char* options)
{
  sim = thesimulation();
  if (!sim)
    err(INITERR);

  // standard options
  start = 0;
  width = 1;
  numbercolumns = 10;
  kind = FREQUENCY;
  form = GRAPH;
  title = new char[BUFSIZ];
  sprintf(title,"histogram");
#ifdef HUSH
  int bg,screenwidth,screenheight,worldwidth,worldheight;

  // standard options
  bg = 0;
  fg = new char[BUFSIZ];
  sprintf(fg,"-fill black");
  screenwidth = screenheight = 0;  // to be set
#endif

  // parse options
  converter* cnv = new converter(options);
  int i = 0;
  for (i=1; i <= cnv -> argc(); i++)
  {
#ifdef HUSH
    if ((!strcmp((*cnv)[i],"-foreground")) && (i < cnv -> argc()))
      sprintf(fg,"-fill %s",(*cnv)[i+1]); 
    else if ((!strcmp((*cnv)[i],"-background")) && (i < cnv -> argc()))
      bg = i+1; 
    else if ((!strcmp((*cnv)[i],"-width")) && (i < cnv -> argc()))
      screenwidth = atoi((*cnv)[i+1]); 
    else if ((!strcmp((*cnv)[i],"-height")) && (i < cnv -> argc()))
      screenheight = atoi((*cnv)[i+1]); 
    else if
#else
    if
#endif
    ((!strcmp((*cnv)[i],"-start")) && (i < cnv -> argc()))
      start = atof((*cnv)[i+1]); 
    else if ((!strcmp((*cnv)[i],"-columnwidth")) && (i < cnv -> argc()))
      width = atof((*cnv)[i+1]); 
    else if ((!strcmp((*cnv)[i],"-columns")) && (i < cnv -> argc()))
    {
      numbercolumns = atoi((*cnv)[i+1]); 
      if (numbercolumns < 1)
        err(HNCLMS);
    }
    else if ((!strcmp((*cnv)[i],"-type")) && (i < cnv -> argc()))
      if (!strcmp((*cnv)[i+1],"frequency"))
        kind = FREQUENCY; 
      else if (!strcmp((*cnv)[i+1],"weighted"))
        kind = WEIGHTED; 
      else if (!strcmp((*cnv)[i+1],"series"))
        kind = SERIES; 
      else
        err(HMODE);
    else if ((!strcmp((*cnv)[i],"-form")) && (i < cnv -> argc()))
      if (!strcmp((*cnv)[i+1],"graph"))
        form = GRAPH; 
      else if (!strcmp((*cnv)[i+1],"cumulative"))
        form = CUMULATIVE; 
      else if (!strcmp((*cnv)[i+1],"table"))
        form = TABLE; 
      else if (!strcmp((*cnv)[i+1],"statistics"))
        form = STATISTICS;
      else
        err(HFORM);
    else if ((!strcmp((*cnv)[i],"-title")) && (i < cnv -> argc()))
      sprintf(title,(*cnv)[i+1]); 
  }

  data = new double[numbercolumns+2];
  for (i=0;i<numbercolumns+2;i++)
    data[i] = 0.0;
  first = 0;
  sumfreq = 0.0;
  sumxfreq = 0.0;
  sumxxfreq = 0.0;
  minval = 0.0;
  maxval = 0.0;
  fp = NULL;
  filename = NULL;
  timelastrecording = sim -> clock();
#ifdef HUSH
  char* buffer = new char[BUFSIZ];

  // format

  if ( (form == GRAPH) || (form == CUMULATIVE) )
  {
    worldwidth = BORDERWIDTH*2 + (numbercolumns+2)*BARWIDTH;
    worldheight = BORDERHEIGHT*2 + BARHEIGHT;

    // if not set
    if (!screenwidth)
      screenwidth = worldwidth;
    if (!screenheight)
      screenheight = worldheight;
    if (bg)
      sprintf(buffer,"-width %d -height %d -background %s -worldx %d -worldy %d",screenwidth,screenheight,(*cnv)[bg],worldwidth,worldheight);
    else
      sprintf(buffer,"-width %d -height %d -background grey -worldx %d -worldy %d",screenwidth,screenheight,worldwidth,worldheight);
    configure(buffer);
    line(0,0,worldwidth,0,fg);
    text(worldwidth/2,TEXTHEIGHT/2,title,fg);
    line(0,TEXTHEIGHT,worldwidth,TEXTHEIGHT,fg);
    text(4*BORDERWIDTH/5,4*BORDERHEIGHT/5+BARHEIGHT,"0",fg);
    line(BORDERWIDTH,BORDERHEIGHT+BARHEIGHT,BORDERWIDTH,BORDERHEIGHT,fg);
    line(BORDERWIDTH,BORDERHEIGHT+BARHEIGHT,worldwidth-BORDERWIDTH,BORDERHEIGHT+BARHEIGHT,fg);
    sprintf(buffer,"x %7.2f",width);
    text(worldwidth-2*BORDERWIDTH,worldheight-BORDERHEIGHT/4,buffer,fg);
  }
  else if (form == TABLE)
  {
    worldwidth = TABLEWIDTH;
    worldheight = TEXTHEIGHT*5+(numbercolumns+2)*TEXTHEIGHT;

    // if set
    if (!screenwidth)
      screenwidth = worldwidth;
    if (!screenheight)
      screenheight = worldheight;
    if (bg)
      sprintf(buffer,"-width %d -height %d -background %s -worldx %d -worldy %d",screenwidth,screenheight,(*cnv)[bg],worldwidth,worldheight);
    else
      sprintf(buffer,"-width %d -height %d -background grey -worldx %d -worldy %d",screenwidth,screenheight,worldwidth,worldheight);
    configure(buffer);
    line(0,0,worldwidth,0,fg);
    text(TABLEWIDTH/2,TEXTHEIGHT/2,title,fg);
    line(0,TEXTHEIGHT,worldwidth,TEXTHEIGHT,fg);
    line(COLUMN1*2,TEXTHEIGHT,COLUMN1*2,worldheight,fg);
    text(COLUMN1,TEXTHEIGHT*2,"     Range",fg);
    text(COLUMN2,TEXTHEIGHT*2,"  Observed",fg);
    text(COLUMN3,TEXTHEIGHT*2,"  Per cent",fg);
    text(COLUMN4,TEXTHEIGHT*2,"Cumulative",fg);
    text(COLUMN5,TEXTHEIGHT*2,"Cumulative",fg);
    text(COLUMN2,TEXTHEIGHT*3,"     value",fg);
    text(COLUMN3,TEXTHEIGHT*3,"  of total",fg);
    text(COLUMN4,TEXTHEIGHT*3,"percentage",fg);
    text(COLUMN5,TEXTHEIGHT*3," remainder",fg);
    line(0,TEXTHEIGHT*4,worldwidth,TEXTHEIGHT*4,fg);
  }
  else if (form == STATISTICS)
  {
    worldwidth = STATISTICSWIDTH;
    worldheight = TEXTHEIGHT*5;

    // if set
    if (!screenwidth)
      screenwidth = worldwidth;
    if (!screenheight)
      screenheight = worldheight;
    if (bg)
      sprintf(buffer,"-width %d -height %d -background %s -worldx %d -worldy %d",screenwidth,screenheight,(*cnv)[bg],worldwidth,worldheight);
    else
      sprintf(buffer,"-width %d -height %d -background grey -worldx %d -worldy %d",screenwidth,screenheight,worldwidth,worldheight);
    configure(buffer);
    line(0,0,STATISTICSWIDTH,0,fg);
    text(STATISTICSWIDTH/2,TEXTHEIGHT/2,title,fg);
    line(0,TEXTHEIGHT,STATISTICSWIDTH,TEXTHEIGHT,fg);
  }
//  delete buffer;  // NO      <----------
  delete[] buffer;  // YES     <----------
#endif
  delete cnv;
}


// destroy allocated memory

histogram::~histogram()
{
  if (data)
//    delete data;      // NO     <-----------
    delete[] data;      // YES    <-----------
  if (title)
//    delete title;     // NO     <-----------
    delete[] title;     // YES    <-----------
  if (fp)
    fp -> close();
#ifdef HUSH
  if (fg)
    delete fg;
#endif
}
  

// add a sample, adjust internal data according to the type

void histogram::sample(double x,double y)
{
  // first determine the column x falls in
  int i,j;
  if (kind == SERIES)
    i = (int)(( sim -> clock() - start ) / width  + 1);
  else
    i = (int)(( x - start ) / width  + 1);
  if (i < 0) 
    i = 0;
  else if (i > numbercolumns+1) 
    i = numbercolumns+1;

  // adjust minimal and maximal values
  if (!first)
  {
    minval = x;
    maxval = x;
    first++;
  }
  else
  {
    if (x < minval) 
      minval = x;
    if (x > maxval) 
      maxval = x;
  }

  // add sample according to the type
  switch(kind)
  {
    case WEIGHTED :
      // take the time y persisted into account
      y = (sim -> clock() - timelastrecording) * y;
      data[i] += y;
      sumfreq += y;
      break;
    case FREQUENCY :
      // just add y
      data[i] += y;
      sumfreq += y;
      break;
    case SERIES :
      // first determine the last column a sample was put in
      j = (int)(( timelastrecording - start ) / width  + 1);
      if (j < 0) 
        j = 0;
      else if (j > numbercolumns+1) 
        j = numbercolumns+1;
      // first column has nothing in it, so :
      if (i > j) 
        j++;
      // put y in the columns between the last and the current
      while (j <= i) 
      {
        data[j++] += y;
        sumfreq += y;
      }
      break;
  }

  // if fp write x and y to fp
  if (fp)
  {
    char* buffer = new char[BUFSIZ];
    sprintf(buffer,"%f\n",x);
    (*fp) << buffer;
    sprintf(buffer,"%f\n",y);
    (*fp) << buffer;
//  delete buffer;  // NO      <----------
    delete[] buffer;  // YES     <----------
  }

  // adjust data for SERIES and WEIGHTED histograms
  timelastrecording = sim -> clock();

  // for calculating mean and variance
  sumxfreq += x * y;
  sumxxfreq += (x * x * y);

  // if it has a behaviorreport, sample the mean
  behaviorreport* std;
  if ((std = sim -> findbehaviorreport(this)) != NULL)
    (*std)(mean());
}


// write samples to specified file

ofstream* histogram::file(char* fn)
{
  if (!(fp = new ofstream(fn)))
    err(ERRFILE);
  cout << "Created file - " << fn << endl;
  filename = fn;
  return fp;
}


// close the file

void histogram::closefile()
{
  if (fp)
    fp -> close();
}


// generate a postscript of the screen

void histogram::postscript(char* fn)
{
#ifdef HUSH
  postscript(fn);
#else
  fn = fn;
#endif
}


// reset the histogram as if no data were recorded

void histogram::reset()
{  
  if (data)
    delete[] data;
  data = new double[numbercolumns+2];
#ifdef HUSH
  // clear the screen (the tag is the path)
  del(path());
#endif
  for (int i=0;i<numbercolumns+2;i++)
    data[i] = 0.0;
  first = 0;
  sumfreq = 0.0;
  sumxfreq = 0.0;
  sumxxfreq = 0.0;
  minval = 0.0;
  maxval = 0.0;
  timelastrecording = sim -> clock();
}


// the mean = sum (x*f) / sum f

double histogram::mean()
{
  if (sumfreq > 0.0)
    return (sumxfreq/sumfreq);
  else
    return 0.0;
}


// the variance = sum (x*x*f) / sum f - mean^2 or :

double histogram::variance()
{
  if (sumfreq > 1.0)
  {
    double x = sumxxfreq - sumxfreq * mean(); 
    x = x / (sumfreq-1.0);
    return x;
  }
  else
    return 0.0;
}


// the square root of the variance

double histogram::standarddeviation()
{
  return (sqrt(variance()));
}


// overloaded operator<<, prints the histogram depending on the output 
// form and type and compiler directives

ostream& operator<<(ostream& os,histogram& h)
{
#ifdef HUSH
  kit* tk = thekit();
#endif
  switch (h.form)
  {
    case GRAPH :
#ifdef HUSH
      // clear the screen, the tag is the path of the screen widget
      h.del(h.path());
      h.graph();
      tk -> update();
#else
      graph(os,h);
#endif
      break;
    case CUMULATIVE :
#ifdef HUSH
      // clear the screen, the tag is the path of the screen widget
      h.del(h.path());
      h.cumulative();
      tk -> update();
#else
      cumulative(os,h);
#endif
      break;
    case TABLE :
#ifdef HUSH
      // clear the screen, the tag is the path of the screen widget
      h.del(h.path());
      h.table();
      tk -> update();
#else
      table(os,h);
#endif
      break;
    case STATISTICS :
#ifdef HUSH
      // clear the screen, the tag is the path of the screen widget
      h.del(h.path());
      h.statistics();
      tk -> update();
#else
      statistics(os,h);
#endif
      break;
  }
  return os;
}


#ifdef HUSH

// as a frequency graph

void histogram::graph()
{
  int worldwidth = BORDERWIDTH*2 + (numbercolumns+2)*BARWIDTH;
  int worldheight = BORDERHEIGHT*2 + BARHEIGHT;

  int id;
  if (sumfreq <= 0.0)
  {
    id = text(worldwidth/2,worldheight/2,"no histogram data recorded",fg);
    tag(id,path());
    return;
  }
 
  // first determine the maximal column value
  double max;
  int i; 
  for (max = 0.0,i = 0;i < numbercolumns+2;i++)
  {
    if (max < data[i])
      max = data[i];
  }
 
  char* buffer = new char[BUFSIZ];
  sprintf(buffer,"%7.1f",max);
  id = text(BORDERWIDTH/2,BORDERHEIGHT,buffer,fg);
  tag(id,path());
 
  int x = BORDERWIDTH-BARWIDTH;

  // underflow
  if (data[0] > 0.0)
  {
    x = BORDERWIDTH;
    id = text(x,worldheight-BORDERHEIGHT/4*3,"u",fg);
    tag(id,path());
    bar(data[0],max,x+BARWIDTH/2);
  }
 
  // get for each column a bar
  for (i=1;i < numbercolumns+1;i++)
  {
    sprintf(buffer,"%d",(int)(start/width+i-1));
    id = text(x+i*BARWIDTH,worldheight-BORDERHEIGHT/4*3,buffer,fg);
    tag(id,path());
    bar(data[i],max,x+BARWIDTH/2+i*BARWIDTH);
  }
  sprintf(buffer,"%d",(int)(start/width+i-1));
  id = text(x+i*BARWIDTH,worldheight-BORDERHEIGHT/4*3,buffer,fg);
  tag(id,path());
 
  // overflow
  if (data[numbercolumns + 1] > 0.0)
  {
    id = text(x+(i+1)*BARWIDTH,worldheight-BORDERHEIGHT/4*3,"o",fg);
    tag(id,path());
    bar(data[numbercolumns+1],max,x+BARWIDTH/2+i*BARWIDTH);
  }
 
//  delete buffer;  // NO      <----------
  delete[] buffer;  // YES     <----------
}


// puts the histogram as a cumulative frequency on the screen 
 
void histogram::cumulative()
{
  int worldwidth = BORDERWIDTH*2 + (numbercolumns+2)*BARWIDTH;
  int worldheight = BORDERHEIGHT*2 + BARHEIGHT;
  int id;
  if (sumfreq <= 0.0)
  {
    id = text(worldwidth/2,worldheight/2,"no histogram data recorded",fg);
    tag(id,path());
    return;
  }
 
  // the maximal column value is the sum of frequencies here
  double sum = sumfreq;
 
  char* buffer = new char[BUFSIZ];
  sprintf(buffer,"%7.1f",sum);
  id = text(BORDERWIDTH/2,BORDERHEIGHT,buffer,fg);
  tag(id,path());
 
  int x = BORDERWIDTH-BARWIDTH;

  // underflow
  double cum = data[0];
  if (data[0] > 0.0)
  {
    x = BORDERWIDTH;
    id = text(x,worldheight-BORDERHEIGHT/4*3,"u",fg);
    tag(id,path());
    bar(cum,sum,x+BARWIDTH/2);
  }
 
  // print bars, just add each column value to cum
  int i;
  for (i = 1;i < numbercolumns+1;i++)
  {
    cum += data[i];
    sprintf(buffer,"%d",(int)(start/width+i-1));
    id = text(x+i*BARWIDTH,worldheight-BORDERHEIGHT/4*3,buffer,fg);
    tag(id,path());
    bar(cum,sum,x+BARWIDTH/2+i*BARWIDTH);
  }
  sprintf(buffer,"%d",(int)(start/width+i-1));
  id = text(x+i*BARWIDTH,worldheight-BORDERHEIGHT/4*3,buffer,fg);
  tag(id,path());
 
  // overflow
  if (data[numbercolumns + 1] > 0.0)
  {
    cum += data[numbercolumns+1];
    id = text(x+(i+1)*BARWIDTH,worldheight-BORDERHEIGHT/4*3,"o",fg);
    tag(id,path());
    bar(cum,sum,x+BARWIDTH/2+i*BARWIDTH);
  }
 
//  delete buffer;  // NO      <----------
  delete[] buffer;  // YES     <----------
}


// puts the histogram as a table on the screen

void histogram::table()
{
  char* buffer = new char[BUFSIZ];
  int id;
  if (sumfreq < 1)
  {
    id = text(TABLEWIDTH/2,3*TEXTHEIGHT,"no histogram data recorded",fg);
    tag(id,path());
    return;
  }

  // cum for recording the cumulative value
  double cum = 0.0;
  double sum = sumfreq;

  int x = 5*TEXTHEIGHT;

  // underflow
  if (data[0] > 0.0)
  {
    sprintf(buffer,"         -> %8.2f",start);
    id = text(COLUMN1,x,buffer,fg);
    tag(id,path());
    cum = data[0];
    sprintf(buffer,"     %10.2f",data[0] );
    id = text(COLUMN2,x,buffer,fg);
    tag(id,path());
    sprintf(buffer,"   %10.2f",(100.0*data[0]/sum) );
    id = text(COLUMN3,x,buffer,fg);
    tag(id,path());
    sprintf(buffer,"   %10.2f",(100.0*cum/sum) );
    id = text(COLUMN4,x,buffer,fg);
    tag(id,path());
    sprintf(buffer,"   %10.2f",(100.0*(sum-cum)/sum) );
    id = text(COLUMN5,x,buffer,fg);
    tag(id,path());
  }

  // put the columns in the ostream
  int i;
  for (i=1;i < (numbercolumns+1);i++)
  {
    sprintf(buffer,"%8.2f -> %8.2f",start+((i-1)*width),start+(i*width));
    id = text(COLUMN1,x+i*TEXTHEIGHT,buffer,fg);
    tag(id,path());
    cum += data[i];
    sprintf(buffer,"     %10.2f",data[i] );
    id = text(COLUMN2,x+i*TEXTHEIGHT,buffer,fg);
    tag(id,path());
    sprintf(buffer,"   %10.2f",(100.0*data[i]/sum) );
    id = text(COLUMN3,x+i*TEXTHEIGHT,buffer,fg);
    tag(id,path());
    sprintf(buffer,"   %10.2f",(100.0*cum/sum) );
    id = text(COLUMN4,x+i*TEXTHEIGHT,buffer,fg);
    tag(id,path());
    sprintf(buffer,"   %10.2f",(100.0*(sum-cum)/sum) );
    id = text(COLUMN5,x+i*TEXTHEIGHT,buffer,fg);
    tag(id,path());
    if (cum >= sum)
    {
      id = text(2*TABLEWIDTH/3,x+(i+1)*TEXTHEIGHT," Remaining frequencies are all zero",fg);
      tag(id,path());
      return;
    }
  }

  // overflow
  if (data[(numbercolumns)+1] > 0.0)
  {
    sprintf(buffer,"%8.2f ->         ",start+((numbercolumns+1)*width));
    id = text(COLUMN1,x+i*TEXTHEIGHT,buffer,fg);
    tag(id,path());
    cum += data[numbercolumns+1];
    sprintf(buffer,"     %10.2f",data[numbercolumns+1] );
    id = text(COLUMN2,x+i*TEXTHEIGHT,buffer,fg);
    tag(id,path());
    sprintf(buffer,"   %10.2f",(100.0*data[numbercolumns+1]/sum) );
    id = text(COLUMN3,x+i*TEXTHEIGHT,buffer,fg);
    tag(id,path());
    sprintf(buffer,"   %10.2f",(100.0*cum/sum) );
    id = text(COLUMN4,x+i*TEXTHEIGHT,buffer,fg);
    tag(id,path());
    sprintf(buffer,"   %10.2f",(100.0*(sum-cum)/sum) );
    id = text(COLUMN5,x+i*TEXTHEIGHT,buffer,fg);
    tag(id,path());
  }
//  delete buffer;  // NO      <----------
  delete[] buffer;  // YES     <----------
}


// puts the mean and variance, the sum of frequencies and the minimum
// and maximum the screen

void histogram::statistics()
{
  char* buffer = new char[BUFSIZ];
  int id;
  if (sumfreq < 1)
  {
    id = text(STATISTICSWIDTH/2,3*TEXTHEIGHT,"no histogram data recorded",fg);
    tag(id,path());
    return;
  }
  sprintf(buffer,"mean = %10.3f",mean());
  id = text(STATISTICSWIDTH/2,TEXTHEIGHT*3/2,buffer,fg);
  tag(id,path());
  if (sumfreq > 1.0)
  {
    double x = variance();
    sprintf(buffer,"variance = %10.3f, std dev = %10.3f",x,sqrt(x));
  }
  else
    sprintf(buffer,"variance = UNDEFINED, std dev = UNDEFINED");
  id = text(STATISTICSWIDTH/2,TEXTHEIGHT*5/2,buffer,fg);
  tag(id,path());
  sprintf(buffer,"sum of frequencies             = %5.0f",sumfreq);
  id = text(STATISTICSWIDTH/2,TEXTHEIGHT*7/2,buffer,fg);
  tag(id,path());
  sprintf(buffer,"min val = %10.3f,   max val = %10.3f",minval,maxval);
  id = text(STATISTICSWIDTH/2,TEXTHEIGHT*9/2,buffer,fg);
  tag(id,path());
//  delete buffer;  // NO      <----------
  delete[] buffer;  // YES     <----------
}


// puts column y with given value as a bar on the screen

void histogram::bar(double value,double max,int y)
{
  if (value)
  {
    // the bar, adjusted to the maximal column value
    int id = rectangle(y-(BARWIDTH/2-1),BORDERHEIGHT+BARHEIGHT,y+(BARWIDTH/2-1),BORDERHEIGHT+(int)((max-value)/max*BARHEIGHT),fg);
    tag(id,path());
  }
}


#else // ASCII


// returns the histogram as a frequency graph

ostream& graph(ostream& os,histogram& h)
{
  line(os);
  os << h.title << endl;
  if (h.sumfreq <= 0.0)
  {
    os << "no histogram data recorded" << endl;
    return os;
  }

  // first determine the maximal column value
  double max = 0.0;
  int i = 0;
  for (i = 0;i < h.numbercolumns+2;i++)
  {
    if (max < h.data[i])
      max = h.data[i];
  }

  char* buffer = new char[BUFSIZ];

  // underflow
  if (h.data[0] > 0.0)
  {
    os << endl << "under  ";
    bar(os,h.data[0],max);
  }

  // get for each column a bar
  for (i = 1;i < h.numbercolumns+1;i++)
  {
    sprintf(buffer,"%7.2f",h.start+((i - 1) * h.width));
    os << endl << buffer;
    bar(os,h.data[i],max);
  }

  // overflow
  if (h.data[h.numbercolumns + 1] > 0.0)
  {
    os << endl << "over   ";
    bar(os,h.data[h.numbercolumns+1],max);
  }

//  delete buffer;  // NO      <----------
  delete[] buffer;  // YES     <----------
  os << endl;
  return os;
}


// returns the histogram as a cumulative frequency graph
 
ostream& cumulative(ostream& os,histogram& h)
{
  line(os);
  os << h.title << endl;
 
  if (h.sumfreq < 1)
  {
    os << "no histogram data recorded" << endl;
    return os;
  }
 
  // the maximal column value is the sum of frequencies here
  double sum = h.sumfreq;

  char* buffer = new char[BUFSIZ];

  // underflow
  double cum = h.data[0];
  if (cum > 0.0)
  {
    os << endl << "under  ";
    bar(os,cum,sum);
  }

  // print bars, just add each column value to cum
  for (int i = 1;i < h.numbercolumns+1;i++)
  {
    cum += h.data[i];
    sprintf(buffer,"%7.2f",h.start+((i - 1) * h.width));
    os << endl << buffer;
    bar(os,cum,sum);
  }

  // overflow
  if (h.data[h.numbercolumns + 1] > 0.0)
  {
    cum += h.data[h.numbercolumns+1];
    os << endl << "over   ";
    bar(os,cum,sum);
  }
//  delete buffer;  // NO      <----------
  delete[] buffer;  // YES     <----------
  os << endl;
  return os;
}


// puts the histogram as a table in the output stream

ostream& table(ostream& os,histogram& h)
{
  char* buffer = new char[BUFSIZ];
  line(os);
  os << h.title << endl;
  if (h.sumfreq < 1)
  {
    os << "no histogram data recorded" << endl;
    return os;
  }

  // cum for recording the cumulative value
  double cum = 0.0;
  double sum = h.sumfreq;

  // put header in the ostream
  sprintf(buffer,"        Range               Observed     Per cent    Cumulative   Cumulative");
  os << endl << buffer << endl;
  sprintf(buffer,"                            value        of total    percentage   remainder");
  os << buffer << endl << endl;

  // underflow
  if (h.data[0] > 0.0)
  {
    sprintf(buffer,"         -> %8.2f",h.start);
    os << buffer;
    cum = h.data[0];
    sprintf(buffer,"     %10.2f",h.data[0] );
    os << buffer;
    sprintf(buffer,"   %10.2f",(100.0*h.data[0]/sum) );
    os << buffer;
    sprintf(buffer,"   %10.2f",(100.0*cum/sum) );
    os << buffer;
    sprintf(buffer,"   %10.2f",(100.0*(sum-cum)/sum) );
    os << buffer << endl;
  }

  // put the columns in the ostream
  for (int i=1;i < (h.numbercolumns+1);i++)
  {
    sprintf(buffer,"%8.2f -> %8.2f",h.start+((i-1)*h.width),h.start+(i*h.width)) ;
    os << buffer;
    cum += h.data[i];
    sprintf(buffer,"     %10.2f",h.data[i] );
    os << buffer;
    sprintf(buffer,"   %10.2f",(100.0*h.data[i]/sum) );
    os << buffer;
    sprintf(buffer,"   %10.2f",(100.0*cum/sum) );
    os << buffer;
    sprintf(buffer,"   %10.2f",(100.0*(sum-cum)/sum) );
    os << buffer << endl;
    if (cum >= sum)
    {
      os << " Remaining frequencies are all zero" << endl;
      return os;
    }
  }

  // overflow
  if (h.data[(h.numbercolumns)+1] > 0.0)
  {
    sprintf(buffer,"%8.2f ->         ",h.start+((h.numbercolumns+1)*h.width));
    os << buffer;
    cum += h.data[h.numbercolumns+1];
    sprintf(buffer,"     %10.2f",h.data[h.numbercolumns+1] );
    os << buffer;
    sprintf(buffer,"   %10.2f",(100.0*h.data[h.numbercolumns+1]/sum) );
    os << buffer;
    sprintf(buffer,"   %10.2f",(100.0*cum/sum) );
    os << buffer;
    sprintf(buffer,"   %10.2f",(100.0*(sum-cum)/sum) );
    os << buffer << endl;
  }
//  delete buffer;  // NO      <----------
  delete[] buffer;  // YES     <----------
  return os;
}


// puts the mean and variance, the sum of frequencies and the minimum
// and maximum in the output stream

ostream& statistics(ostream& os,histogram& h)
{
  char* buffer = new char[BUFSIZ];
  line(os);
  os << h.title << endl;
  if (h.sumfreq < 1)
  {
    os << "no histogram data recorded" << endl;
    return os;
  }
  double mn = h.mean();
  sprintf(buffer,"mean = %10.3f",mn);
  os << endl << buffer;
  if (h.sumfreq > 1.0)
  {
    double x = h.variance();
    sprintf(buffer,"variance = %10.3f, std dev = %10.3f",x,sqrt(x));
  }
  else
    sprintf(buffer,"variance = UNDEFINED, std dev = UNDEFINED",mn);
  os << ", " << buffer << endl;
  sprintf(buffer,"sum of frequencies             = %5.0f",h.sumfreq);
  os << buffer << endl;
  sprintf(buffer,"min val = %10.3f,   max val = %10.3f",h.minval,h.maxval);
  os << buffer << endl;
//  delete buffer;  // NO      <----------
  delete[] buffer;  // YES     <----------
  return os;
}


// puts the column as a bar in the output stream

ostream& bar(ostream& os,double value,double max)
{
  // first the value
  char* buffer = new char[BUFSIZ];
  sprintf(buffer," %7.2f",value);
  os << buffer;

  // then the bar, adjusted to the maximal column value
  int stars = (int)((PAGEWIDTH - 15) * value * SCALEFACTOR / max);
  for (int i = 0;i < stars;i++)
    os << "#";

//  delete buffer;  // NO      <----------
  delete[] buffer;  // YES     <----------
  return os;
}


// puts a line in the output stream

ostream& line(ostream& os)
{
  os << "-------------------------------------------------------------------" << endl;
  return os;
}


#endif

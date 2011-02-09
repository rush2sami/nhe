////////////////////////////////////////////////////////////////////////
//                                                                    //
// simulationclock.c                                                  //
//                                                                    //
// implementation of the clock class                                  //
//                                                                    //
// (c) 1995 D. Bolier, A. Eliens                                      //
//                                                                    //
////////////////////////////////////////////////////////////////////////

#ifdef HUSH

#include <sim/hush.h>
#include <sim/defs.h>
#include <stdlib.h>

enum {CLOCKWIDTH = 200,CLOCKHEIGHT = 60};

extern kit* thekit();

#else

#include <iostream.h>

#endif

#include <sim/simulation.h>
#include <sim/error.h>
#include <sim/clock.h>
#include <sim/converter.h>

// create the clock widget, the create method parses the options and
// suites them for the clock

#ifdef HUSH
simulationclock::simulationclock(char* p,char* options) : screen(p)
{
  create(options);
}
#else
simulationclock::simulationclock(char* p,char* options) : screen(p)
{
  options = options;
}
#endif


#ifdef HUSH
simulationclock::simulationclock(widget* w,char* p,char* options) : screen(w,p)
{
  create(options);
}
#else
simulationclock::simulationclock(widget* w,char* p,char* options) : screen(w,p)
{
  options = options;
}
#endif


#ifdef HUSH

void simulationclock::create(char* options)
{
  char* buffer = new char[BUFSIZ];
  int fg,bg;
  fg = bg = 0;  // standard options
  int wd = CLOCKWIDTH;
  int hg = CLOCKHEIGHT;

  converter* cnv = new converter(options);
  for (int i=1; i <= cnv -> argc(); i++)
  {
    if ((!strcmp((*cnv)[i],"-foreground")) && (i < cnv -> argc()))
      fg = i+1;
    else if ((!strcmp((*cnv)[i],"-background")) && (i < cnv -> argc()))
      bg = i+1;
    else if ((!strcmp((*cnv)[i],"-width")) && (i < cnv -> argc()))
      wd = atoi((*cnv)[i+1]);
    else if ((!strcmp((*cnv)[i],"-height")) && (i < cnv -> argc()))
      hg = atoi((*cnv)[i+1]);
  }

  // screen options
  if (bg)
    sprintf(buffer,"-width %d -height %d -worldx %d -worldy %d -background %s",wd,hg,CLOCKWIDTH,CLOCKHEIGHT,(*cnv)[bg]);
  else
    sprintf(buffer,"-width %d -height %d -worldx %d -worldy %d -background blue",wd,hg,CLOCKWIDTH,CLOCKHEIGHT);
  configure(buffer);
 
  if (fg)
    sprintf(buffer,"-fill %s",(*cnv)[fg]);
  else
    sprintf(buffer,"-fill white");
  text(CLOCKWIDTH/2,CLOCKHEIGHT/6,"clock",buffer);
  line(0,CLOCKHEIGHT/3,CLOCKWIDTH,CLOCKHEIGHT/3,buffer);

  foreground = new char[BUFSIZ];
  if (fg)
    sprintf(foreground,"-fill %s -font -Adobe-helvetica-medium-r-normal--*-%d*",(*cnv)[fg],240*hg/CLOCKHEIGHT);
  else
    sprintf(foreground,"-fill white -font -Adobe-helvetica-medium-r-normal--*-%d*",240*hg/CLOCKHEIGHT);

  char* cc = new char[BUFSIZ];
  sprintf(cc,"%10.2f",0.00);
  int id = text(6*CLOCKWIDTH/10,2*CLOCKHEIGHT/3,cc,foreground);
  tag(id,"clock");
  delete cc;
  
  // give pointer to the simulation object
  simulation* sim = thesimulation();
  if (!sim)
    err(INITERR);
  sim -> setclock(this);

  delete buffer;
  delete cnv;
}

#endif


simulationclock::~simulationclock()
{
#ifdef HUSH
  delete foreground;
#endif
}


// updates the clock, (not yet implemented as operator()(double))

void simulationclock::setclock(double cl)
{
#ifdef HUSH
  del("clock");
  char* cc = new char[BUFSIZ];
  sprintf(cc,"%10.2f",cl);
  int id = text(6*CLOCKWIDTH/10,2*CLOCKHEIGHT/3,cc,foreground);
  tag(id,"clock");
  thekit() -> update();
  delete cc;
#else
  cout << "clock : " << cl << endl;
#endif
}

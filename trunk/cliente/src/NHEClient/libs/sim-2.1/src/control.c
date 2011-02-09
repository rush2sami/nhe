////////////////////////////////////////////////////////////////////////
//                                                                    //
// control.c                                                          //
//                                                                    //
// implementation of the control class                                //
//                                                                    //
// (c) 1995 D. Bolier, A. Eliens                                      //
//                                                                    //
////////////////////////////////////////////////////////////////////////

#ifdef HUSH

#include <sim/hush.h>
#include <sim/defs.h>
#include <sim/handlers.h>
#include <stdlib.h>

enum {CONTROLWIDTH = 180,CONTROLHEIGHT = 120};

extern kit* thekit();

#else

#include <iostream.h>

#endif

#include <sim/simulation.h>
#include <sim/error.h>
#include <sim/control.h>
#include <sim/converter.h>

// create the contol widget, the create method parses the options and
// suites them for the screen

#ifdef HUSH
control::control(char* p,char* options) : screen(p)
{
  create(options);
}
#else
control::control(char* p,char* options) : screen(p)
{
  options = options;
}
#endif


#ifdef HUSH
control::control(widget* w,char* p,char* options) : screen(w,p)
{
  create(options);
}
#else
control::control(widget* w,char* p,char* options) : screen(w,p)
{
  options = options;
}
#endif


#ifdef HUSH

void control::create(char* options)
{
  // standard options
  int fg,bg,wg;
  fg = bg = wg = 0;
  int wd = CONTROLWIDTH;
  int hg = CONTROLHEIGHT;
  double fct = 1;
 
  // parse options
  converter* cnv = new converter(options);
  for (int i=1; i <= cnv -> argc(); i++)
  {
    if ((!strcmp((*cnv)[i],"-foreground")) && (i < (*cnv).argc()))
      fg = i+1;
    else if ((!strcmp((*cnv)[i],"-background")) && (i < (*cnv).argc()))
      bg = i+1;
    else if ((!strcmp((*cnv)[i],"-factor")) && (i < (*cnv).argc()))
      fct = atof((*cnv)[i+1]);
    else if ((!strcmp((*cnv)[i],"-widgets")) && (i < (*cnv).argc()))
      wg = i+1;
    else if ((!strcmp((*cnv)[i],"-width")) && (i < (*cnv).argc()))
      wd = atoi((*cnv)[i+1]);
    else if ((!strcmp((*cnv)[i],"-height")) && (i < (*cnv).argc()))
      hg = atoi((*cnv)[i+1]);
  }

  // foreground option
  foreground = new char[BUFSIZ];
  if (fg)
    sprintf(foreground,"-fill %s",(*cnv)[fg]);
  else
    sprintf(foreground,"-fill white");
 
  // screen options
  char* buffer = new char[BUFSIZ];
  if (bg)
    sprintf(buffer,"-width %d -height %d -worldx %d -worldy %d -background %s",wd,hg,CONTROLWIDTH,CONTROLHEIGHT,(*cnv)[bg]);
  else
    sprintf(buffer,"-width %d -height %d -worldx %d -worldy %d -background grey",wd,hg,CONTROLWIDTH,CONTROLHEIGHT);
  configure(buffer);

  // the start stop button
  if (fg && wg)
    sprintf(buffer,"-background %s -foreground %s",(*cnv)[wg],(*cnv)[fg]);
  else if (fg)
    sprintf(buffer,"-background blue -foreground %s",(*cnv)[fg]);
  else if (wg)
    sprintf(buffer,"-background %s -foreground white",(*cnv)[wg]);
  else
    sprintf(buffer,"-background blue -foreground white",(*cnv)[wg]);
  button* b = new button(this,".button",buffer);
  bh = new buttonhandler();
  b -> bind(bh);
  b -> text("start/stop");
  window(CONTROLWIDTH/4,CONTROLHEIGHT/4,b);

  // the quit button
  if (fg && wg)
    sprintf(buffer,"-background %s -foreground %s",(*cnv)[wg],(*cnv)[fg]);
  else if (fg)
    sprintf(buffer,"-background blue -foreground %s",(*cnv)[fg]);
  else if (fg)
    sprintf(buffer,"-background %s -foreground white",(*cnv)[wg]);
  else
    sprintf(buffer,"-background blue -foreground white",(*cnv)[fg]);
  button* q = new button(this,".quit",buffer);
  quithandler* qh = new quithandler(this);
  q -> bind(qh);
  q -> text("quit");
  window(5*CONTROLWIDTH/8,CONTROLHEIGHT/4,q);

  // the scale widget and its handler
  scale* s = new scale(this,".scale");
  if (fg && wg)
    sprintf(buffer,"-background %s -foreground %s -label speed -orient horizontal -length %d",(*cnv)[wg],(*cnv)[fg],7*wd/8);
  else if (fg)
    sprintf(buffer,"-background blue -foreground %s -label speed -orient horizontal -length %d",(*cnv)[fg],7*wd/8);
  else if (wg)
    sprintf(buffer,"-background %s -foreground white -label speed -orient horizontal -length %d",(*cnv)[wg],7*wd/8);
  else
    sprintf(buffer,"-background blue -foreground white -label speed -orient horizontal -length %d",7*wd/8);
  s -> configure(buffer);
  s -> set(100);  // maximum speed
  sh = new scalehandler(s,fct);
  s -> bind("<Any-ButtonRelease>",sh);
  window(CONTROLWIDTH/2,2*CONTROLHEIGHT/3,s);

  // the exit button
  if (fg && wg)
    sprintf(buffer,"-background %s -foreground %s",(*cnv)[wg],(*cnv)[fg]);
  else if (fg)
    sprintf(buffer,"-background blue -foreground %s",(*cnv)[fg]);
  else if (fg)
    sprintf(buffer,"-background %s -foreground white",(*cnv)[wg]);
  else
    sprintf(buffer,"-background blue -foreground white",(*cnv)[fg]);
  button* e = new button(this,".exit",buffer);
  exithandler* eh = new exithandler(this);
  e -> bind(eh);
  e -> text("exit");
  window(7*CONTROLWIDTH/8,CONTROLHEIGHT/4,e);

  // give pointer to the simulation object
  simulation* sim = thesimulation();
  if (!sim)
    err(INITERR);
  sim -> setcontrol(this);

  delete buffer;
  delete cnv;
}

#endif


// destructor

control::~control()
{
#ifdef HUSH
  delete sh;
  delete bh;
  delete foreground;
#endif
}


// sets the speed factor

void control::factor(double fct)
{
#ifdef HUSH
  sh -> factor(fct);
#endif
}


// reset the widgets

void control::reset()
{
#ifdef HUSH
  bh -> check(OK);
  thekit() -> update();
#endif
}


#ifdef HUSH

// get the start/stop flag

int control::check()
{
  thekit() -> update();
  return bh -> check();
}


// return the current speed

double control::speed()
{
  thekit() -> update();
  return sh -> speed();
}

#endif

////////////////////////////////////////////////////////////////////////
//                                                                    //
// screen.c                                                           //
//                                                                    //
// implementation of the screen class, it is identical to the canvas  //
// except for the use of world coordinates.                           //
//                                                                    //
// (c) 1995 D. Bolier, A. Eliens                                      //
//                                                                    //
////////////////////////////////////////////////////////////////////////

#ifdef HUSH

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sim/hush.h>
#include <sim/simulation.h>
#include <sim/error.h>
#include <sim/defs.h>
#include <sim/converter.h>

//extern "C" int strcat(...);


#else

#include <iostream.h>

#endif

#include <sim/screen.h>

// screen constructors, the parse method derives the arguments from
// the options

#ifdef HUSH
screen::screen(const char* p, const char* options) // AE : canvas(p,0)
			: widget(p,0)
{
  c = new canvas(p,parse(options));
  redirect(c); _register(c);
#else
screen::screen(const char* p, const char* options) : widget(p)
{
  options = options;
#endif
}

#ifdef HUSH
screen::screen(const widget* w, const char* p, const char* options) 
		: widget(w,p,0) // : canvas(w,p,0)
{
  c = new canvas(w,p,parse(options));
  redirect(c); _register(c);
#else
screen::screen(const widget* w, const char* p, const char* options) : widget(w,p)
{
  options = options;
#endif
}

#ifdef HUSH

// derives the parameters and returns the options for the canvas
// Tk is set

char* screen::parse(const char* options)
{
  // standard options
  int bg = 0,width,height,world_x,world_y;
  width = world_x = 800;
  height = world_y = 600;
  factor = 1.0;
 
  // parse options
  converter* cnv = new converter(options);
  for (int i=1; i <= cnv -> argc(); i++)
  {
    if ((!strcmp((*cnv)[i],"-background")) && (i < cnv -> argc()))
      bg = i+1; 
    else if ((!strcmp((*cnv)[i],"-width")) && (i < cnv -> argc()))
      width = atoi((*cnv)[i+1]);
    else if ((!strcmp((*cnv)[i],"-height")) && (i < cnv -> argc()))
      height = atoi((*cnv)[i+1]);
    else if ((!strcmp((*cnv)[i],"-worldx")) && (i < cnv -> argc()))
      world_x = atoi((*cnv)[i+1]);
    else if ((!strcmp((*cnv)[i],"-worldy")) && (i < cnv -> argc()))
      world_y = atoi((*cnv)[i+1]);
  }

  // right scale ?
  if ((double)width/(double)world_x == (double)height/(double)world_y)
    factor = (double)width/(double)world_x;
  else
    err(FRACTION);

  // construct the font
  font = new char[BUFSIZ];
  sprintf(font,"-font -Adobe-Helvetica-Bold-R-Normal--*-%d-* ",(int)(factor*120));

  // canvas options
  char* buffer = new char[BUFSIZ];
  if (bg)
    sprintf(buffer,"-width %d -height %d -background %s",width,height,(*cnv)[bg]);
  else
    sprintf(buffer,"-width %d -height %d -background grey",width,height);
  delete cnv;
  return buffer;
}

#endif


// destructor

screen::~screen()
{
#ifdef HUSH
  delete c;
  delete font;
#endif
}


#ifdef HUSH

// configure options

void screen::configure(const char* options)
{
  c -> configure(parse(options));
}

#endif


// add a line to the screen.

int screen::line(int x1,int y1,int x2,int y2,char* options)
{
#ifdef HUSH
  int p1 = (int)(factor * x1);
  int q1 = (int)(factor * y1);
  int p2 = (int)(factor * x2);
  int q2 = (int)(factor * y2);
  return ((int)c -> line(p1,q1,p2,q2,options));
#else
  x1 = x1;
  y1 = y1;
  x2 = x2;
  y2 = y2;
  options = options;
  return 0;
#endif
}

int screen::line(char* linespec,char* options)
{
#ifdef HUSH
  int i=0;
  char* linesp = new char[BUFSIZ];
  while (i < strlen(linespec))
  {
    linesp[i] = (int)(linespec[i] * factor);
    i++;
  }
  int id = (int)c -> line(linesp,options);
  delete linesp;
  return id;
#else
  linespec = linespec;
  options = options;
  return 0;
#endif
}


// add an oval to the screen.

int screen::oval(int x1,int y1,int x2,int y2,char* options)
{
#ifdef HUSH
  int p1 = (int)(factor * x1);
  int q1 = (int)(factor * y1);
  int p2 = (int)(factor * x2);
  int q2 = (int)(factor * y2);
  return ((int)c -> oval(p1,q1,p2,q2,options));
#else
  x1 = x1;
  y1 = y1;
  x2 = x2;
  y2 = y2;
  options = options;
  return 0;
#endif
}


// add a circle to the screen.

int screen::circle(int x,int y,int r,char* options)
{
#ifdef HUSH
  int x1 = (int)(factor * x);
  int y1 = (int)(factor * y);
  int r1 = (int)(factor * r);
  return ((int)c -> oval(x1-r1,y1-r1,x1+r1,y1+r1,options));
#else
  x = x;
  y = y;
  r = r;
  options = options;
  return 0;
#endif
}


// add a polygon to the screen.

int screen::polygon(char* linespec,char* options)
{
#ifdef HUSH
  int i=0;
  char* linesp = new char[BUFSIZ];
  while (i < strlen(linespec))
  {
    linesp[i] = (int)(linespec[i] * factor);
    i++;
  }
  int id = (int)c -> polygon(linesp,options);
  delete linesp;
  return id;
#else
  linespec = linespec;
  options = options;
  return 0;
#endif
}


// add a rectangle to the screen.

int screen::rectangle(int x1,int y1,int x2,int y2,char* options)
{
#ifdef HUSH
  int p1 = (int)(factor * x1);
  int q1 = (int)(factor * y1);
  int p2 = (int)(factor * x2);
  int q2 = (int)(factor * y2);
  return ((int)c -> rectangle(p1,q1,p2,q2,options));
#else
  x1 = x1;
  y1 = y1;
  x2 = x2;
  y2 = y2;
  options = options;
  return 0;
#endif
}


// add a square to the screen.

int screen::square(int x,int y,int r,char* options)
{
#ifdef HUSH
  int x1 = (int)(factor * x);
  int y1 = (int)(factor * y);
  int r1 = (int)(factor * r);
  return ((int)c -> rectangle(x1-r1,y1-r1,x1+r1,y1+r1,options));
#else
  y = y;
  x = x;
  r = r;
  options = options;
  return 0;
#endif
}


// add a bitmap

int screen::bitmap(int x,int y,char* bitmap,char* options)
{
#ifdef HUSH
  int x1 = (int)(factor * x);
  int y1 = (int)(factor * y);
  return ((int)c -> bitmap(x1,y1,bitmap,options));
#else
  x = x;
  y = y;
  bitmap = bitmap;
  options = options;
  return 0;
#endif
}


// add a window

int screen::window(int x,int y,widget* w,char* options)
{
#ifdef HUSH
  int x1 = (int)(factor * x);
  int y1 = (int)(factor * y);
  return ((int)c -> window(x1,y1,w,options));
#else
  x = x;
  y = y;
  w= w;
  options = options;
  return 0;
#endif
}


// add text belonging to the specified figure to the screen.

int screen::text(int x,int y,char* txt,char* options)
{
#ifdef HUSH
  char* buffer = new char[BUFSIZ];
  int x1 = (int)(factor * x);
  int y1 = (int)(factor * y);
  strcpy(buffer,font);
  strcat(buffer,options);
  int id = (int)c -> text(x1,y1,txt,buffer);
  delete buffer;
  return id;
#else
  x = x;
  y = y;
  txt = txt;
  options = options;
  return 0;
#endif
}


// move the figure or id according to the x and y values

void screen::move(int id,int x,int y)
{
#ifdef HUSH
  int x1 = (int)(factor * x);
  int y1 = (int)(factor * y);
  c -> move(id,x1,y1);
#else
  id = id;
  x = x;
  y = y;
#endif
}

void screen::move(char* tg,int x,int y)
{
#ifdef HUSH
  int x1 = (int)(factor * x);
  int y1 = (int)(factor * y);
  c -> move(tg,x1,y1);
#else
  x = x;
  y = y;
#endif
}


// remove the specified figure or item from the screen

void screen::del(int id)
{
#ifdef HUSH
  c -> del(id);
#else
  id = id;
#endif
}

void screen::del(char* tg)
{
#ifdef HUSH
  c -> del(tg);
#else
  tg = tg;
#endif
}


// tag the specified item

void screen::tag(int id,char* tg)
{
#ifdef HUSH
  c -> tag(id,tg);
#else
  id = id;
  tg = tg;
#endif
}


// returns the tag from the specified item

char* screen::tags(int id)
{
#ifdef HUSH
  return (c -> tags(id));
#else
  id = id;
  return NULL;
#endif
}


// returns overlapping items

int screen::overlapping(int x,int y)
{
#ifdef HUSH
  return ((int)c -> overlapping(x,y));
#else
  x = x;
  y = y;
  return 0;
#endif
}

char* screen::items(int x,int y)
{
#ifdef HUSH
  return (c -> items(x,y));
#else
  x = x;
  y = y;
  return NULL;
#endif
}


// generate a postscript from the canvas

void screen::postscript(char* fn)
{
#ifdef HUSH
  c -> postscript(fn);
#else
  fn = fn;
#endif
}

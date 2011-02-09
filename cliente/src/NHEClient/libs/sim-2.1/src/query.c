////////////////////////////////////////////////////////////////////////
//                                                                    //
// query.c                                                            //
//                                                                    //
// implementation of the query class                                  //
//                                                                    //
// (c) 1995 D. Bolier, A. Eliens                                      //
//                                                                    //
////////////////////////////////////////////////////////////////////////

#ifdef HUSH

#include <sim/hush.h>
#include <sim/handlers.h>
#include <sim/simulation.h>
#include <sim/error.h>
#include <stdlib.h>

enum {SCREENWIDTH = 160,SCREENHEIGHT = 80};

extern kit* thekit();

#else

#include <iostream.h>

#endif

#include <sim/query.h>
#include <sim/converter.h>


// pop up a input query with the given options, the create method
// parses the options and suites them for the screen

query::query(char* p,char* options) : screen(p)
{
  create(options);
}

query::query(widget* w,char* p,char* options) : screen(w,p)
{
  create(options);
}

void query::create(char* options)
{
  // standard options
  char* txt = new char[BUFSIZ];
  sprintf(txt,"input");
#ifdef HUSH
  int fg,bg;
  fg = bg = 0;
  int wd = SCREENWIDTH;
  int hg = SCREENHEIGHT;
#endif

  // parse options
  converter* cnv = new converter(options);
  int i;
  for (i=1; i <= cnv -> argc(); i++)
  {
#ifdef HUSH
    if ((!strcmp((*cnv)[i],"-foreground")) && (i < cnv -> argc()))
      fg = i+1; 
    if ((!strcmp((*cnv)[i],"-background")) && (i < cnv -> argc()))
      bg = i+1; 
    if ((!strcmp((*cnv)[i],"-width")) && (i < cnv -> argc()))
      wd = atoi((*cnv)[i+1]); 
    if ((!strcmp((*cnv)[i],"-height")) && (i < cnv -> argc()))
      hg = atoi((*cnv)[i+1]); 
#endif
    if ((!strcmp((*cnv)[i],"-text")) && (i < cnv -> argc()))
      sprintf(txt,(*cnv)[i+1]); 
  }

#ifdef HUSH
  // screen options
  char* buffer = new char[BUFSIZ];
  if (bg)
    sprintf(buffer,"-width %d -height %d -worldx %d -worldy %d -background %s",wd,hg,SCREENWIDTH,SCREENHEIGHT,(*cnv)[bg]);
  else
    sprintf(buffer,"-width %d -height %d -worldx %d -worldy %d -background lightblue",wd,hg,SCREENWIDTH,SCREENHEIGHT);
  configure(buffer);

  // text
  if (fg)
    sprintf(buffer,"-fill %s",(*cnv)[i+1]);
  else
    sprintf(buffer,"-fill black");
  tag(text(SCREENWIDTH/2,SCREENHEIGHT/10,txt,buffer),"text");

  // entry
  if (fg && bg)
    sprintf(buffer,"-width %d -relief sunken -background %s -foreground %s",wd/10,(*cnv)[bg],(*cnv)[fg]);
  else if (fg)
    sprintf(buffer,"-width %d -relief sunken -background lightblue -foreground %s",wd/10,(*cnv)[fg]);
  else if (bg)
    sprintf(buffer,"-width %d -relief sunken -background %s -foreground black",wd/10,(*cnv)[bg]);
  else
    sprintf(buffer,"-width %d -relief sunken -background lightblue -foreground black",wd/10);
  entry* e = new entry(this,".entry",buffer);
  window(SCREENWIDTH/2,3*SCREENHEIGHT/10,e);

  // ok button
  if (fg && bg)
    sprintf(buffer,"-background %s -foreground %s",(*cnv)[bg],(*cnv)[fg]);
  else if (fg)
    sprintf(buffer,"-background lightblue -foreground %s",(*cnv)[fg]);
  else if (bg)
    sprintf(buffer,"-background %s -foreground black",(*cnv)[bg]);
  else
    sprintf(buffer,"-background lightblue -foreground black");
  button* ok = new button(this,".button",buffer);
  ok -> text("OK");
  eh = new entryhandler(e);
  ok -> bind(eh);
  window(SCREENWIDTH/2,3*SCREENHEIGHT/4,ok);
  delete buffer;
 
#else
  cout << txt << endl;
#endif

//  delete txt;  // NO      <----------
  delete[] txt;  // YES     <----------
  delete cnv;
}


// destructor

query::~query()
{
#ifdef HUSH
  delete eh;
#endif
}


// returns the value of the entry

char* query::get()
{
#ifdef HUSH
  thekit() -> update();
  char* inputvalue = eh -> get();
#else
  char* inputvalue = new char[BUFSIZ];
  cin >> inputvalue;
#endif
  return inputvalue;
}

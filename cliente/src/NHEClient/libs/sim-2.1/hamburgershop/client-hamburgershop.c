////////////////////////////////////////////////////////////////////////
//                                                                    //
// hamburgershop-client.c                                             //
//                                                                    //
// (c) 1995 D. Bolier, A. Eliens                                      //
//                                                                    //
////////////////////////////////////////////////////////////////////////
 


#ifndef HUSH
#define HUSH
#endif

// no sim.h (to avoid name clash of queue)
#include <sim/simulation.h>
#include <sim/entity.h>
#include <sim/event.h>
#include <sim/histogram.h>
#include <sim/generator.h>
#include <sim/hush.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <errno.h>
#include <math.h>
 
#define RTC_PORT 2001         // use this port
#define MAX_RETRY 10          // max. number of retries
#define WAIT_INTERVAL 200000  // in micro seconds
#define OLD 30                // hamburger too old after 30 minutes
#define TOOLONG 3             // customer waited too long
#define PARAMETERTIME 1      // 40 seconds to give initial params (jrvosse)
 
// phases customer
enum {ARRIVAL,SITDOWN,EATING,DEPARTURE};

// globals
char serverhost[BUFSIZ];
char identifier[40];
simulation* sim=0;
kit* kt=0;
screen* cn=0;
// char* textoption = "-fill black -font -*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*";
char* textoption = "-fill black ";
generator* g1, *g2, *g3, *g4;
int numberrun = 0,lastrain = 0,reorderlevel = 0,soldcola = 0,soldburgers = 0,numbertables=10,barwaiting = 0, occupied = 0, advance = 1, waitedtoolong = 0, noplace = 0, lostburgers = 0, fignum = 0,costprice = 120,sellingprice = 250,rain=0,schoolbreak=0,numberemployees = 3,totalarrivals = 0;
double costtables = 0,rainstarted,initpattern=1.5,arrpattern=1.5,costemployees = 0,profit,stayingtime=20.0,initstayingtime=20.0;

// mov. avg. waiting times and occupation
#define MOVAVG 25
double waitingtimes[MOVAVG];
int wttx = 50,wtty = 180,wttindex = 0;
double wttavg = 0;
int occx = 550,occy = 180,occindex = 0;
double occavg = 0;
double occupation[MOVAVG];

// C functions
int sock_init(char* host, int port, int family, int type, struct sockaddr* addr, int client);
char* getreply(int sd,struct sockaddr_in nm);
void stats();
 

// coordinates 

static int barcoord[40] = 
     {75,390,105,390,135,390,165,390,195,390,
      75,410,105,410,135,410,165,410,195,410,
      75,430,105,430,135,430,165,430,195,430,
      75,450,105,450,135,450,165,450,195,450};

static int tables[80] = 
     {100,470,100,530,70,500,130,500,
      100,570,100,630,70,600,130,600,
      300,320,300,380,270,350,330,350,
      300,420,300,480,270,450,330,450,
      300,520,300,580,270,550,330,550,
      300,620,300,680,270,650,330,650,
      400,320,400,380,370,350,430,350,
      400,420,400,480,370,450,430,450,
      400,520,400,580,370,550,430,550,
      400,620,400,680,370,650,430,650};

static int dishes[80] = 
     {100,485,100,515,85,500,115,500,
      100,585,100,615,85,600,115,600,
      300,335,300,365,285,350,315,350,
      300,435,300,465,285,450,315,450,
      300,535,300,565,285,550,315,550,
      300,635,300,665,285,650,315,650,
      400,335,400,365,385,350,415,350,
      400,435,400,465,385,450,415,450,
      400,535,400,565,385,550,415,550,
      400,635,400,665,385,650,415,650};

// occupation

int tableocc[40];
int barocc[20];

// list of hamburgers

class hamburger
{
public :
  hamburger()
    {n = NULL;}
  hamburger* next()
    {return n;}
  void next(hamburger* h)
    {n = h;}
  double time()
    {return t;}
  void time(double tm)
    {t = tm;}
  int size()
  {
    int sz;
    hamburger* h;

    h = n;
    sz = 1;
    while (h)
    {
      sz++;
      h = h -> next();
    }
    return sz;
  }
private :
  hamburger* n;
  double t;

};

  
// maintain list of hamburgers

hamburger* first = NULL;


// definition of the figure structure (each group of customers has one)

class figure
{
public :
  figure(int n);
  ~figure();
  void arrival();
  void noplace();
  void sitdown();
  void eat();
  void departure();
  int firsttable();
  int firstbar();
private :
  int number,tablenumber;
  int* tg;
  int* x;
  int* y;
  int* occ;
  int* dish;

};


// customer entity

class customer : public entity
{
public :
  customer(int ph);
  virtual ~customer();
  virtual int operator()();
  int arrival();
  int sitdown();
  int eat();
  int departure();
  double order();
private :
  int number,cola,burger;
  figure* f;

};


// generate number and order for a group of customers

customer::customer(int ph) : entity(ph)
{
  // number of customers in a group
  double p = g2 -> uniform(0,1);
  if (p < 0.5) 
    number = 1;
  else if (p < 0.75)
    number = 2;
  else if (p < 0.875)
    number = 3;
  else
    number = 4;

  // each customer gets 1 or 2 burgers and 0 or 1 cola, depending on
  // the selling prices
  cola = burger = 0;
  for (int i=0;i<number;i++)
  {
    burger++;
    p = sqrt(1.0 - (double)sellingprice/350.0);
    if (g2 -> probability(p))
      cola++;
    if (g2 -> probability(p))
      burger++;
  } 
  f = new figure(number);
}


customer::~customer()
{
  delete f;
}


// behavior

int customer::operator()()
{
  // adjust cost employees
  costemployees = numberemployees * (sim -> clock()*12.0/60.0);
  costtables = numbertables * (sim -> clock()*4.0/60.0);

  // update clock
  char cl[BUFSIZ];
  int hr = (9+(int)(sim -> clock()/60));
  int mn = ((int)((sim -> clock()/60-(int)(sim -> clock()/60))*60));
  if (mn < 10)
    sprintf(cl,"%d:0%d",hr,mn);
  else
    sprintf(cl,"%d:%d",hr,mn);

  cn -> del("clock");
  // int id = (int)cn -> text(130,260,cl,"-fill white -font -Adobe-helvetica-medium-r-normal--*-240*");
  int id = (int)cn -> text(130,260,cl,"-fill white");
  cn -> tag(id,"clock");
  kt -> update();

  switch ( phase() )
  {
    case ARRIVAL :
      arrival();
      break;
    case SITDOWN :
      sitdown();
      break;
    case EATING :
      eat();
      break;
    case DEPARTURE :
      departure();
  }
  return OK;
}


// arrival event

int customer::arrival()
{
  // schedule next customer
  customer* c = new customer(ARRIVAL);
  sim -> schedule(c,(g1 -> poisson(arrpattern)));

  totalarrivals += number;

  // if not enough room
  if ( ((number + occupied) > numbertables*4) || ((number + barwaiting) > 20) )
  {
     // adjust counter and statistics
    noplace += number;

    f -> noplace();
    sim -> terminate(this);
  }
  else  // enough room
  {
    barwaiting += number;
    occupied += number;

    // adjust statistics (moving average of the last MOVAVG occ-times)
    int i = occindex % MOVAVG;
    occavg -= occupation[i];
    occupation[i] = ((double)occupied/40.0 * 100.0) / MOVAVG;
    occavg += occupation[i];
    occindex++;

    // update stats
    soldcola += cola;
    soldburgers += burger;

    // sit down
    phase(SITDOWN);
    sim -> wait((cola+burger)/5.0/numberemployees);
    f -> arrival();
  }
  return OK;
}


// the customers take a seat

int customer::sitdown()
{
  phase(EATING);
  barwaiting -= number;

  // make the order
  double b = order();

  // adjust statistics (moving average of the last MOVAVG w-times)
  int i = wttindex % MOVAVG;
  wttavg -= waitingtimes[i];
  waitingtimes[i] = b / MOVAVG;
  wttavg += waitingtimes[i];
  wttindex++;

  // adjust counter
  if (b > TOOLONG)
    waitedtoolong += number;

  sim -> wait(b);

  f -> sitdown();
  return OK;
}


// eat event

int customer::eat()
{
  phase(DEPARTURE);
  sim -> wait(g3 -> exponential(stayingtime-5));
  f -> eat();
  return OK;
}


// the customers depart

int customer::departure()
{
  // adjust statistics (moving average of the last MOVAVG occ-times)
  int i = occindex % MOVAVG;
  occavg -= occupation[i];
  occupation[i] = ((double)occupied/40.0 * 100.0) / MOVAVG;
  occavg += occupation[i];
  occindex++;

  // adjust counters
  occupied -= number;

  f -> departure();
  sim -> terminate(this);
  return OK;
}


// handle the order of the group of customers dependent on the number of
// burgers allready made. It returns the time to deliver the order

double customer::order()
{
  // look for burgers that are too old
  hamburger* h = first;
  while ( (h) && ((sim -> clock() - h -> time()) > OLD) )
  {
    first = h -> next();
    delete h;
    h = first;

    // adjust counter
    lostburgers++;
  }

  // get as many burgers as possible
  int i = 0;
  double delivertime;
  while ( (h) && (i < burger) ) 
  {
    delivertime = h -> time();
    first = h -> next();
    delete h;
    h = first;
    i++;
  }
      
  // if not enough or below the reorder level, make the required number
  // of burgers 
  if ( (i < burger) || (!first) || (first -> size() < reorderlevel) )
  {
    int n = burger - i + advance;

    // determine when ready
    delivertime = sim -> clock() + 2.5 + (double)occupied / ((double)numberemployees*7.5+37.5);

    for (int j=0;j<n;j++)
    {
      h = new hamburger();
      h -> time(delivertime);
      h -> next(first);
      first = h;
    }

    // get the rest
    h = first;
  }

  // return time to wait (possibly finished allready)
  double waittime = (delivertime - sim -> clock());
  if (waittime < 0)
    return 0;
  else
    return waittime;
}


// update statistics on the screen

void stats()
{
  char buf[BUFSIZ];
  cn -> del("stats");
  sprintf(buf,"%d sold hamburgers",soldburgers);
  int id = (int)cn -> text(370,60,buf,textoption);
  cn -> tag(id,"stats");
  sprintf(buf,"%.2f",(soldburgers*(double)sellingprice/100.0));
  id = (int)cn -> text(470,60,buf,textoption);
  cn -> tag(id,"stats");
  sprintf(buf,"%d sold cola's",soldcola);
  id = (int)cn -> text(370,80,buf,textoption);
  cn -> tag(id,"stats");
  sprintf(buf,"%.2f",(soldcola*(double)sellingprice/100.0));
  id = (int)cn -> text(470,80,buf,textoption);
  cn -> tag(id,"stats");
  sprintf(buf,"%d sold hamburgers",soldburgers);
  id = (int)cn -> text(370,130,buf,textoption);
  cn -> tag(id,"stats");
  sprintf(buf,"%.2f",(soldburgers*(double)costprice/100.0));
  id = (int)cn -> text(470,130,buf,textoption);
  cn -> tag(id,"stats");
  sprintf(buf,"%d sold cola's",soldcola);
  id = (int)cn -> text(370,150,buf,textoption);
  cn -> tag(id,"stats");
  sprintf(buf,"%.2f",(soldcola*(double)costprice/100.0));
  id = (int)cn -> text(470,150,buf,textoption);
  cn -> tag(id,"stats");
  sprintf(buf,"%d lost hamburgers",lostburgers);
  id = (int)cn -> text(370,170,buf,textoption);
  cn -> tag(id,"stats");
  sprintf(buf,"%.2f",(lostburgers*(double)costprice/100.0));
  id = (int)cn -> text(470,170,buf,textoption);
  cn -> tag(id,"stats");
  sprintf(buf,"%d employees",numberemployees);
  id = (int)cn -> text(370,190,buf,textoption);
  cn -> tag(id,"stats");
  sprintf(buf,"%.2f",costemployees);
  id = (int)cn -> text(470,190,buf,textoption);
  cn -> tag(id,"stats");
  sprintf(buf,"%d tables",numbertables);
  id = (int)cn -> text(370,210,buf,textoption);
  cn -> tag(id,"stats");
  sprintf(buf,"%.2f",costtables);
  id = (int)cn -> text(470,210,buf,textoption);
  cn -> tag(id,"stats");
  profit = (soldburgers+soldcola)*(double)(sellingprice-costprice)/100.0 - lostburgers*(double)costprice/100.0 - costemployees - costtables;
  id = (int)cn -> text(370,240,"profit",textoption);
  cn -> tag(id,"stats");
  sprintf(buf,"%.2f",profit);
  id = (int)cn -> text(470,240,buf,textoption);
  cn -> tag(id,"stats");

  // update unsatisfied customers
  sprintf(buf,"%.1f",100.0*(double)noplace/(double)totalarrivals);
  id = (int)cn -> text(720,270,buf,textoption);
  cn -> tag(id,"stats");
  sprintf(buf,"%.1f",100.0*(double)waitedtoolong/(double)totalarrivals);
  id = (int)cn -> text(720,290,buf,textoption);
  cn -> tag(id,"stats");

  // update waiting times
  int newx = 50 + (int)(sim -> clock() * 25.0 / 60.0);
  int newy = 180 - (int)(wttavg / 5.0 * 100.0);
  id = (int)cn -> line(wttx,wtty,newx,newy,"-fill black");
  cn -> tag(id,"wtt");
  wttx = newx;
  wtty = newy;

  // update occupation
  newx = 550 + (int)(sim -> clock() * 25.0 / 60.0);
  newy = 180 - (int)(occavg);
  id = (int)cn -> line(occx,occy,newx,newy,"-fill black");
  cn -> tag(id,"occ");
  occx = newx;
  occy = newy;

  kt -> update();
}


// constructor taking the number of customers

figure::figure(int n)
{
  number = n;
  tg = new int[number];               // tags customers
  x = new int[number];                // x coordinates
  y = new int[number];                // y coordinates
  occ = new int[number];              // which place at the bar / table
  dish = new int[number];             // id's dishes
}

// destroy allocated memory and remove from screen

figure::~figure()
{
  char t[BUFSIZ];
  for (int i=0;i<number;i++)
  {
    sprintf(t,"tag%d",tg[i]);
    cn -> del(t);
    cn -> del(dish[i]);
  }
  delete[] x;
  delete[] y;
  delete[] tg;
  delete[] occ; 
  delete[] dish;
}

// arrival on the screen

void figure::arrival()
{
  char t[BUFSIZ];

  // open the door
  cn -> del("door");
  int id = (int)cn -> rectangle(178,662,182,702,"-fill black");
  cn -> tag(id,"door");
  kt -> update();

  // number arrivals
  for (int i=0;i<number;i++)
  {
    sprintf(t,"tag%d",fignum);
    tg[i] = fignum;
    id = (int)cn -> oval(195,725,205,735,"-fill brown");
    cn -> tag(id,t);

    // move to the door and to the bar
    cn -> move(t,0,-60);
    kt -> update();
    int j = firstbar();
    occ[i] = j;              // record place at the bar
    x[i] = barcoord[2*j];
    y[i] = barcoord[2*j+1];
    cn -> move(t,x[i]-200,y[i]-670);
    kt -> update();

    // new figure so 
    fignum++;
  }

  // close the door
  cn -> del("door");
  id = (int)cn -> rectangle(180,698,220,702,"-fill black");
  cn -> tag(id,"door");
  kt -> update();

}


// an arrival, but no place

void figure::noplace()
{
  char t[BUFSIZ];

  // open the door
  cn -> del("door");
  int id = (int)cn -> rectangle(178,662,182,702,"-fill black");
  cn -> tag(id,"door");
  kt -> update();

  // only the first
  sprintf(t,"tag%d",fignum);
  id = (int)cn -> oval(195,725,205,735,"-fill brown");
  cn -> tag(id,t);

  // move to the door and back
  cn -> move(t,0,-60);
  kt -> update();
  cn -> move(t,0,60);
  kt -> update();
  cn -> del(t);
  kt -> update();

  // new figure so 
  fignum++;

  // close the door
  cn -> del("door");
  id = (int)cn -> rectangle(180,698,220,702,"-fill black");
  cn -> tag(id,"door");
  kt -> update();

}


// sit down on the screen

void figure::sitdown()
{
  char t[BUFSIZ];

  // place at the bar not occupied anymore
  int i;
  for (i=0;i<number;i++)
    barocc[occ[i]] = 0;

  for (i=0;i<number;i++)
  {
    sprintf(t,"tag%d",tg[i]);
    int j = firsttable();
    occ[i] = j;              // record table place
    cn -> move(t,tables[2*j]-x[i],tables[2*j+1]-y[i]);
    x[i] = tables[2*j];
    y[i] = tables[2*j+1];
    kt -> update();
  }
}


// eat event (for the screen)

void figure::eat()
{
  for (int i=0;i<number;i++)
  {
    // occ[i] gives place customer
    int id = (int)cn -> oval(dishes[2*occ[i]]-3,dishes[2*occ[i]+1]-3,dishes[2*occ[i]]+3,dishes[2*occ[i]+1]+3,"-fill white");
    dish[i] = id;
    kt -> update();
  }
}


// departure on the screen

void figure::departure()
{
  char t[BUFSIZ];

  // table not occupied any more
  int i;
  for (i=0;i<number;i++)
    tableocc[occ[i]] = 0;

  // open the door
  cn -> del("door");
  int id = (int)cn -> rectangle(178,698,182,738,"-fill black");
  cn -> tag(id,"door");
  kt -> update();

  // number departures
  for (i=0;i<number;i++)
  {
    // move customer to the door and dish to the garbage can
    sprintf(t,"tag%d",tg[i]);
    cn -> move(t,200-x[i],670-y[i]);
    cn -> move(dish[i],150-dishes[2*occ[i]],670-dishes[2*occ[i]+1]);
    kt -> update();

    // move customer through the door and remove dish
    cn -> move(t,0,60);
    cn -> del(dish[i]);
    kt -> update();

    // remove customer
    cn -> del(t);
    kt -> update();
  }

  // close the door
  cn -> del("door");
  id = (int)cn -> rectangle(180,698,220,702,"-fill black");
  cn -> tag(id,"door");
  kt -> update();
}


// return first free table

int figure::firsttable()
{
  int j=0;
  while (j < 40) 
  {
    if (!(tableocc[j]))
    {
      tableocc[j] = 1;       // occupied now
      return j;
    }
    j++;
  }
  return 0;  // should never occur
}


// return first free place at the bar

int figure::firstbar()
{
  int j=0;
  while (j < 20)
  {
    if (!(barocc[j]))
    {
      barocc[j] = 1;
      return j;
    }
    j++;
  }
  return 0;  // should never occur
}


// definitions and implementations of the various handlers

class buthandler : public handler
{
public :
  buthandler()
    {ok = FALSE;}
  virtual int operator()()
    {ok = OK; return OK;}
  int get()
    {return ok;}
private :
  int ok;
}; 

class pricehandler : public handler
{
public :
  pricehandler(scale* sc)
    {s = sc;}
  virtual int operator()()
    {sellingprice = (s -> get());return sellingprice;}
private :
  scale* s;

};

class levelhandler : public handler
{
public :
  levelhandler(scale* sc)
    {s = sc;}
  virtual int operator()()
    {reorderlevel = (s -> get());return reorderlevel;}
private :
  scale* s;

};

class hamburgerhandler : public handler
{
public :
  hamburgerhandler(scale* sc)
    {s = sc;}
  virtual int operator()()
    {advance = (s -> get());return advance;}
private :
  scale* s;

};

class tablehandler : public handler
{
public :
  tablehandler(scale* sc)
  {s = sc;}
  virtual int operator()()
  {
    numbertables = (s -> get());
    return OK;
  }
private :
  scale* s;

};

class employeehandler : public handler
{
public :
  employeehandler(scale* sc)
  {
    s = sc;
  }

  virtual int operator()()
  {
    numberemployees = (s -> get());
    return OK;
  }

private :
  scale *s;

};
      

// the client event

class clientevent : public event
{
public :

clientevent() : event()
{
}


virtual int operator()()
{
  // create  and initialize socket
  if ((sd = sock_init(serverhost,RTC_PORT,AF_INET,SOCK_DGRAM,(struct sockaddr*)&nm,0)) < 0)
  {
    cerr << "INET Domain Socket" << endl;
  }

  // first update statistics
  stats();

  // send identifier en results
  char msg[BUFSIZ];
  sprintf(msg,"%s %d %d 0 %d %d %d %d %d %d",identifier,(int)(100*profit),(noplace+waitedtoolong),(int)(100*sim -> clock()),numbertables,numberemployees,sellingprice,reorderlevel,advance);
  if (sendto(sd,msg,BUFSIZ,0,(struct sockaddr*)&nm,sizeof(nm)) < 0 ) 
    cerr << "INET Domain sendto" << endl;
  // else cerr << msg << endl;

  // get the reply
  char* buf = NULL;
  int i = 0;
  while ( (!buf) && (i <= MAX_RETRY) )
  {
    buf = getreply(sd,nm); 
    i++;
  }
  if (i>1)
    cout << "number of retries : " << (i-1) << endl;

  if (!buf)
  {
    // schedule again
    sim -> wait(15);
    return FALSE;
  }

  // read parameters and show on the screen
  int sz;
  sscanf(buf,"%d %d %d ",&sz,&rain,&schoolbreak);
  if (rain)
  {
    if (!lastrain)
    {
      lastrain = 1;
      rainstarted = sim -> clock();
    }
  }
  else
    lastrain = 0;
  cn -> del("parameters");
  int ident;
  if ( (rain) && (schoolbreak) )
  {
    stayingtime = initstayingtime + 10;
    // how long does it rain ?
    if ( (sim -> clock()-rainstarted) < 35 )
      arrpattern = initpattern*0.25;
    else
      arrpattern = initpattern*2.0;
    ident = (int)cn -> oval(690,590,710,595,"-fill black");
    cn -> tag(ident,"parameters");
    ident = (int)cn -> line(700,595,700,610,"-fill black -width 3");
    cn -> tag(ident,"parameters");
    ident = (int)cn -> line(695,610,700,610,"-fill black -width 3");
    cn -> tag(ident,"parameters");
    ident = (int)cn -> text(700,620,"yes","-fill red");
    cn -> tag(ident,"parameters");
  }
  else if (rain)
  {
    stayingtime = initstayingtime + 10;
    // how long does it rain ?
    if ( (sim -> clock()-rainstarted) < 15 )
      arrpattern = initpattern*0.40;
    else
      arrpattern = initpattern*3.0;
    ident = (int)cn -> oval(690,590,710,595,"-fill black");
    cn -> tag(ident,"parameters");
    ident = (int)cn -> line(700,595,700,610,"-fill black -width 3");
    cn -> tag(ident,"parameters");
    ident = (int)cn -> line(695,610,700,610,"-fill black -width 3");
    cn -> tag(ident,"parameters");
    ident = (int)cn -> text(700,620,"no",textoption);
    cn -> tag(ident,"parameters");
  }
  else if (schoolbreak)
  {
    stayingtime = initstayingtime;
    arrpattern = initpattern*0.75;
    ident = (int)cn -> oval(690,590,710,610,"-fill yellow");
    cn -> tag(ident,"parameters");
    ident = (int)cn -> text(700,620,"yes","-fill red");
    cn -> tag(ident,"parameters");
  }
  else
  {
    stayingtime = initstayingtime;
    arrpattern = initpattern;
    ident = (int)cn -> oval(690,590,710,610,"-fill yellow");
    cn -> tag(ident,"parameters");
    ident = (int)cn -> text(700,620,"no",textoption);
    cn -> tag(ident,"parameters");
  }

  // adjust arrival pattern on the screen
  char tx[BUFSIZ];
  cn -> del("arrivals");
  sprintf(tx,"%d",(int)(60.0/arrpattern*1.875));
  ident = (int)cn -> text(700,560,tx,textoption);
  cn -> tag(ident,"arrivals");
  sprintf(tx,"%.2f",stayingtime);
  ident = (int)cn -> text(700,580,tx,textoption);
  cn -> tag(ident,"arrivals");

  // read profits from all clients and show on the screen
  int offset = 210;
  cn -> del("profits");
  char* profits;
  int prof,uns;
  while ( (profits = (char* )strchr(buf,'p')) != NULL )
  {
    sscanf(profits,"p %s %d %d",tx,&prof,&uns);
    ident = (int)cn -> text(840,offset,tx,textoption); 
    cn -> tag(ident,"profits");
    sprintf(tx,"%.2f",(double)prof/100);
    ident = (int)cn -> text(920,offset,tx,textoption); 
    cn -> tag(ident,"profits");
    sprintf(tx,"%d",uns);
    ident = (int)cn -> text(1000,offset,tx,textoption); 
    cn -> tag(ident,"profits");
    profits[0] = '.'; // change p
    offset += 20;
  }
  kt -> update();
 
  delete buf;
  // close socket
  close(sd);

  // schedule again
  sim -> wait(15);
  return OK;
}

private :
  int sd;
  struct sockaddr_in nm;

};


// create and initialize a socket

int sock_init(char* host, int port, int family, int type, struct sockaddr* addr, int)  
{
  int sd;
  if ( ( sd = socket( family, type, 0 ) ) < 0 ) 
  {
    cerr << "SOCK_INIT" << endl;
    exit(1);
  }

  struct sockaddr_in* in_name;
  struct hostent* hptr;
  if ( family == AF_INET ) 
  {
    in_name = (struct sockaddr_in*)addr;
    in_name->sin_family = family;
    in_name->sin_port = htons(port);

    if (host)  
    {
      if ((hptr = gethostbyname(host)))
        memcpy(&in_name->sin_addr.s_addr,hptr->h_addr,hptr->h_length); 
      else { 
        cerr << "Could not get hostentry of: " <<  host << endl;
	exit(1);
      }
    } 
    else 
      in_name->sin_addr.s_addr = INADDR_ANY;  
  }

  return sd;
}


// gets a reply or times out

char* getreply(int sd,struct sockaddr_in nm)
{
  char* buf = new char[BUFSIZ];

  // initialize wchich socket and which interval to wait
  fd_set read_map;
  struct timeval tval;
  FD_ZERO(&read_map);
  FD_SET(sd,&read_map);
  tval.tv_sec = 0;
  tval.tv_usec = WAIT_INTERVAL;

  //  set timer
  int i = select(sd+1,&read_map,NULL,NULL,&tval);
  if (i < 0)
  {
    if (errno == EINTR)
      return NULL;     // timeout
    else
    {
      cerr << "select failed" << endl;
      return NULL;
    }
  }
  if (i == 0)
    return NULL;

  // receive
  int nmlen = sizeof(nm);
  if (recvfrom(sd,buf,BUFSIZ,0,(struct sockaddr*)&nm,&nmlen) < 0) 
  {
    cerr << "INET Domain recv" << endl;
    return NULL;
  }
  // cerr <<  buf << endl;
  return buf;
}


// the application

class application : public session
{
public :
  application(int argc,char** argv);
  int main();
  void intro();
  void makescreen();
  int operator()() { statistics(); return OK;}
  int menu();
  int reset();
private:
  scale *levelscale, *quantityscale;
};

application::application(int argc,char** argv) : session(argc,argv,"hamburger shop")
  {
	levelscale = quantityscale = 0;
	if (argc == 2) { 
		strcpy(serverhost, &argv[1][1]); 
		argv[1][1]='-';
	} else {
		cerr << "Usage: " << argv[0] << " -<serverhostname>" << endl;
		exit(1);
	}
}


// set up and run the simulation

int application::main()
{
  // cout << "host server ?" << endl;
  // cin >> serverhost;

  // tk->trace(100);
  tk->bind("stats", this);

  // identifier of the player
  pid_t pid = getpid();
  gethostbyname(identifier);
  sprintf(identifier,"%s%ld",identifier,pid);

  kt = tk;
  sim = new simulation();
  sim -> init("-suppress statistics");

  g1 = new generator(1000,8,10);
  g2 = new generator(1,89,100);
  g3 = new generator(433,444,455);
  g4 = new generator(20,20999,2010);

  // DEBUG intro();
  // DEBUG makescreen();

  // nothing occupied yet, so
  int i;
  for (i=0;i<40;i++)
    tableocc[i] = 0;
  for (i=0;i<20;i++)
    barocc[i] = 0;

  // histograms
  for (i=0;i<MOVAVG;i++)
    waitingtimes[i] = 0;
  for (i=0;i<MOVAVG;i++)
    occupation[i] = 0;

  // schedule the initial group of customers and the clientevent
  customer* c = new customer(ARRIVAL);
  sim -> schedule(c,0.0);

  clientevent* cl = new clientevent();
  sim -> schedule(cl,15);

  while (menu()) { sim -> run(480.0); reset(); session::statistics(); } // main loop

  delete sim; delete cn;
  return OK;
}


// the menu at the beginning of a run

int application::menu()
{
  char buf[BUFSIZ];
  if (cn) cn->destroy(); 
  delete cn; 
  cn = new screen(".shop","-width 1060 -height 800 -worldx 1060 -worldy 800 -background grey");
  kt -> pack(cn);
  cn->coordsystem(canvas::canvas_coords);

  int id = (int)cn -> rectangle(500,140,700,580,"-fill white");
  cn -> tag(id,"menu");
  id = (int)cn -> text(600,160,"PARAMETERS",textoption);
  cn -> tag(id,"menu");
  id = (int)cn -> line(500,180,700,180,"-fill black -width 5");
  cn -> tag(id,"menu");
  sprintf(buf,"new cost price : %.2f",(double)costprice/100.0);
  id = (int)cn -> text(600,200,buf,textoption);
  cn -> tag(id,"menu");
  id = (int)cn -> line(500,220,700,220,"-fill black");
  cn -> tag(id,"menu");
  sprintf(buf,"new arrivals (hour) : %d",(int)(60.0/arrpattern*1.875));        // 1.875 is the mean number of cust. in a group
  id = (int)cn -> text(600,240,buf,textoption);
  cn -> tag(id,"menu");
  id = (int)cn -> line(500,260,700,260,"-fill black");
  cn -> tag(id,"menu");
  sprintf(buf,"new staying time : %.2f",stayingtime);
  id = (int)cn -> text(600,280,buf,textoption);
  cn -> tag(id,"menu");
  id = (int)cn -> line(500,300,700,300,"-fill black");
  cn -> tag(id,"menu");

  scale* s1 = new scale(cn,".scale1");
  s1 -> configure("-background mediumturquoise -foreground black -label tables -orient horizontal -length 160");
  s1 -> from(6);
  s1 -> to(10);
  s1 -> set(numbertables);
  tablehandler* th = new tablehandler(s1);
  s1 -> bind("<Any-ButtonRelease>",th);
  id = (int)cn -> window(600,340,s1);
  cn -> tag(id,"menu");

  scale* s2 = new scale(cn,".scale2");
  s2 -> configure("-background mediumturquoise -foreground black -label employees -orient horizontal -length 160");
  s2 -> from(1);
  s2 -> to(3);
  s2 -> set(numberemployees);
  employeehandler* eh = new employeehandler(s2);
  s2 -> bind("<Any-ButtonRelease>",eh);
  id = (int)cn -> window(600,400,s2);
  cn -> tag(id,"menu");

  scale* s3 = new scale(cn, ".scale3");
  s3 -> configure("-background mediumturquoise -foreground black -label sellingprice -orient horizontal -length 160");
  s3 -> from(175);
  s3 -> to(350);
  s3 -> set(sellingprice);
  pricehandler* ph = new pricehandler(s3);
  s3 -> bind("<Any-ButtonRelease>",ph);
  id = (int)cn -> window(600,460,s3);
  cn -> tag(id,"menu");
  
  id = (int)cn -> line(500,500,700,500,"-fill black -width 5");
  cn -> tag(id,"menu");

  class button* b = new class button(cn, ".button","-background lightblue -foreground black");
  b -> text("OK");
  buthandler* bh = new buthandler();
  b -> bind(bh);
  id = (int)cn -> window(600,540,b);
  cn -> tag(id,"menu");
  kt -> update();

  // read input (player gets PARAMETERTIME seconds)
  struct timeval tv;
  struct timezone tz;
  gettimeofday(&tv,&tz);
  long start = tv.tv_sec;
  long tim = start;
  int inp = bh -> get();
  while ( (!inp) && (tim < (start+PARAMETERTIME)) )
  {
    kt -> update();
    inp = bh -> get();
    gettimeofday(&tv,&tz);
    tim = tv.tv_sec;
  }
  cn -> del("menu");
  b->destroy(); 
  s1->destroy();
  s2->destroy();
  s3->destroy();

  // refresh everything ( also items that won't be removed but may be
  // lost (?) )
  cn -> del("stats");
  cn -> del("profits");
  cn -> del("clock");
  cn -> del("wtt");
  cn -> del("occ");
  cn -> del("parameters");
  cn -> del("prices");
  cn -> del("arrivals");
  cn -> del("tables");
  cn -> del("emp1");
  cn -> del("emp2");
  cn -> del("emp3");
  cn -> del("screen");
  cn -> del("door");
  tk->update(); 
  makescreen(); 

  return OK; // FALSE quits the simulation (add a quit button)
}


// reset the simulation

int application::reset()
{
  // show reset on the screen
  int id = (int)cn -> rectangle(400,250,700,350,"-fill white");
  cn -> tag(id,"reset");
  id = (int)cn -> text(550,300,"WAITING ON THE OTHER PARTICIPANTS...",textoption);
  cn -> tag(id,"reset");
  kt -> update();
   
  // send stop signal
  char* msg = new char[BUFSIZ];
  char* buf;
  char* tx = new char[BUFSIZ];
  int i,unblocked = 0;
  cout << "blocked" << endl;
  struct sockaddr_in nm;
  // int nmlen = sizeof(nm);
  while (!unblocked)
  {
    sprintf(msg,"%s %d %d 1 %d %d %d %d %d %d",identifier,(int)(100*profit),(noplace+waitedtoolong),(int)(100*sim -> clock()),numbertables,numberemployees,sellingprice,reorderlevel,advance);
    // create  and initialize socket
    int sd;
    if ((sd = sock_init(serverhost,RTC_PORT,AF_INET,SOCK_DGRAM,(struct sockaddr*)&nm,0)) < 0)
      cerr << "INET Domain Socket" << endl;

    else if (sendto(sd,msg,BUFSIZ,0,(struct sockaddr*)&nm,sizeof(nm)) < 0 ) 
      cerr << "INET Domain sendto" << endl;
    else
    {
      // cerr << msg << endl;

      // wait until server returns the unblocking message
      buf = NULL;
      i = 0;
      while ( (!buf) && (i <= MAX_RETRY) )
      {
        buf = getreply(sd,nm); 
        i++;
      }
      if (i>1)
        cout << "number of retries : " << (i-1) << endl;
  
      else if (buf)
      {
        int arrivals,staying;
        sscanf(buf,"%d %d %d %d",&unblocked,&costprice,&arrivals,&staying);
        initpattern = arrpattern = 60.0/((double)arrivals/1.875);
        initstayingtime = stayingtime = (double)staying;

        // read profits from all clients and show on the screen
        int offset = 210;
        cn -> del("profits");
        char* profits;
        int prof,uns;
        while ( (profits = (char* )strchr(buf,'p')) != NULL )
        {
          sscanf(profits,"p %s %d %d",tx,&prof,&uns);
          int id = (int)cn -> text(840,offset,tx,textoption); 
          cn -> tag(id,"profits");
          sprintf(tx,"%.2f",(double)prof/100);
          id = (int)cn -> text(920,offset,tx,textoption); 
          cn -> tag(id,"profits");
          sprintf(tx,"%d",uns);
          id = (int)cn -> text(1000,offset,tx,textoption); 
          cn -> tag(id,"profits");
          profits[0] = '.'; // change p
          offset += 20;
        }
        kt -> update();
 
      }
    }
    close(sd);
    kt -> after(2000);  // wait 2 seconds
  }
  cout << "unblocked" << endl;

  delete buf;
  delete tx;
  delete msg;
  cn -> del("reset");

  sim -> reset();  // also removes figures from the sight
  sim -> init("-suppress statistics");

  // nothing occupied yet, so
  for (i=0;i<40;i++)
    tableocc[i] = 0;
  for (i=0;i<20;i++)
    barocc[i] = 0;

  // histograms
  for (i=0;i<MOVAVG;i++)
    waitingtimes[i] = 0;
  for (i=0;i<MOVAVG;i++)
    occupation[i] = 0;
  wttx = 50;
  wtty = 180;
  wttindex = 0;
  wttavg = 0;
  occx = 550;
  occy = 180;
  occindex = 0;
  occavg = 0;

  // globals
  lastrain = barwaiting = occupied = soldcola = soldburgers = totalarrivals = noplace = lostburgers = waitedtoolong = rain = schoolbreak = 0;
  costtables = costemployees = profit = 0;

  if (first)
    delete first;
  first = NULL;

  // schedule the initial group of customers and the clientevent
  customer* c = new customer(ARRIVAL);
  sim -> schedule(c,0.0);

  clientevent* cl = new clientevent();
  sim -> schedule(cl,15);

  return OK;
}


// intro gives the introduction

void application::intro()
{
  if (cn) cn->destroy();
  delete cn;
  cn = new screen(".screen","-width 1000 -height 600 -worldx 1000 -worldy 600 -background grey");
  cn->coordsystem(canvas::canvas_coords);
  cn -> text(500,20,"A hamburger shop",textoption);
  cn -> line(0,40,1000,40,"-fill black -width 3");
  cn -> text(500,60,"The game shows a configuration of a hamburger shop. The shop is selling hamburgers and cola's and wants to maximize the profits and the",textoption); 
  cn -> text(300,80,"number of satisfied customers. The profits are determined by the following :",textoption);
  cn -> text(500,100,"- number of sold articles (revenues and costs)",textoption); 
  cn -> text(500,120,"- number of tables (a table costs 4.00 for an hour)",textoption); 
  cn -> text(500,140,"- number of employees (an employee costs 12.00 for an hour)",textoption); 
  cn -> text(500,160,"- number of lost hamburgers (costs)",textoption); 
  cn -> text(500,180,"Customers are unsatisfied if they have to wait longer then 3 minutes on an article or if they can get no place in the shop.",textoption); 
  cn -> text(500,220,"The simulation consist of a number of runs with each run representing a day. Before each run you get the new costprice of an article, ",textoption); 
  cn -> text(500,240,"the mean number of customers that will arrive in your shop (if circumstances as the weather don't change) and their mean staying time.",textoption); 
  cn -> text(310,260,"The shop can then be managed by the following : (you will get a limited amount of time)",textoption);
  cn -> text(500,280,"- The number of tables (more tables can handle more customers but cost more)",textoption);
  cn -> text(500,300,"- The number of employees (more employees can serve the customers faster (+/- 20% for each one extra),  but cost more)",textoption);
  cn -> text(500,320,"- The selling price of an article (a 1% raise on the selling price decreases the chance of selling an extra cola and burger by 1%, down to 0%)",textoption);
  cn -> text(500,360,"After you have set up the shop the simulation starts. You'll see the dynamics of the dayly proces in the shop, of the profits you make, of the",textoption);
  cn -> text(500,380,"number of unsatisfied customers, of the waiting times and of the occupation. During the simulation you can manage the number of hamburgers",textoption);
  cn -> text(500,400,"you make in advance and the level at which this will occur. Making more hamburgers in advance decreases waiting times but hamburgers that",textoption);
  cn -> text(300,420,"were made 30 or more minutes ago are lost and this effects your profits",textoption);
  cn -> text(500,460,"The number of customers and their mean staying time are affected by the weather and school breaks. If it starts to rain more customers arrive",textoption);
  cn -> text(500,480,"and they stay longer. If it keeps raining less and less customers arrive. A schoolbreak has also an increase in the number of customers as effect.",textoption);
  cn -> text(500,520,"The various hamburger shops are connected so if it starts to rain it rains with every shop. Furthermore you can follow the profits and the",textoption);
  cn -> text(500,540,"number of unsatisfied customers of the other shops in the profits window. As a consequence the program waits on the other participants",textoption);
  cn -> text(500,560,"after a run. If all shops are finished you can start a new day. You will get more customers as your competitors if you had less unsatisfied",textoption);
  cn -> text(145,580,"customers (up to 10%).",textoption);

  class button* b = new class button(".button","-background black -foreground grey");
  b -> text("OK");
  buthandler* bh = new buthandler();
  b -> bind(bh);
  kt -> pack(cn);
  kt -> pack(b);
  kt -> update();
  int inp = bh -> get();
  while (!inp)
  {
    kt -> update();
    inp = bh -> get();
  }
  b -> destroy();
  cn -> destroy();
  delete b;
  delete cn; cn=0;
}


// create the screen

void application::makescreen()
{
  char* buf = new char[BUFSIZ];

  // statistics 
  int id = (int)cn -> rectangle(30,40,280,200,"-fill lightsteelblue");
  cn -> tag(id,"screen");
  id = (int)cn -> line(50,70,50,180,"-fill black");
  cn -> tag(id,"screen");
  id = (int)cn -> line(50,180,270,180,"-fill black");
  cn -> tag(id,"screen");
  id = (int)cn -> line(30,60,280,60,"-fill black");
  cn -> tag(id,"screen");
  id = (int)cn -> text(155,50,"moving average - waiting times",textoption);
  cn -> tag(id,"screen");
  id = (int)cn -> text(40,160,"1",textoption);
  cn -> tag(id,"screen");
  id = (int)cn -> text(40,140,"2",textoption);
  cn -> tag(id,"screen");
  id = (int)cn -> text(40,120,"3",textoption);
  cn -> tag(id,"screen");
  id = (int)cn -> text(40,100,"4",textoption);
  cn -> tag(id,"screen");
  id = (int)cn -> text(40,80,"5",textoption);
  cn -> tag(id,"screen");
  int i = 9; int j = 50;
  while (i<=17)
  {
    sprintf(buf,"%d",i);
    id = (int)cn -> text(j,190,buf,textoption);
    cn -> tag(id,"screen");
    i ++;
    j += 25;
  }

  id = (int)cn -> rectangle(520,40,770,200,"-fill lightsteelblue");
  cn -> tag(id,"screen");
  id = (int)cn -> line(550,70,550,180,"-fill black");
  cn -> tag(id,"screen");
  id = (int)cn -> line(550,180,760,180,"-fill black");
  cn -> tag(id,"screen");
  id = (int)cn -> line(520,60,770,60,"-fill black");
  cn -> tag(id,"screen");
  id = (int)cn -> text(645,50,"moving average - occupation",textoption);
  cn -> tag(id,"screen");
  id = (int)cn -> text(535,155,"25",textoption);
  cn -> tag(id,"screen");
  id = (int)cn -> text(535,130,"50",textoption);
  cn -> tag(id,"screen");
  id = (int)cn -> text(535,105,"75",textoption);
  cn -> tag(id,"screen");
  id = (int)cn -> text(535,80,"100",textoption);
  cn -> tag(id,"screen");
  i = 9; j = 550;
  while (i<=17)
  {
    sprintf(buf,"%d",i);
    id = (int)cn -> text(j,190,buf,textoption);
    cn -> tag(id,"screen");
    i ++;
    j += 25;
  }

  id = (int)cn -> rectangle(300,30,500,90,"-fill lightsteelblue");
  cn -> tag(id,"screen");
  id = (int)cn -> text(370,40,"revenues",textoption);
  cn -> tag(id,"screen");
  id = (int)cn -> text(470,40,"+",textoption);
  cn -> tag(id,"screen");
  id = (int)cn -> line(440,30,440,90,"-fill black");
  cn -> tag(id,"screen");
  id = (int)cn -> line(300,50,500,50,"-fill black");
  cn -> tag(id,"screen");
  id = (int)cn -> text(370,60,"0 sold hamburgers",textoption);
  cn -> tag(id,"stats");
  id = (int)cn -> text(370,80,"0 sold cola's",textoption);
  cn -> tag(id,"stats");
  id = (int)cn -> text(470,60,"0.00",textoption);
  cn -> tag(id,"stats");
  id = (int)cn -> text(470,80,"0.00",textoption);
  cn -> tag(id,"stats");

  id = (int)cn -> rectangle(300,100,500,220,"-fill lightsteelblue");
  cn -> tag(id,"screen");
  id = (int)cn -> text(370,110,"costs",textoption);
  cn -> tag(id,"screen");
  id = (int)cn -> text(470,110,"-",textoption);
  cn -> tag(id,"screen");
  id = (int)cn -> line(440,100,440,220,"-fill black");
  cn -> tag(id,"screen");
  id = (int)cn -> line(300,120,500,120,"-fill black");
  cn -> tag(id,"screen");
  id = (int)cn -> text(370,130,"0 sold hamburgers",textoption);
  cn -> tag(id,"stats");
  id = (int)cn -> text(370,150,"0 sold cola's",textoption);
  cn -> tag(id,"stats");
  id = (int)cn -> text(370,170,"0 lost hamburgers",textoption);
  cn -> tag(id,"stats");
  sprintf(buf,"%d employees",numberemployees);
  id = (int)cn -> text(370,190,buf,textoption);
  cn -> tag(id,"stats");
  sprintf(buf,"%d tables",numbertables);
  id = (int)cn -> text(370,210,buf,textoption);
  cn -> tag(id,"stats");
  id = (int)cn -> text(470,130,"0.00",textoption);
  cn -> tag(id,"stats");
  id = (int)cn -> text(470,150,"0.00",textoption);
  cn -> tag(id,"stats");
  id = (int)cn -> text(470,170,"0.00",textoption);
  cn -> tag(id,"stats");
  id = (int)cn -> text(470,190,"0.00",textoption);
  cn -> tag(id,"stats");
  id = (int)cn -> text(470,210,"0.00",textoption);
  cn -> tag(id,"stats");

  id = (int)cn -> rectangle(300,230,500,250,"-fill lightsteelblue");
  cn -> tag(id,"screen");
  id = (int)cn -> text(370,240,"profit",textoption);
  cn -> tag(id,"screen");
  id = (int)cn -> line(440,230,440,250,"-fill black");
  cn -> tag(id,"screen");
  id = (int)cn -> text(470,240,"0.00",textoption);
  cn -> tag(id,"stats");

  id = (int)cn -> rectangle(540,240,740,300,"-fill lightblue");
  cn -> tag(id,"screen");
  id = (int)cn -> text(640,250,"unsatisfied customers (%)",textoption);
  cn -> tag(id,"screen");
  id = (int)cn -> line(540,260,740,260,"-fill black");
  cn -> tag(id,"screen");
  id = (int)cn -> line(700,260,700,300,"-fill black");
  cn -> tag(id,"screen");
  id = (int)cn -> text(620,270,"no place",textoption);
  cn -> tag(id,"screen");
  id = (int)cn -> text(620,290,"waited too long (>3)",textoption);
  cn -> tag(id,"screen");
  id = (int)cn -> text(720,270,"0",textoption);
  cn -> tag(id,"stats");
  id = (int)cn -> text(720,290,"0",textoption);
  cn -> tag(id,"stats");

  // clock
  id = (int)cn -> rectangle(80,220,180,280,"-fill blue");
  cn -> tag(id,"screen");
  id = (int)cn -> line(80,240,180,240,"-fill white");
  cn -> tag(id,"screen");
  id = (int)cn -> text(130,230,"clock","-fill white");
  cn -> tag(id,"screen");
  // id = (int)cn -> text(130,260,"9:00","-fill white -font -Adobe-helvetica-medium-r-normal--*-240*");
  id = (int)cn -> text(130,260,"9:00","-fill white");

  cn -> tag(id,"clock");
  
  // sight
  id = (int)cn -> rectangle(50,300,450,700,"-fill lightskyblue1 -outline grey");
  cn -> tag(id,"screen");

  // kitchen
  id = (int)cn -> rectangle(60,340,205,370,"-fill brown");
  cn -> tag(id,"screen");
  id = (int)cn -> rectangle(200,300,205,340,"-fill brown");
  cn -> tag(id,"screen");
  if (numberemployees > 0)
  { 
    id = (int)cn -> oval(100,315,110,325,"-fill white");
    cn -> tag(id,"emp1");
  }
  if (numberemployees > 1)
  { 
    id = (int)cn -> oval(125,315,135,325,"-fill white");
    cn -> tag(id,"emp2");
  }
  if (numberemployees > 2)
  { 
    id = (int)cn -> oval(150,315,160,325,"-fill white");
    cn -> tag(id,"emp3");
  }
  id = (int)cn -> rectangle(65,310,85,330,"-fill white");
  cn -> tag(id,"screen");
  id = (int)cn -> oval(68,313,72,317,"-fill black");
  cn -> tag(id,"screen");
  id = (int)cn -> oval(68,323,72,327,"-fill black");
  cn -> tag(id,"screen");
  id = (int)cn -> oval(78,313,82,317,"-fill black");
  cn -> tag(id,"screen");
  id = (int)cn -> oval(78,323,82,327,"-fill black");
  cn -> tag(id,"screen");
  id = (int)cn -> rectangle(175,310,195,330,"-fill white");
  cn -> tag(id,"screen");
  id = (int)cn -> oval(178,313,182,317,"-fill black");
  cn -> tag(id,"screen");
  id = (int)cn -> oval(178,323,182,327,"-fill black");
  cn -> tag(id,"screen");
  id = (int)cn -> oval(188,313,192,317,"-fill black");
  cn -> tag(id,"screen");
  id = (int)cn -> oval(188,323,192,327,"-fill black");
  cn -> tag(id,"screen");

  // door
  id = (int)cn -> rectangle(180,698,220,702,"-fill black");
  cn -> tag(id,"door");

  // garbage can
  id = (int)cn -> oval(140,660,160,680,"-fill black");
  cn -> tag(id,"screen");
  id = (int)cn -> oval(142,662,158,678,"-fill grey");
  cn -> tag(id,"screen");

  // tables
  id = (int)cn -> oval(80,480,120,520,"-fill black");
  cn -> tag(id,"screen");
  id = (int)cn -> oval(80,580,120,620,"-fill black");
  cn -> tag(id,"screen");
  id = (int)cn -> oval(280,330,320,370,"-fill black");
  cn -> tag(id,"screen");
  id = (int)cn -> oval(280,430,320,470,"-fill black");
  cn -> tag(id,"screen");
  id = (int)cn -> oval(280,530,320,570,"-fill black");
  cn -> tag(id,"screen");
  id = (int)cn -> oval(280,630,320,670,"-fill black");
  cn -> tag(id,"screen");
  int offset = 350; i = 6;
  while (i < numbertables)
  {
    id = cn -> oval(380,offset-20,420,offset+20,"-fill black");
    cn -> tag(id,"tables");
    offset += 100;
    i++;
  }
  if (numbertables < 10)
  {
    // adjust sight
    id = (int)cn -> rectangle(350,700,450,700-(10-numbertables)*100,"-fill grey -outline grey");
    cn -> tag(id,"tables");

    // adjust occupation histogram
    id = (int)cn -> line(550,180-(numbertables*10),760,180-(numbertables*10),"-fill red");
    cn -> tag(id,"tables");
  }

  // parameters
  id = (int)cn -> rectangle(530,330,750,650,"-fill mediumseagreen");
  cn -> tag(id,"screen");
  id = (int)cn -> text(640,345,"PARAMETERS",textoption);
  cn -> tag(id,"screen");
  id = (int)cn -> line(530,360,750,360,"-fill black -width 5");
  cn -> tag(id,"screen");

  id = (int)cn -> rectangle(560,380,720,410,"-fill mediumturquoise");
  cn -> tag(id,"screen");
  id = (int)cn -> text(620,395,"selling price",textoption);
  cn -> tag(id,"screen");
  id = (int)cn -> line(680,380,680,410,"-fill black");
  cn -> tag(id,"screen");
  sprintf(buf,"%.2f",(double)sellingprice/100.0);
  id = (int)cn -> text(700,395,buf,textoption);
  cn -> tag(id,"prices");

  id = (int)cn -> rectangle(560,430,720,510,"-fill mediumturquoise");
  cn -> tag(id,"screen");
  id = (int)cn -> text(640,440,"cost prices",textoption);
  cn -> tag(id,"screen");
  id = (int)cn -> line(560,450,720,450,"-fill black");
  cn -> tag(id,"screen");
  id = (int)cn -> line(680,450,680,510,"-fill black");
  cn -> tag(id,"screen");
  id = (int)cn -> text(620,460,"1 article",textoption);
  cn -> tag(id,"screen");
  id = (int)cn -> text(620,480,"1 employee (hour)",textoption);
  cn -> tag(id,"screen");
  id = (int)cn -> text(620,500,"1 table (hour)",textoption);
  cn -> tag(id,"screen");
  id = (int)cn -> text(700,480,"12.00",textoption);
  cn -> tag(id,"screen");
  id = (int)cn -> text(700,500,"4.00",textoption);
  cn -> tag(id,"screen");
  sprintf(buf,"%.2f",(double)costprice/100.0);
  id = (int)cn -> text(700,460,buf,textoption);
  cn -> tag(id,"prices");

  id = (int)cn -> rectangle(560,530,720,630,"-fill mediumturquoise");
  cn -> tag(id,"screen");
  id = (int)cn -> text(640,540,"arrival proces",textoption);
  cn -> tag(id,"screen");
  id = (int)cn -> line(560,550,720,550,"-fill black");
  cn -> tag(id,"screen");
  id = (int)cn -> line(560,590,720,590,"-fill black");
  cn -> tag(id,"screen");
  id = (int)cn -> line(680,550,680,630,"-fill black");
  cn -> tag(id,"screen");
  id = (int)cn -> text(620,560,"arrivals (hour)",textoption);
  cn -> tag(id,"screen");
  id = (int)cn -> text(620,580,"mean staying time",textoption);
  cn -> tag(id,"screen");
  id = (int)cn -> text(620,600,"weather",textoption);
  cn -> tag(id,"screen");
  id = (int)cn -> text(620,620,"school break",textoption);
  cn -> tag(id,"screen");
  sprintf(buf,"%d",(int)(60.0/arrpattern*1.875));
  id = (int)cn -> text(700,560,buf,textoption);
  cn -> tag(id,"arrivals");
  sprintf(buf,"%.2f",stayingtime);
  id= (int)cn -> text(700,580,buf,textoption);
  cn -> tag(id,"arrivals");
  id = (int)cn -> oval(690,590,710,610,"-fill yellow");
  cn -> tag(id,"parameters");
  id = (int)cn -> text(700,620,"no",textoption);
  cn -> tag(id,"parameters");

  // scales
  id = (int)cn -> rectangle(550,680,950,760,"-fill white");
  cn -> tag(id,"screen");
  id = (int)cn -> text(750,690,"STRATEGY",textoption);
  cn -> tag(id,"screen");
  id = (int)cn -> line(550,700,950,700,"-fill black");
  cn -> tag(id,"screen");

  levelscale = new scale(cn, ".levelscale");
  levelscale -> configure("-background mediumturquoise -foreground black -label reorderlevel -orient horizontal -length 160");
  levelscale -> from(0);
  levelscale -> to(10);
  levelhandler* lh = new levelhandler(levelscale);
  levelscale -> bind("<Any-ButtonRelease>",lh);
  id = (int)cn -> window(650,730,levelscale);
  cn -> tag(id,"screen");

  quantityscale = new scale(cn, ".quantityscale");
  quantityscale -> configure("-background mediumturquoise -foreground black -label quantity -orient horizontal -length 160");
  quantityscale -> from(1);
  quantityscale -> to(40);
  hamburgerhandler* hh = new hamburgerhandler(quantityscale);
  quantityscale -> bind("<Any-ButtonRelease>",hh);
  id = (int)cn -> window(850,730,quantityscale);
  cn -> tag(id,"screen");

  // profits
  id = (int)cn -> rectangle(800,50,1040,650,"-fill pink");
  cn -> tag(id,"screen");
  id = (int)cn -> line(800,90,1040,90,"-fill black");
  cn -> tag(id,"screen");
  id = (int)cn -> text(920,70,"PROFITS",textoption);
  cn -> tag(id,"screen");
  id = (int)cn -> rectangle(820,100,1020,140,"-fill pink");
  cn -> tag(id,"screen");
  buf = new char[BUFSIZ];
  sprintf(buf,"your identifier : %s",identifier);
  id = (int)cn -> text(920,120,buf,textoption);
  delete buf;
  cn -> tag(id,"screen");
  id = (int)cn -> line(800,150,1040,150,"-fill black");
  cn -> tag(id,"screen");
  id = (int)cn -> line(880,150,880,650,"-fill black");
  cn -> tag(id,"screen");
  id = (int)cn -> line(960,150,960,650,"-fill black");
  cn -> tag(id,"screen");
  id = (int)cn -> line(800,190,1040,190,"-fill black");
  cn -> tag(id,"screen");
  id = (int)cn -> text(840,170,"id",textoption);
  cn -> tag(id,"screen");
  id = (int)cn -> text(920,170,"profit",textoption);
  cn -> tag(id,"screen");
  id = (int)cn -> text(1000,170,"unsatisfied",textoption);
  cn -> tag(id,"screen");
  kt -> update();
}

 
// create and start session

main(int argc,char** argv)
{
  session* s = new application(argc,argv);
  int result = s -> run();
  delete s;
  session::statistics();
  return result;
}

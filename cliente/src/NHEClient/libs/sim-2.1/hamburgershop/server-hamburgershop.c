////////////////////////////////////////////////////////////////////////
//                                                                    //
// hamburgershop-server.c                                             //
//                                                                    //
// (c) 1995 D. Bolier, A. Eliens                                      //
//                                                                    //
////////////////////////////////////////////////////////////////////////
 
 
#ifndef HUSH
#define HUSH
#endif

#include <sim/hush.h>   // to avoid nameclash with queue
#include <sim/generator.h>
#include <sim/defs.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream.h>
#include <fstream.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <errno.h>
 
#define RTC_PORT 2001
#define MAX_RETRY 1
#define WAIT_INTERVAL 100000   // micro seconds
 

// C functions
int sock_init(char* host, int port, int family, int type, struct sockaddr* addr, int client);


// globals

class profits;
class weatherhandler;
class breakhandler;

const int end_day = 55000;
int running, startrain, stoprain, startschoolbreak, stopschoolbreak, simmode, costprice, arrivals, stayingtime, currentclock;
profits* p;
checkbutton* cb2,* cb3;
breakhandler* bh;
weatherhandler* wh;
generator* g;
scale* ps,* rs,* ss;


// defenition and implementation of the shop (client) and profits 
// (sorted list of clients) data-structures.

class shop
{
public :
  shop(char* a,int p,int u,int s,int c,int t,int e,int se,int r,int ad)
  { 
    pi = new char[40];
    strcpy(pi,a); 
    pr = p;
    uns = u;
    st = s;
    cl = c;
    tb = t;
    em = e;
    sp = se;
    rd = r;
    adv = ad;
  }
  virtual ~shop()
    {delete pi;}
  void leftpointer(shop* l)
    {lp = l;}
  shop* leftpointer()
    {return lp;}
  void rightpointer(shop* r)
    {rp = r;}
  shop* rightpointer()
    {return rp;}
  int profit()
    {return pr;}
  int unsatisfied()
    {return uns;}
  char* identifier()
    {return pi;}
  int stopped()
    {return st;}
  int clock()
    {return cl;}
  int tables()
    {return tb;}
  int employees()
    {return em;}
  int sellingprice()
    {return sp;}
  int reorder()
    {return rd;}
  int advance()
    {return adv;}

private :
  shop* lp;
  shop* rp;
  int pr;        // profit
  int uns;       // number unsatisfied
  char* pi;      // player identifier
  int st;        // stopped ?
  int cl;        // current clock * 100
  int tb;        // #tables
  int em;        // #employees
  int sp;        // selling price
  int rd;        // reorder level
  int adv;       // hamburgers in advance

};


class profits
{
public :
  profits();
  virtual ~profits();
  void reset();
  void add(shop* sh);
  int remove(char* pi);
  shop* find(char* pi);
  char* message();
  char* message(int stopped,char* currentid);
  int adjustarrivals(char* currentid);
  void file();
  void closefile();
  int stopped();
  int latest();
  int size()
    {return sz;}

private :
  shop* hd;
  shop* tl;
  int sz,day;
  ofstream* fp;

};


class breakhandler : public handler
{
public :
  breakhandler();
  virtual int operator()();
  void reset();

private :
  int schoolbreak;

};
 

class weatherhandler : public handler
{
public :
  weatherhandler();
  virtual int operator()();
  void reset();

private :
  int rain;

};
 


 // constructor, sets head and tail to the null pointer
 
profits::profits()
{
  hd = NULL;
  tl = NULL;
  sz = day = 0;
  if (!(fp = new ofstream("results")))
    cerr << "error creating results file" << endl;
}
 

// destructor closes the results file

profits::~profits()
{
  if (fp)
    fp -> close();
}


// reset the list

void profits::reset()
{
  shop* sh,* tp;
  sh = tp = hd;
  while (sh)
  {
    // throw shop away
    tp = sh -> rightpointer();
    delete sh;
    sh = tp;
  }

  hd = tl = NULL;
  sz = 0;

  // globals
  startrain = startschoolbreak = end_day;     // end of a day
  stoprain = stopschoolbreak = end_day;
  wh -> reset();
  bh -> reset();
  cb2 -> deselect();
  cb3 -> deselect();

  cout << "reset, number of clients : 0" << endl;
}


// adds an shop sh in profit order, the shop with the
// highest profit will always be at the front of the list.
 
void profits::add(shop* sh)
{
  // make sure no left or right pointer and adjust the state
  sh -> leftpointer(NULL);
  sh -> rightpointer(NULL);
 
  // if empty
  if ( (hd == NULL) || (tl == NULL) ) 
    hd = tl = sh;
 
  // if sh has the highest profit
  else if ( sh -> profit() >= hd -> profit() )
  {
    sh -> rightpointer(hd);
    hd -> leftpointer(sh);
    hd = sh;
  }
 
  // if sh has the lowest profit
  else if ( sh -> profit() <= tl -> profit() )
  {
    tl -> rightpointer(sh);
    sh -> leftpointer(tl);
    tl = sh;
  }
 
  // none of these
  else
  {
    shop* y = hd;
    // search right place, there is always a rightpointer
    while (sh -> profit() < y -> profit())
      y = y -> rightpointer();
    // insert
    y -> leftpointer() -> rightpointer(sh);
    sh -> leftpointer(y -> leftpointer());
    y -> leftpointer(sh);
    sh -> rightpointer(y);
  }

  // adjust size
  sz++;
}
 
 
// remove the shop with given identifier from the profits list
// notice that we don't have to dereference the tail pointer, because
// we adjust the head and tail, if one of them is equal to NULL.
 
int profits::remove(char* pi)
{
  // find the shop with the given identifier
  shop* sh = (find(pi));
  if (!sh)
  {
    // shop was not in the list and is added hereafter, so
    cout << "number of clients : " << (sz+1) << endl;
    return FALSE;
  }

  // if sh is the head
  if (hd == sh)
  {
    hd = hd -> rightpointer();   // possibly NULL
    if (hd)
      hd -> leftpointer(NULL);
  }
 
  // if sh is the tail
  else if (tl == sh)
  {
    tl = tl -> leftpointer();    // possibly NULL
    if (tl)
      tl -> rightpointer(NULL);
  }
 
  // none of these
  else
  {
    // cut links
    (sh->leftpointer())->rightpointer(sh->rightpointer());
    (sh->rightpointer())->leftpointer(sh->leftpointer());
  }

  // adjust size
  sz--;

  delete sh;
  return OK;
}
 

// search for a shop with the given identifier

shop* profits::find(char* pi)
{
  shop* y = hd;
  while (y && (strcmp(y -> identifier(),pi)))
    y = y -> rightpointer();
  return y;
}


// return a string of the various parameters and shops

char* profits::message()
{
  char* msg = new char[BUFSIZ];
  int rain = FALSE;
  if (currentclock >= startrain) 
    rain = OK;
  if (currentclock >= stoprain)
    rain = FALSE;
  int schoolbreak = FALSE;
  if (currentclock >= startschoolbreak) 
    schoolbreak = OK;
  if (currentclock >= stopschoolbreak)
    schoolbreak = FALSE;
  sprintf(msg,"%d %d %d ",sz,rain,schoolbreak);

  shop* sh = hd;
  while (sh)
  {
    char* buf = new char[BUFSIZ];
    sprintf(buf,"p %s %d %d ",sh -> identifier(),sh -> profit(),sh -> unsatisfied());
    strcat(msg,buf);
    delete buf;
    sh = sh -> rightpointer();
  }
  return msg;
}


// return a string with the stop message

char* profits::message(int stopped,char* currentid)
{
  // auto mode
  if (simmode)  // auto mode
  {
    costprice = (int)g -> uniform(75,150);
    ps -> set(costprice);
    arrivals = (int)g -> uniform(40,100);
    rs -> set(arrivals);
    stayingtime = (int)g -> uniform(10,30);
    ss -> set(stayingtime);
  }
  char* msg = new char[BUFSIZ];
  int arr = adjustarrivals(currentid);
  sprintf(msg,"%d %d %d %d ",stopped,costprice,arr,stayingtime); 
  shop* sh = hd;
  while (sh)
  {
    char* buf = new char[BUFSIZ];
    sprintf(buf,"p %s %d %d ",sh -> identifier(),sh -> profit(),sh -> unsatisfied());
    strcat(msg,buf);
    delete buf;
    sh = sh -> rightpointer();
  }
  return msg;
}


// adjust arrivals at the hand of the number of unsatisfied customers

int profits::adjustarrivals(char* currentid)
{
  if (!hd)
    return 0;
  double unsat;
  double max = hd -> unsatisfied();
  shop* sh = hd;
  while (sh)
  {
    if (strcmp(sh -> identifier(),currentid) == 0)
      unsat = sh -> unsatisfied();
    if (sh -> unsatisfied() > max)
      max = sh -> unsatisfied();
    sh = sh -> rightpointer();
  }
  return ((int)((0.9+(max-unsat)/max*0.2)*arrivals)); 
}


// write to a file

void profits::file()
{
  if (fp)
  {

  day++;
  char* buf = new char[BUFSIZ];
  sprintf(buf,"day : %d\n",day);
  (*fp) << buf;
  sprintf(buf,"   costprice : %d\n",costprice);
  (*fp) << buf;
  sprintf(buf,"   mean #arrivals : %d\n",arrivals);
  (*fp) << buf;
  sprintf(buf,"   mean staying time : %d\n\n",stayingtime);
  (*fp) << buf;
  (*fp) << "identifier   profit   #unsatis   #tables   #empl   sellp   reorder  advance\n";
  (*fp) << "---------------------------------------------------------------------------\n";
  shop* sh = hd;
  while (sh)
  {
     sprintf(buf,"   %s      %.2f      %d         %d       %d      %.2f      %d      %d\n",sh -> identifier(),(double)(sh -> profit())/100.0,sh -> unsatisfied(),sh -> tables(),sh -> employees(),(double)sh -> sellingprice()/100.0,sh -> reorder(),sh -> advance());
    (*fp) << buf;
    sh = sh -> rightpointer();
  }
  (*fp) << "\n\n";
  delete buf;

  }
}


// close fp

void profits::closefile()
{
  if (fp)
    fp -> close();
}


// all stopped ?

int profits::stopped()
{
  shop* sh = hd;
  while (sh)
  {
    if (!(sh -> stopped()))
      return FALSE;
    sh = sh -> rightpointer();
  }
  return OK;
}


// latest shop ?

int profits::latest()
{
  shop* sh = hd;
  int max = 0;
  while (sh)
  {
    if (sh -> clock() > max)
      max = sh -> clock();
    sh = sh -> rightpointer();
  }
  return max;
}


// implementations of the handlers for the server menu

class quithandler : public handler
{
public :
  quithandler()
    {}
  virtual int operator()()
  {
     running = FALSE;
     p -> closefile();
     return OK;
  }
};
 

class resethandler : public handler
{
public :
  resethandler()
    {}
  virtual int operator()()
  {
     p -> reset();
     return OK;
  }
};
 

breakhandler::breakhandler()
{
  schoolbreak = FALSE;
}

int breakhandler::operator()()
{
  schoolbreak= !(schoolbreak);
  if (schoolbreak)
    startschoolbreak = p -> latest() + 1500;
  else
    stopschoolbreak = p -> latest() + 1500;
  return OK;
}

void breakhandler::reset()
{
  schoolbreak = FALSE;
}
 

weatherhandler::weatherhandler()
{
  rain = FALSE;
}

int weatherhandler::operator()()
{
  rain = !(rain); 
  if (rain)
    startrain = p -> latest() + 1500;   // 15 minutes
  else
    stoprain = p -> latest() + 1500;
  return OK;
}

void weatherhandler::reset()
{
  rain = FALSE;
}
 

class modehandler : public handler
{
public :
  modehandler()
    {}
  virtual int operator()()
  {
     simmode = !(simmode);
     return OK;
  }
};


class pricehandler : public handler
{
public :
  pricehandler(scale* s)
    {sc = s;}
  virtual int operator()()
  {
     costprice = sc -> get();
     return OK;
  }
private :
  scale* sc;

};


class arrivalhandler : public handler
{
public :
  arrivalhandler(scale* s)
    {sc = s;}
  virtual int operator()()
  {
     arrivals = sc -> get();
     return OK;
  }
private :
  scale* sc;

};


class stayhandler : public handler
{
public :
  stayhandler(scale* s)
    {sc = s;}
  virtual int operator()()
  {
     stayingtime = sc -> get();
     return OK;
  }
private :
  scale* sc;

};


// create and initialize a socket

int sock_init(char* host, int port, int family, int type, struct sockaddr* addr, int /* client unused */)
{
  struct sockaddr_in* in_name;
  // unused: struct sockaddr_un* un_name;

  struct hostent* hptr;
  int sd;

  if ( ( sd = socket( family, type, 0 ) ) < 0 )
    cerr << "SOCK_INIT" << endl;

  if ( family == AF_INET )
  {
    in_name = (struct sockaddr_in*) addr;
    in_name->sin_family = family;
    in_name->sin_port = htons(port);

    if (host) 
    {
      hptr = gethostbyname( host );
      memcpy( &in_name->sin_addr.s_addr, hptr->h_addr, hptr->h_length);
    }
    else
     in_name->sin_addr.s_addr = INADDR_ANY;
  }

  return sd;
}


// the hush application, takes care of setting up the menu and the server
// and reads both until infinity

class application : public session
{
public :
  application(int argc,char** argv);
  virtual int main();
  char* getreply(char* buf,int sd,struct sockaddr* from);
  void menu();
private :
  int reply;
};
 
 
application::application(int argc,char** argv) : session(argc,argv,"server")
  {}
 
int application::main()
{
  int stopped,j,k,sd,sd1,cl,fromlen,retry,numberunblocked = 0;
  struct sockaddr_in addr,from;
  char* buf,* msg,* ident; 
  shop* sh;
  int tables,empl,sellp,reorder,advance;

  buf = new char[BUFSIZ];
  ident = new char[40];

  // globals
  startrain = startschoolbreak = 55000;     // end of a day
  stoprain = stopschoolbreak = 55000;
  simmode = FALSE;
  costprice = 120;
  arrivals = 75;
  stayingtime = 20;

  fromlen = sizeof( from );

  // create  and initialize socket
  if ((sd = sock_init(NULL,RTC_PORT,AF_INET,SOCK_DGRAM,(struct sockaddr*)&addr,1)) < 0)
  {
    cerr << "INET Domain Socket" << endl;
    exit(1);
  }

  // bind socket to the message file
  if ( bind(sd,(sockaddr*) &addr,sizeof(addr)) < 0 )
  {
    cerr << "INET Domain bind" << endl;
    exit(2);
  }

  // the generator and the profits structure 
  g = new generator(230,2444,10);
  p = new profits();

  menu();

  running = OK;
  while (running)
  {

    // get a message or time out
    memset(buf,0,sizeof(buf));
    retry = reply = 0;
    while ( (!reply) && (retry <= MAX_RETRY) )
    {
      getreply(buf,sd,(struct sockaddr*)&from);
      retry++;
    }
   
    // if no message read the menu
    if (!reply)
      tk -> update();
    else
    { 
      // handle parameters
      memset(ident,0,sizeof(ident));
      sscanf(buf,"%s %d %d %d %d %d %d %d %d %d",ident,&j,&k,&stopped,&cl,&tables,&empl,&sellp,&reorder,&advance);  

      // adjust profits
      p -> remove(ident);
      currentclock = cl;
      sh = new shop(ident,j,k,stopped,cl,tables,empl,sellp,reorder,advance);
      p -> add(sh);

      // generate return message
      if (stopped)
      {
        if (p -> stopped())
        {
          // all clients ready, unblock the client
          numberunblocked++;
          msg = p -> message(OK,ident);
          if (numberunblocked == p -> size())
          {
            // save results and reset
            p -> file();
            p -> reset();
            numberunblocked = 0;
          }
        }
        else  // keep the client blocked
          msg = p -> message(FALSE,ident);
      }
      else
        msg = p -> message();   // send parameters and profits

      if ((sd1 = sock_init(NULL,0,AF_INET,SOCK_DGRAM,(struct sockaddr*)&addr,1)) < 0)
        cerr << "UNIX DOMAIN SOCKET" << endl;
      else
      {
        // send new profits
        if (sendto(sd1,msg,BUFSIZ,0,(struct sockaddr*)&from,fromlen) < 0 ) 
          cerr << "INET Domain sendto" << endl;
      }
      close(sd1);
      delete msg;
    }
  }
  delete buf;
  delete ident;
  tk -> quit();
  return OK;
}

 
// gets a message from the client or times out

char* application::getreply(char* buf,int sd,struct sockaddr* from)
{
  struct timeval tval;
  fd_set read_map;
  int i,fromlen = sizeof(from);
 
 
  FD_ZERO(&read_map);
  FD_SET(sd,&read_map);
  tval.tv_sec = 0;
  tval.tv_usec = WAIT_INTERVAL;
 
  //  set timer
  i = select(sd+1,&read_map,NULL,NULL,&tval);
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
  
  if (recvfrom(sd,buf,BUFSIZ,0,from,&fromlen) < 0) 
    cerr << "INET Domain recv" << endl;
  else
    reply = OK;
 
  return buf;
}


// the server menu

void application::menu()
{
  canvas* cn = new canvas(".canvas","-background pink");
  cn -> geometry(300,450);

  cn -> text(150,10,"affecting the internal state","-fill black -font -*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*");

  checkbutton* cb1 = new checkbutton(".cb1","-bg black -fg white");
  cb1 -> text("automode");
  modehandler* mh = new modehandler();
  cb1 -> bind(mh);
  cn -> window(70,50,cb1);

  class button* b;
  b = new class button(".b","-bg black -fg white");
  b -> text("reset");
  resethandler* rh = new resethandler();
  b -> bind(rh);
  cn -> window(170,50,b);

  class button* q;
  q = new class button(".q","-bg black -fg white");
  q -> text("quit");
  quithandler* qh = new quithandler();
  q -> bind(qh);
  cn -> window(230,50,q);

  cn -> line(0,90,300,90,"-fill black -width 5");
  cn -> text(150,100,"used during the run","-fill black -font -*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*");

  cb2 = new checkbutton(".cb2","-bg blue -fg white");
  cb2 -> text("rain");
  wh = new weatherhandler();
  cb2 -> bind(wh);
  cn -> window(150,150,cb2);

  cb3 = new checkbutton(".cb3","-bg blue -fg white");
  cb3 -> text("break");
  bh = new breakhandler();
  cb3 -> bind(bh);
  cn -> window(150,180,cb3);

  cn -> line(0,220,300,220,"-fill black -width 5");
  cn -> text(150,230,"used before a new run","-fill black -font -*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*");

  ps = new scale(".s1");
  ps -> configure("-background blue -foreground white -label costprice -orient horizontal -length 150");
  ps -> from(75);
  ps -> to(150);
  ps -> set(120);
  pricehandler* ph = new pricehandler(ps);
  ps -> bind("<Any-ButtonRelease>",ph);
  cn -> window(150,285,ps);

  rs = new scale(".s2");
  rs -> configure("-background blue -foreground white -label arrivals -orient horizontal -length 150");
  rs -> from(40);
  rs -> to(100);
  rs -> set(75);
  arrivalhandler* ah = new arrivalhandler(rs);
  rs -> bind("<Any-ButtonRelease>",ah);
  cn -> window(150,345,rs);

  ss = new scale(".s3");
  ss -> configure("-background blue -foreground white -label stayingtime -orient horizontal -length 150");
  ss -> from(10);
  ss -> to(30);
  ss -> set(20);
  stayhandler* sh = new stayhandler(ss);
  ss -> bind("<Any-ButtonRelease>",sh);
  cn -> window(150,405,ss);

  tk -> pack(cn);
  tk -> update();
}

 
// create and start session
 
main(int argc,char** argv)
{
  session* s = new application(argc,argv);
  s -> run();
  exit(0);
}

// jobshop-sc.c --
// <a href=jobshop.txt>Description</a>

#include <sim/sim.h>

// phases job
enum {ARRIVAL,DEPARTURE,INTERRUPTED};

// phases server
enum {RUNNING,BROKEN};

// phases breakdown
enum {BOTH_SERVERS_OK,ONE_SERVER_DOWN,BOTH_SERVERS_DOWN};

// type of change on the screen
enum {APPEND,REMOVE,DISCARD,PREEMPT};

// number of job types and number of grades of service
enum {JOBTYPES=3,PRIORITIES=4};

// define highest queuingpriority and maximal waiting time
enum {HIGHEST=3,MAXWAITED=50};

class server;

simulation* sim;
histogram* waited[PRIORITIES];
histogram* total[PRIORITIES];
histogram* interrupted[PRIORITIES];
histogram* lost;
histogram* lifetime;
histogram* prob;
generator* g1, *g2;
resource* shop;
queue* q;
server* s;
kit* kt;
screen* sc;
int numberruns;
double mean[JOBTYPES] = {0.76,2.14,13.65};
double stddev[JOBTYPES] = {0.16,0.22,1.83};
double probability[JOBTYPES] = {0.21,0.57,0.22};
double arrivalrate[PRIORITIES] = {19.8,13.6,11.2,27.5};


class job : public entity
{
  public :
    job(int ph,int qp,int sp=0);
    virtual int operator()();
    int arrival();
    int departure();
    double servicetime();
    double startingtime;
    double timeleft;
    int interrupts;
};


class server : public entity
{
  public :
    server(int ph);
    virtual int operator()();
    int serve();
    int preempt();
    int discard();
    void breakdown(double t);
    void fixed();
    void changescreen(int qp,int p);
    job* current;
  private :
    int sz0,sz1,sz2,sz3;
};


class breakdown : public entity
{
  public :
    breakdown(int ph,int sp);
    virtual int operator()();
    int bothok();
    int onedown();
    int bothdown();
  private :
    int runs;
    double last;
};


class application : public session
{
public :
  application(int argc,char** argv);
  virtual int main();
  void initscreen(screen* sn);
};
  

// constructor, taking phase, scheduling priority and queuing
// priority of the job

job::job(int ph,int qp,int sp) : entity(ph,qp,sp)
{
  startingtime = 0;
  timeleft = 0;
  interrupts = 0;
}


// behavior of a job

int job::operator()()
{
  switch (phase())
  {
    case ARRIVAL :
      return arrival();
    case DEPARTURE :
      return departure();
  }
  return FALSE;
}

// arrival event

int job::arrival()
{
  // add time stamp (for check on waiting time)
  stamp();

  // schedule next job
  job* j = new job(ARRIVAL,queuingpriority());
  sim -> schedule(j,(g1 -> exponential(arrivalrate[queuingpriority()])));

  // append and report.
  report(total[queuingpriority()]);
  q -> append(this);
  s -> changescreen(this -> queuingpriority(),APPEND);
  return OK;
}

// departure event

int job::departure()
{
  sc -> del("current");
  sc -> tag(sc -> rectangle(590,230,610,270,"-fill black"),"depart");
  for (int i=0;i<20;i++)
    sc -> move("depart",10,0);
  sc -> del("depart");
  kt -> update();

  // add sample, release shop and leave it
  interrupted[queuingpriority()] -> sample(interrupts);
  shop -> release();
  sim -> terminate(this);
  s -> current = NULL;
  return OK;
}


// if interrupted return time left, else determine service time

double job::servicetime()
{
  if (phase() == INTERRUPTED)
    return timeleft;
  else
  {
    double p = g1 -> uniform(0,1);
    if (p < probability[0])
      return (g1 -> normal(mean[0],stddev[0]));
    else if (p < (probability[0] + probability[1]))
      return (g1 -> normal(mean[1],stddev[1]));
    else
      return (g1 -> normal(mean[2],stddev[2]));
  }
}
  

server::server(int ph) : entity(ph)
{
  current = NULL;
  sz0 = sz1 = sz2 = sz3 = 0;
}

// if possible serve or preempt

int server::operator()()
{
  // check if broken
  if (phase() == BROKEN)
    return FALSE;

  // check if empty
  if (!(q -> empty()))
  {  
    job* j = (job *)q -> front();
    // if waited too long don't serve it
    if (j -> timespent() > MAXWAITED)
      return discard();

    // shop available
    else if (shop -> available())
      return serve();

    // current has not highest priority and the front has
    else if ( (j -> queuingpriority() == HIGHEST) && (current -> queuingpriority() != HIGHEST) )
        return preempt();
  }
  return FALSE;
}
    
// serve the front

int server::serve()
{
  job* j = (job *)q -> removefront();

  // determine service time, before adjusting the phase
  double t = j -> servicetime();

  // acquire shop and schedule departure
  shop -> acquire();
  j -> phase(DEPARTURE);

  // set starting time and time left.
  j -> startingtime = sim -> clock();
  j -> timeleft = t;

  // set current
  current = j;
  sim -> schedule(j,t);

  // remove front and change the current on the screen
  changescreen(j -> queuingpriority(),REMOVE);

  return OK;
}


// preempt current job and serve the front

int server::preempt()
{
  job* j = (job *)q -> removefront();

  // update current job, (it always exists)
  current -> timeleft -= sim -> clock() - current -> startingtime;
  current -> startingtime = sim -> clock();

  // remove as departing entity and append as interrupted entity
  sim -> passivate(current);
  current -> phase(INTERRUPTED);
  (current -> interrupts)++;
  q -> append(current);

  // append current on the screen
  changescreen(current -> queuingpriority(),PREEMPT);

  // update current job, (it always exists)
  // serve new job
  j -> phase(DEPARTURE);
  double t = j -> servicetime();

  // set starting time and time left.
  j -> startingtime = sim -> clock();
  j -> timeleft = t;

  current = j;
  sim -> schedule(j,t);

  // remove front and update the current on the screen
  changescreen(j -> queuingpriority(),REMOVE);

  return OK;
}

// discard front

int server::discard()
{
  job* j = (job *)q -> front();

  // add sample
  lost -> sample(j -> queuingpriority());
  interrupted[queuingpriority()] -> sample(j -> interrupts);

  // remove front on the screen
  changescreen(j -> queuingpriority(),DISCARD);

  // terminate without recording the waiting time
  q -> terminatefront();

  return FALSE;
}

// model a breakdown of the server

void server::breakdown(double t)
{
  if (current)
  {
    // extract departure
    sim -> passivate(current);

    // update current job
    current -> timeleft -= sim -> clock() - current -> startingtime;
    current -> startingtime = sim -> clock() + t;

    // schedule departure
    sim -> schedule(current,(current -> timeleft + t));
  }
  phase(BROKEN);

  // show it on the screen
  sc -> del("server");
  sc -> tag(sc -> text(730,270,"broken","-fill white"),"server");
  kt -> update();
}

// the server is fixed

void server::fixed()
{
  phase(RUNNING);

  // show it on the screen
  sc -> del("server");
  sc -> tag(sc -> text(730,270,"running","-fill white"),"server");
  kt -> update();
}


// show the arrival or departure on the screen, depending on the 
// priority

void server::changescreen(int qp,int p)
{
  int x,y;
  char* buf = new char[BUFSIZ];
  int i = 0;
  switch (qp)
  {
    case 0 :
      if (p == APPEND)
      {
        sc -> tag(sc -> rectangle(10,230,30,270,"-fill black"),"new");
        for (int i=0;i<10;i++)
          sc -> move("new",(460-sz0*20)/10,-15);
        sc -> del("new");
        sz0++;
      }
      else if (p == PREEMPT)
      {
        sc -> del("current");
        sc -> tag(sc -> rectangle(570,230,590,270,"-fill black"),"current");
        for (i=0;i<20;i++)
          sc -> move("new",-28,0);
        for (i=0;i<10;i++)
          sc -> move("new",(460-sz0*20)/10,-15);
        sc -> del("new");
        sz0++;
      }
      else
      {
        sz0--;
        if (p == DISCARD)
        {
          sc -> tag(sc -> rectangle(510,80,530,120,"-fill red"),"discard");
          for (int i=0;i<10;i++)
            sc -> move("discard",10,-10);
          sc -> del("discard");
        }
        else
        {
          sc -> tag(sc -> rectangle(510,80,530,120,"-fill black"),"remove");
          for (int i=0;i<10;i++)
            sc -> move("remove",10,15);
          sc -> del("remove");
          sprintf(buf,"%d",current -> queuingpriority());
          sc -> del("current");
          sc -> tag(sc -> rectangle(600,220,640,280,"-fill white"),"current");
          sc -> tag(sc -> text(620,235,"job"),"current");
          sc -> tag(sc -> text(620,260,buf),"current");
        }
      }
      x = 500 - (sz0*20);
      y = 80;
      sc -> del("r0");
      sc -> tag(sc -> rectangle(x,y,500,y+40,"-fill black"),"r0");
      sprintf(buf,"%d",sz0);
      sc -> del("t0");
      sc -> tag(sc -> text(500,70,buf),"t0");
      break;
    case 1 :
      if (p == APPEND)
      {
        sc -> tag(sc -> rectangle(10,230,30,270,"-fill black"),"new");
        for (int i=0;i<10;i++)
          sc -> move("new",(460-sz1*20)/10,-5);
        sc -> del("new");
        sz1++;
      }
      else if (p == PREEMPT)
      {
        sc -> del("current");
        sc -> tag(sc -> rectangle(570,230,590,270,"-fill black"),"new");
        for (i=0;i<20;i++)
          sc -> move("new",-28,0);
        for (i=0;i<10;i++)
          sc -> move("new",(460-sz1*20)/10,-5);
        sc -> del("new");
        sz1++;
      }
      else
      {
        sz1--;
        if (p == DISCARD)
        {
          sc -> tag(sc -> rectangle(510,180,530,220,"-fill red"),"discard");
          for (int i=0;i<10;i++)
            sc -> move("discard",10,-10);
          sc -> del("discard");
        }
        else
        {
          sc -> tag(sc -> rectangle(510,180,530,220,"-fill black"),"remove");
          for (int i=0;i<10;i++)
            sc -> move("remove",10,5);
          sc -> del("remove");
          sprintf(buf,"%d",current -> queuingpriority());
          sc -> del("current");
          sc -> tag(sc -> rectangle(600,220,640,280,"-fill white"),"current");
          sc -> tag(sc -> text(620,235,"job"),"current");
          sc -> tag(sc -> text(620,260,buf),"current");
        }
      }
      x = 500 - (sz1*20);
      y = 180;
      sc -> del("r1");
      sc -> tag(sc -> rectangle(x,y,500,y+40,"-fill black"),"r1");
      sprintf(buf,"%d",sz1);
      sc -> del("t1");
      sc -> tag(sc -> text(500,170,buf),"t1");
      break;
    case 2 :
      if (p == APPEND)
      {
        sc -> tag(sc -> rectangle(10,230,30,270,"-fill black"),"new");
        for (i=0;i<10;i++)
          sc -> move("new",(460-sz2*20)/10,5);
        sc -> del("new");
        sz2++;
      }
      else if (p == PREEMPT)
      {
        sc -> del("current");
        sc -> tag(sc -> rectangle(570,230,590,270,"-fill black"),"new");
        for (i=0;i<20;i++)
          sc -> move("new",-28,0);
        for (int i=0;i<10;i++)
          sc -> move("new",(460-sz2*20)/10,5);
        sc -> del("new");
        sz2++;
      }
      else
      {
        sz2--;
        if (p == DISCARD)
        {
          sc -> tag(sc -> rectangle(510,280,530,320,"-fill red"),"discard");
          for (int i=0;i<10;i++)
            sc -> move("discard",10,10);
          sc -> del("discard");
        }
        else
        {
          sc -> tag(sc -> rectangle(510,280,530,320,"-fill black"),"remove");
          for (int i=0;i<10;i++)
            sc -> move("remove",10,-5);
          sc -> del("remove");
          sprintf(buf,"%d",current -> queuingpriority());
          sc -> del("current");
          sc -> tag(sc -> rectangle(600,220,640,280,"-fill white"),"current");
          sc -> tag(sc -> text(620,235,"job"),"current");
          sc -> tag(sc -> text(620,260,buf),"current");
        }
      }
      x = 500 - (sz2*20);
      y = 280;
      sc -> del("r2");
      sc -> tag(sc -> rectangle(x,y,500,y+40,"-fill black"),"r2");
      sprintf(buf,"%d",sz2);
      sc -> del("t2");
      sc -> tag(sc -> text(500,270,buf),"t2");
      break;
    case 3 :
      if (p == APPEND)
      {
        sc -> tag(sc -> rectangle(10,230,30,270,"-fill black"),"new");
        for (int i=0;i<10;i++)
          sc -> move("new",(460-sz3*20)/10,15);
        sc -> del("new");
        sz3++;
      }
      else if (p == PREEMPT)
      {
        sc -> del("current");
        sc -> tag(sc -> rectangle(570,230,590,270,"-fill black"),"current");
        for ( i=0;i<20;i++)
          sc -> move("new",-28,0);
        for (int i=0;i<10;i++)
          sc -> move("new",(460-sz3*20)/10,15);
        sc -> del("new");
        sz3++;
      }
      else
      {
        sz3--;
        if (p == DISCARD)
        {
          sc -> tag(sc -> rectangle(510,380,530,420,"-fill red"),"discard");
          for (int i=0;i<10;i++)
            sc -> move("discard",10,10);
          sc -> del("discard");
        }
        else
        {
          sc -> tag(sc -> rectangle(510,380,530,420,"-fill black"),"remove");
          for (int i=0;i<10;i++)
            sc -> move("remove",10,-15);
          sc -> del("remove");
          sprintf(buf,"%d",current -> queuingpriority());
          sc -> del("current");
          sc -> tag(sc -> rectangle(600,220,640,280,"-fill white"),"current");
          sc -> tag(sc -> text(620,235,"job"),"current");
          sc -> tag(sc -> text(620,260,buf),"current");
        }
      }
      x = 500 - (sz3*20);
      y = 380;
      sc -> del("r3");
      sc -> tag(sc -> rectangle(x,y,500,y+40,"-fill black"),"r3");
      sprintf(buf,"%d",sz3);
      sc -> del("t3");
      sc -> tag(sc -> text(500,370,buf),"t3");
      break;
  }
  kt -> update();
//  delete buf;  // NO      <----------
  delete[] buf;  // YES     <----------
}


breakdown::breakdown(int ph,int sp) : entity(ph,0,sp)
{
  last = 0;
  runs = 0;
}

// behavior of a breakdown

int breakdown::operator()()
{
  switch (phase())
  {
    case BOTH_SERVERS_OK :
      return bothok();
    case ONE_SERVER_DOWN : 
      return onedown();
    case BOTH_SERVERS_DOWN :
      return bothdown();
  }
  return FALSE;
}
  
// both servers OK

int breakdown::bothok()
{  
  // if total number of runs is reached, quit
  if (runs == numberruns)
    sim -> quit();
  else
    runs++;
  // schedule one part down
  s -> fixed();
  double life = g2 -> weibull(1,1.4);
  phase(ONE_SERVER_DOWN);
  sim -> wait(life);
  return OK;
}

// one server DOWN

int breakdown::onedown()
{
  // schedule both parts down, if not replaced on time
  double life = g2 -> weibull(1,1.4);
  double replacementtime = g2 -> uniform(0.1,0.12);
  if (life < replacementtime)
    phase(BOTH_SERVERS_DOWN);
  sim -> wait(life);
  return OK;
}

// both servers DOWN

int breakdown::bothdown()
{
  // replace both servers and adjust server process
  double replacementtime = g2 -> uniform(0.1,0.12);
  phase(BOTH_SERVERS_OK);
  sim -> wait(replacementtime);
  s -> breakdown(replacementtime);

  // sample time server was up, and the probability of the lifetime
  double t = sim -> clock() - last;
  last = sim -> clock() + replacementtime;
  lifetime -> sample(t);
  int j=0;
  while (j < t)
  {
    prob -> sample(j,100.0/numberruns);
    j += 5;
  }
  return OK;
}


application::application(int argc,char** argv) : session(argc,argv,"jobshop")
  {}

// initialize and run the simulation

int application::main()
{
  sim = new simulation();
  sim -> init("-scan yes");

  kt = tk;
  screen *sn = new screen(".s","-height 800 -worldy 800");
  initscreen(sn);

  g1 = new generator(821,2222,9); // inter-arrival and service times
  g2 = new generator(18,99,900);  // life and replacement times of the server

  waited[0]= new histogram(sn,".h1","-title waitingtimes(0) -width 290 -height 180 -background blue -foreground white");
  sn -> window(200,400,waited[0]);
  waited[1]= new histogram(sn,".h2","-title waitingtimes(1) -width 290 -height 180 -background blue -foreground white");
  sn -> window(600,400,waited[1]);
  waited[2]= new histogram(sn,".h3","-title waitingtimes(2) -width 290 -height 180 -background blue -foreground white");
  sn -> window(200,650,waited[2]);
  waited[3]= new histogram(sn,".h4","-title waitingtimes(3) -width 290 -height 180 -background blue -foreground white");
  sn -> window(600,650,waited[3]);
  tk -> update();

  toplevel* t1 = new toplevel(".time_spent");
  toplevel* t2 = new toplevel(".interrupts");
  total[0]= new histogram(t1,".h1","-form statistics -title totaltimespent(0)");
  total[1]= new histogram(t1,".h2","-form statistics -title totaltimespent(1)");
  total[2]= new histogram(t1,".h3","-form statistics -title totaltimespent(2)");
  total[3]= new histogram(t1,".h4","-form statistics -title totaltimespent(3)");
  interrupted[0]= new histogram(t2,".h1","-form statistics -title interrupts(0)");
  interrupted[1]= new histogram(t2,".h2","-form statistics -title interrupts(1)");
  interrupted[2]= new histogram(t2,".h3","-form statistics -title interrupts(2)");
  interrupted[3]= new histogram(t2,".h4","-form statistics -title interrupts(3)");

  toplevel* t3 = new toplevel(".lostjobs");
  lost = new histogram(t3,".h","-columns 4 -title lostjobs -width 170 -height 180");
  toplevel* t4 = new toplevel(".lifetime");
  lifetime = new histogram(t4,".h1","-columnwidth 10 -columns 20 -title lifetime -width 490 -height 180");
  prob = new histogram(t4,".h2","-columnwidth 5 -columns 20 -title P(X>t)(%%) -width 490 -height 180");

  q = new queue();

  shop = new resource(1);

  // add reports on the waiting times and print each 50 time units
  int i = 0;
  for (i=0;i<PRIORITIES;i++)
    q -> reportwaiting(waited[i],50,i);

  // initially the jobs are submitted and the server is conditional
  // the first ones are scheduled in priority order to generate
  // the reports in queuing priority order
  for (i=0;i<PRIORITIES;i++)
  {
    job* j = new job(ARRIVAL,i,PRIORITIES-i);
    sim -> schedule(j,0);
  }

  s = new server(RUNNING);
  sim -> hold(s);

  // schedule a breakdown, it has the lowest scheduling priority so a
  // customer first departs, if a breakdown occurs at the same time
  breakdown* b = new breakdown(BOTH_SERVERS_OK,-1);
  sim -> schedule(b,0);

  // run for numberruns lifetimes of the server
  sim -> run();

  // print results
  tk -> pack(total[0]);
  tk -> pack(total[1]);
  tk -> pack(total[2]);
  tk -> pack(total[3]);
  tk -> pack(interrupted[0]);
  tk -> pack(interrupted[1]);
  tk -> pack(interrupted[2]);
  tk -> pack(interrupted[3]);
  tk -> pack(lost);
  tk -> pack(lifetime);
  tk -> pack(prob);
  tk -> update();

  for (i=0;i<PRIORITIES;i++)
  {
    cout << (*interrupted[i]) << endl;
    delete interrupted[i];
    delete waited[i];
    delete total[i];
  }
  cout << (*lost) << endl;
  cout << (*lifetime) << endl;
  analysis* alifetime = new analysis(lifetime);
  tk -> pack(alifetime -> format(t4,".a1","-width 400 -height 100"));
  alifetime -> confidence("percent 95");
  cout << (*prob) << endl;
  analysis* aprob = new analysis(prob);
  tk -> pack(aprob -> format(t4,".a2","-width 400 -height 100"));
  aprob -> exponential();

  delete lost;
  delete lifetime;
  delete prob;
  delete alifetime;
  delete aprob;
  delete q;
  delete sim;
  return 0;
}

// initialize the screen

void application::initscreen(screen* sn)
{
  query* q = new query(sn,".query","-text #runs");
  sn -> tag(sn -> window(400,400,q),"query");
  kt -> pack(sn);
  char* input;
  while (!(input = q -> get()))
   {}
  numberruns = atoi(input);
  sn -> del("query");
  delete q;

  sc = new screen(sn,".screen","-width 400 -height 250 -worldx 800 -worldy 500 -background pink");
  sc -> rectangle(680,200,780,300,"-fill blue");
  sc -> text(730,220,"server","-fill white");
  sc -> tag(sc -> text(730,270,"running","-fill white"),"server");
  sc -> text(400,70,"size (priority 0) : ");
  sc -> text(400,170,"size (priority 1) : ");
  sc -> text(400,270,"size (priority 2) : ");
  sc -> text(400,370,"size (priority 3) : ");
  sn -> window(250,150,sc);

  control* cn = new control(sn,".control","-background white");
  sn -> window(650,100,cn);
  simulationclock* cl = new simulationclock(sn,".clock","-background black -foreground white");
  sn -> window(650,250,cl);
  kt -> update();
}


// create and start the session

int main(int argc,char** argv)
{
  session* s = new application(argc,argv);
  s -> run();
  exit(0);
}

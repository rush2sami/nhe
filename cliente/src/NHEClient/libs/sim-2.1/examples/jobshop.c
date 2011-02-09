// jobshop-hs.c --
// <a href=jobshop.txt>Description</a>

#include <sim/sim.h>

// phases job
enum {ARRIVAL,DEPARTURE,INTERRUPTED};

// phases server
enum {RUNNING,BROKEN};

// phases breakdown
enum {BOTH_SERVERS_OK,ONE_SERVER_DOWN,BOTH_SERVERS_DOWN};

// number of job types,number of grades of service and number of runs
enum {JOBTYPES=3,PRIORITIES=4,RUNS=10000};

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
    job* current;
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
  return OK;
}

// departure event

int job::departure()
{
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

  // update current job, (it always exists)
  // serve new job
  j -> phase(DEPARTURE);
  double t = j -> servicetime();

  // set starting time and time left.
  j -> startingtime = sim -> clock();
  j -> timeleft = t;

  current = j;
  sim -> schedule(j,t);
  return OK;
}

// discard front

int server::discard()
{
  job* j = (job *)q -> front();

  // add sample
  lost -> sample(j -> queuingpriority());
  interrupted[queuingpriority()] -> sample(j -> interrupts);

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
}

// the server is fixed

void server::fixed()
{
  phase(RUNNING);
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
  if (runs == RUNS)
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
  int j=0;
  // replace both servers and adjust server process
  double replacementtime = g2 -> uniform(0.1,0.12);
  phase(BOTH_SERVERS_OK);
  sim -> wait(replacementtime);
  s -> breakdown(replacementtime);

  // sample time server was up, and the probability of the lifetime
  double t = sim -> clock() - last;
  last = sim -> clock() + replacementtime;
  lifetime -> sample(t);
  while (j < t)
  {
    prob -> sample(j,100.0/RUNS);
    j += 5;
  }
  return OK;
}


class application : public session
{
public :
  application(int argc,char** argv);
  virtual int main();
};


// initialize and run the simulation

application::application(int argc,char** argv) : session(argc,argv,"jobshop")
  {}

int application::main()
{
  sim = new simulation();
  sim -> init("-scan yes");

  g1 = new generator(821,899,9009); // inter-arrival and service times
  g2 = new generator(18,3,20911);  // life and replacement times of the server

  toplevel* t1 = new toplevel(".waitingtimes");
  toplevel* t2 = new toplevel(".time_spent");
  toplevel* t3 = new toplevel(".interrupts");
  waited[0]= new histogram(t1,".h1","-form statistics -title waitingtimes(0)");
  tk -> pack(waited[0]);
  waited[1]= new histogram(t1,".h2","-form statistics -title waitingtimes(1)");
  tk -> pack(waited[1]);
  waited[2]= new histogram(t1,".h3","-form statistics -title waitingtimes(2)");
  tk -> pack(waited[2]);
  waited[3]= new histogram(t1,".h4","-form statistics -title waitingtimes(3)");
  tk -> pack(waited[3]);
  total[0]= new histogram(t2,".h1","-form statistics -title totaltimespent(0)");
  tk -> pack(total[0]);
  total[1]= new histogram(t2,".h2","-form statistics -title totaltimespent(1)");
  tk -> pack(total[1]);
  total[2]= new histogram(t2,".h3","-form statistics -title totaltimespent(2)");
  tk -> pack(total[2]);
  total[3]= new histogram(t2,".h4","-form statistics -title totaltimespent(3)");
  tk -> pack(total[3]);
  interrupted[0]= new histogram(t3,".h1","-form statistics -title interrupts(0)");
  tk -> pack(interrupted[0]);
  interrupted[1]= new histogram(t3,".h2","-form statistics -title interrupts(1)");
  tk -> pack(interrupted[1]);
  interrupted[2]= new histogram(t3,".h3","-form statistics -title interrupts(2)");
  tk -> pack(interrupted[2]);
  interrupted[3]= new histogram(t3,".h4","-form statistics -title interrupts(3)");
  tk -> pack(interrupted[3]);

  toplevel* t4 = new toplevel(".lost_jobs");
  toplevel* t5 = new toplevel(".lifetime");
  lost = new histogram(t4,".h","-columns 4 -title lostjobs -width 170 -height 180");
  lifetime = new histogram(t5,".h1","-columnwidth 10 -columns 20 -title lifetime -width 490 -height 180");
  prob = new histogram(t5,".h2","-columnwidth 5 -columns 20 -title P(X>t)(%%) -width 490 -height 180");
  tk -> pack(lost);
  tk -> pack(lifetime);
  tk -> pack(prob);

  q = new queue();

  shop = new resource(1);

  // add reports on the waiting times
  for (int i=0;i<PRIORITIES;i++)
    q -> reportwaiting(waited[i],0,i);

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

  // run for RUNS lifetimes of the server
  sim -> run();

  // print results
  for (i=0;i<PRIORITIES;i++)
  {
    cout << (*interrupted[i]);
    delete interrupted[i];
    delete waited[i];
    delete total[i];
  }
  cout << (*lost);
  cout << (*lifetime);
  cout << (*prob);
  analysis* alifetime = new analysis(lifetime);
  tk -> pack(alifetime -> format(t5,".a1","-width 400 -height 100"));
  alifetime -> confidence("percent 95");
  cout << (*prob) << endl;
  analysis* aprob = new analysis(prob);
  tk -> pack(aprob -> format(t5,".a2","-width 400 -height 100"));
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


// create and start the session

int main(int argc,char** argv)
{
  session* s = new application(argc,argv);
  s -> run();
  exit(0);
}

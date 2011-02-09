// airport.c --
// <a href=airport.txt>Description</a>

#include <sim/sim.h>

enum {LANDING,WORKING,FINISHED,DEPARTURE};

enum types {TYPE1,TYPE2};

enum prioritys {PRIORDEPARTING,PRIORTYPE2,PRIORTYPE1,PRIORLANDING};

#define MORNING 6.00
#define EVENING 22.00


simulation* sim;
generator* g1, *g2, *g3, *g4;
histogram* waitingland[2];
histogram* waitingdepart[2];
histogram* totalland[2];
histogram* totaldepart[2];
resource* landingzone;
queue* q;
int day = 0;

double landingtime(types tp);
double workingtime(types tp);
double departingtime(types tp);
void generate(types tp);


// process definitions

class airplane : public entity
{ 
public :
  airplane(int ph,int qp,types t);
  virtual int operator()();
  int land(void);
  int work(void);
  int finish(void);
  int depart(void);
  types gettype(void)
    { return tp; }
private :
  types tp;
};


class server : public entity
{
public :
  server();
  virtual int operator()();
  void servelanding(airplane* ap);
  void servedeparting(airplane* ap);
};


// process implementations

// constructor

airplane::airplane(int ph,int qp,types t) : entity(ph,qp)
{
  tp  = t;
}

// operator

int airplane::operator()()
{
  switch (phase())
  {
    case LANDING :
      return land();
    case WORKING :
      return work();
    case FINISHED :
      return finish();
    case DEPARTURE :
      return depart();
  }
  return FALSE;
}

// landing event

int airplane::land(void)
{
  // generate a new airplane and append to the queue.
  generate(tp);
  q -> append(this);
  return OK;
}

// working event

int airplane::work(void)
{
  // release landingzone and wait
  landingzone -> release();
  phase(FINISHED);
  sim -> wait(workingtime(tp));
  return OK;
}

// finish event

int airplane::finish(void)
{
  // adjust priority and append to the queue.
  if (tp == TYPE1)
    queuingpriority(PRIORTYPE1 + PRIORDEPARTING);
  else
    queuingpriority(PRIORTYPE2 + PRIORDEPARTING);
  q -> append(this);
  return OK;
}

// departing event

int airplane::depart(void)
{
  // release landingzone and leave airport
  landingzone -> release();
  sim -> terminate(this);
  return OK;
}
  

// constructor

server::server() : entity()
{
}

// the server takes airplanes, if possible and serves them.

int server::operator()()
{
  while ( (!(q -> empty())) && (landingzone -> available()) )
  {
    airplane* ap = (airplane *)q -> removefront();
    switch (ap -> phase())
    {
      case LANDING :
        servelanding(ap);
        break;
      case FINISHED :
        servedeparting(ap);
        break;
      default :
        cout << "wrong airplane mode." << endl;
    }
  }
  return OK;
}
    
// serve landing airplane

void server::servelanding(airplane* ap)
{
  // acquire landingzone and schedule working
  landingzone -> acquire();
  ap -> phase(WORKING);
  sim -> schedule(ap,landingtime(ap -> gettype()));
}

// serve departing airplane

void server::servedeparting(airplane* ap)
{
  // acquire landingzone and schedule departure  
  landingzone -> acquire();
  ap -> phase(DEPARTURE);
  sim -> schedule(ap,departingtime(ap -> gettype()));
}


// generates a new airplane

void generate(types tp)
{
  double time = sim -> clock() - 24 * day;
  if (tp == TYPE1)
  {
    int priority = PRIORTYPE1 + PRIORLANDING;
    if ( (time > MORNING) && (time < EVENING) )
    {
      airplane* ap = new airplane(LANDING,priority,TYPE1);
      sim -> schedule(ap,g1 -> gamma(0.05,0.5));
    }
    else
    {
      airplane* ap = new airplane(LANDING,priority,TYPE1);
      sim -> schedule(ap,g1 -> gamma(0.10,0.5));
    }
  }
  else if (tp == TYPE2)
  {
    int priority = PRIORTYPE2 + PRIORLANDING;
    if ( (time > MORNING) && (time < EVENING) )
    {
      airplane* ap = new airplane(LANDING,priority,TYPE2);
      sim -> schedule(ap,g1 -> gamma(0.04,2.0));
    }
    else
    {
      airplane* ap = new airplane(LANDING,priority,TYPE2);
      sim -> schedule(ap,g1 -> gamma(0.1,2.0));
    }
  }
  else
    cout << "wrong airplane type" << endl;
}
  
    
// returns landing time.

double landingtime(types tp)
{
  if (tp == TYPE1)
    return (g2 -> exponential(2.4/60.0));
  else
    return (g2 -> exponential(2.0/60.0));
}


// returns working time.

double workingtime(types tp)
{
  if (tp == TYPE1)
    return (g3 -> exponential(15));
  else
    return (g3 -> exponential(10));
}


// returns departing  time.

double departingtime(types tp)
{
  if (tp == TYPE1)
    return (g4 -> exponential(2.4/60.0));
  else
    return (g4 -> exponential(2.0/60.0));
}


class application : public session
{
public :
  application(int argc,char** argv);
  virtual int main();
private :
  int totalday;
};


application::application(int argc,char** argv) : session(argc,argv,"airport")
{
  cout << "number of day's ( > 1 ) : " << endl;
  cin >> totalday;
  if (totalday < 2)
  {
    cerr << " > 1" << endl;
    exit(0);
  }
}

// set up and run the simulation and prints the results.

int application::main()
{
  sim = new simulation();

  g1 = new generator(10,20,30);
  g2 = new generator(20,30,40);
  g3 = new generator(30,40,50);
  g4 = new generator(40,50,60);

  waitingland[0] = new histogram(".h1","-form statistics -title waitingtime-1");
  waitingland[1] = new histogram(".h2","-form statistics -title waitingtime-2");
  waitingdepart[0] = new histogram(".h3","-form statistics -title departingtime-1");
  waitingdepart[1] = new histogram(".h4","-form statistics -title departingtime-2");
 
  tk -> pack(waitingland[0]) -> pack(waitingland[1]) -> pack(waitingdepart[0]) -> pack(waitingdepart[1]);
  tk -> update();
 
  q = new queue();

  landingzone = new resource(3);

  // initially two different airplanes arrive
  generate(TYPE1);
  generate(TYPE2);

  // the server is conditional
  server* sr = new server();
  sim -> hold(sr);

  // generate the reports
  q -> reportwaiting(waitingland[0],24,PRIORLANDING+PRIORTYPE1);
  q -> reportwaiting(waitingland[1],24,PRIORLANDING+PRIORTYPE2);
  q -> reportwaiting(waitingdepart[0],24,PRIORDEPARTING+PRIORTYPE1);
  q -> reportwaiting(waitingdepart[1],24,PRIORDEPARTING+PRIORTYPE2);

  // run for totalday day's
  sim -> run(totalday * 24.0);
  
  delete sim;
  delete waitingland[0];
  delete waitingland[1];
  delete waitingdepart[0];
  delete waitingdepart[1];
  delete q;
  return 0;
}


// create and start the session

int main(int argc,char** argv)
{
  session* s = new application(argc,argv);
  s -> run();
  exit(0);
}

// airport-ev.c --
// <a href=airport.txt>Description</a>

#include <sim/sim.h>

enum {LANDING,FINISHED};

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


// event definitions

class landing : public entity
{
public :
  landing(int pr,types type);
  virtual int operator()();
  types gettype(void)
    { return tp; }
private :
  types tp;
};


class work : public event
{
public :
  work(landing* ln,types type);
  virtual int operator()();
private :
  landing* l;
  types tp;
};


class finish : public entity
{
public :
  finish(work* wr,types type);
  virtual int operator()();
  types gettype(void)
    { return tp; }
private :
  work* w;
  types tp;
};


class departure : public event
{
public :
  departure(finish* fin,types type);
  virtual int operator()();
private :
  finish* f;
  types tp;
};


class service : public event
{
public :
  service();
  virtual int operator()();
  void servicelanding(void);
  void servicedeparture(void);
};


// event implementations

// landing event
landing::landing(int pr,types type) : entity(LANDING,pr)
{
  tp = type;
}

int landing::operator()()
{
  // generate a new land event and append to the queue.
  generate(tp);
  q -> append(this);
  return OK;
}

// working event
work::work(landing* ln,types type) : event()
{
  // record land event and type
  l = ln;
  tp = type;
}
  
int work::operator()(void)
{
  // release landingzone and wait
  sim -> terminate(l);
  landingzone -> release();
  finish* f = new finish(this,tp);
  sim -> schedule(f,workingtime(tp));
  return OK;
}

// finish event
finish::finish(work* wr,types type) : entity(FINISHED)
{
  w = wr;
  tp = type;
}

int finish::operator()(void)
{
  // finished working
  sim -> terminate(w);
  // adjust priority and append to the queue.
  if (tp == TYPE1)
    queuingpriority(PRIORTYPE1 + PRIORDEPARTING);
  else
    queuingpriority(PRIORTYPE2 + PRIORDEPARTING);
  q -> append(this);
  return OK;
}

// departing event
departure::departure(finish* fin,types type) : event()
{
  // record finish event and type
  f = fin;
  tp = type;
}

int departure::operator()(void)
{
  // release landingzone and leave airport
  sim -> terminate(f);
  landingzone -> release();
  sim -> terminate(this);
  return OK;
}
  

// service event
service::service() : event()
{
}

// service takes airplanes, if possible and serves them.
int service::operator()()
{
  while ( (!(q -> empty())) && (landingzone -> available()) )
  {
    entity* e = (entity *)q -> front();
    switch (e -> phase())
    {
      case LANDING :
        servicelanding();
        break;
      case FINISHED :
        servicedeparture();
        break;
      default :
        cout << "wrong event mode." << endl;
        break;
    }
  }
  return OK;
}

// two service events, for landing and departure

// serve landing airplane
void service::servicelanding(void)
{
  landing* l = (landing *)q -> removefront();
  // acquire landingzone and schedule working
  landingzone -> acquire();
  work* w = new work(l,(l -> gettype()));
  sim -> schedule(w,landingtime(l -> gettype()));
}

// serve departing airplane
void service::servicedeparture(void)
{
  finish* f = (finish *)q -> removefront();
  // acquire landingzone and schedule departure  
  landingzone -> acquire();
  departure* dep = new departure(f,(f -> gettype()));
  sim -> schedule(dep,departingtime(f -> gettype()));
}


// generates a new landing event
void generate(types tp)
{
  double time = sim -> clock() - 24.0 * (double)day;
  if (tp == TYPE1)
  {
    int prior = PRIORTYPE1 + PRIORLANDING;
    landing* l = new landing(prior,TYPE1);
    if ( (time > MORNING) && (time < EVENING) )
      sim -> schedule(l,g1 -> gamma(0.05,0.5));
    else
      sim -> schedule(l,g1 -> gamma(0.10,0.5));
  }
  else if (tp == TYPE2)
  {
    int prior = PRIORTYPE2 + PRIORLANDING;
    landing* l = new landing(prior,TYPE2);
    if ( (time > MORNING) && (time < EVENING) )
      sim -> schedule(l,g1 -> gamma(0.04,2.0));
    else
      sim -> schedule(l,g1 -> gamma(0.1,2.0));
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

  // the service event is conditional
  service* sr = new service();
  sim -> hold(sr);

  // generate reports
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

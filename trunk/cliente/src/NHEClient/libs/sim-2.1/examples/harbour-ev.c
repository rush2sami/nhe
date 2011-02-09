// harbour-ev.c --
// <a href=harbour.txt>Description</a>

#include <sim/sim.h>

enum {ARRIVAL,DEPARTURE};

simulation* sim;
generator* g1, *g2;
histogram* h1, *h2;
resource* harbour;
queue* q;


class arrival : public event
{
  public :
    arrival();
    virtual int operator()();
};


class departure : public event
{
  public :
    departure(arrival* a);
    virtual int operator()();
  private :
    arrival* arr;
};


class service : public event
{
  public :
    service();
    virtual int operator()();
};


arrival::arrival() : event()
{
}


int arrival::operator()()
{
  // schedule next arrival
  arrival* arr = new arrival();
  sim -> schedule(arr,(g1 -> exponential(4.0/3.0)));

  // append.
  q -> append(this);
  return OK;
}


departure::departure(arrival* a) : event()
{
  arr = a;
}


int departure::operator()()
{
  // release harbour and leave it
  harbour -> release();
  sim -> terminate(arr);
  sim -> terminate(this);
  return OK;
}


service::service() : event()
{
}


int service::operator()()
{
  while ( (!(q -> empty())) && (harbour -> available()) )
  {
    // append and record size.
    arrival* arr = (arrival *)q -> removefront();

    // acquire harbour and schedule departure
    departure* dep = new departure(arr);
    harbour -> acquire();

    double processing;
    if (harbour -> full())
      processing = g2 -> uniform(0.25,0.75);
    else
      processing = g2 -> uniform(0.5,1.5);
    sim -> schedule(dep,processing);
  }
  return OK;
}
    

class application : public session
{
public :
  application(int argc,char** argv);
  virtual int main();
};
 
 
application::application(int argc,char** argv) : session(argc,argv,"harbour")
{
}
 
 
int application::main()
{
  sim = new simulation();
  sim -> init("-suppress both");

  g1 = new generator(100,1,2);
  g2 = new generator(1,2,3);

  histogram* h1 = new histogram(".h1","-type weighted");
  histogram* h2 = new histogram(".h2"); // type frequency
  histogram* sizes = new histogram(".sizes","-form statistics -title means_size");
  histogram* waitingtimes = new histogram(".waitingtimes","-form statistics -title means_waitingtimes");

  q = new queue();
  q -> reportsize(h1);
  q -> reportwaiting(h2);

  harbour = new resource(2);

  // initially a ship arrives and the master is conditional
  arrival* ar = new arrival();
  sim -> schedule(ar,0.0);

  service* sr = new service();
  sim -> hold(sr);

  // 100 runs of 1000 time units
  for (int i=0;i<100;i++)
  {
    sim -> run(1000.0);
    sizes -> sample(h1 -> mean());
    waitingtimes -> sample(h2 -> mean());
    h1 -> reset();
    h2 -> reset();
  }

  tk -> pack(sizes) -> pack(waitingtimes);
  cout << (*waitingtimes) << endl;
  cout << (*sizes) << endl;

  delete h1;
  delete h2;
  delete q;
  delete sim;
  return 0;
}


int main(int argc,char** argv)
{
  session* s = new application(argc,argv);
  s -> run();
  exit(0);
}

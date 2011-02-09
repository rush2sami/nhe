// postoffice-ev.c --
// <a href=postoffice.txt>Description</a>

#include <sim/sim.h>

#define BUFFERSIZE 20

simulation* sim;
generator* g1, *g2;
resource* servicepoint;
queue* q;


// event definitions.

class arrival : public event
{
  public :
    arrival();
    virtual int operator()();
};


class departure : public event
{
  public :
    departure();
    virtual int operator()();
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


// arrival event.
int arrival::operator()()
{
  arrival* arr = new arrival();

  // schedule next arrival
  sim -> schedule(arr,(g1 -> exponential(1.0/5.0)));

  // append
  q -> append(this);

  return OK;
}


departure::departure() : event()
{
}


// departure event.
int departure::operator()()
{
  // release servicepoint and leave it
  servicepoint -> release();
  sim -> terminate(this);

  return OK;
}


service::service() : event()
{
}


// service event.
int service::operator()()
{
  while ( (!(q -> empty())) && (servicepoint -> available()) )
  {
    sim -> terminate(q -> removefront());

    // acquire servicepoint and schedule departure
    servicepoint -> acquire();
    departure* d = new departure();
    sim -> schedule(d,(g2 -> exponential(1.0/6.0)));
  }
  return OK;
}
    


class application : public session
{
public :
  application(int argc,char** argv);
  virtual int main();
private :
  int totalday;
};
 
 
application::application(int argc,char** argv) : session(argc,argv,"postoffice")
{
}
 
 
// set up and run the simulation and prints the results.
 
int application::main()
{
  sim = new simulation();

  g1 = new generator(255,89,1009);
  g2 = new generator(1000,8,77);

  histogram* h1 = new histogram(".h1","-columnwidth 3 -type weighted -title size");
  histogram* h2 = new histogram(".h2","-title waitingtimes");
  tk -> pack(h1) -> pack(h2);
  toplevel* t = new toplevel(".moving_average");
  histogram* h3 = new histogram(t,".h1","-columnwidth 20 -columns 24 -type series -title moving_average_size");
  histogram* h4 = new histogram(t,".h2","-columnwidth 20 -columns 24 -type series -title moving_average_waitingtimes");
  tk -> pack(h3);
  tk -> pack(h4);

  q = new queue();
  q -> reportsize(h1);
  q -> reportwaiting(h2);

  analysis* a1 = new analysis(h1);
  a1 -> behavior(h3,BUFFERSIZE);
  analysis* a2 = new analysis(h2);
  a2 -> behavior(h4,BUFFERSIZE);

  servicepoint = new resource(1);

  // initially a customer arrives and the server is conditional
  arrival* arr = new arrival();
  sim -> schedule(arr,0.0);

  service* sr = new service();
  sim -> hold(sr);

  // run for 8 hours
  sim -> run(480.0);

  // reset environment
  q -> reset();
  delete sim;                // to delete steady reports
  sim = new simulation();
  servicepoint -> reset();
  g1 -> reset();
  g2 -> reset();

  // run for transient phase.
  cout << "transient phase" << endl;
  arr = new arrival();
  sim -> schedule(arr,0.0);
  sr = new service();
  sim -> hold(sr);
  sim -> run(50.0);

  // reset statistics and run for steady phase.
  cout << "steady phase" << endl;
  h1 -> reset();
  h2 -> reset();

// CRASH : sim was deleted without q's knowlegde. q will try to generate
// a report based on the deleted sim.
  q -> reportsize(h1);
  q -> reportwaiting(h2);
  sim -> run(430.0);

  delete a1;
  delete a2;
  delete q;
  delete h1;
  delete h2;
  delete h3;
  delete h4;
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

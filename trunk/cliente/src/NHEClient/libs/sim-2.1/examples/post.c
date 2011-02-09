// postoffice.c --
// <a href=postoffice.txt>Description</a>

#include <sim/sim.h>

#define BUFFERSIZE 20


enum {ARRIVAL,DEPARTURE};

simulation* sim;
generator* g1,*g2;
resource* servicepoint;
queue* q;


class application : public session
{
public :
  application(int argc,char** argv);
  virtual int main();
  void initscreen();
};


class customer : public entity
{ 
  public :
    customer(int ph);
    virtual int operator()();
};


class server : public entity
{
  public :
    server();
    virtual int operator()();
};


customer::customer(int ph) : entity(ph)
{
}


// actions of the customer.
int customer::operator()()
{
  customer *c;
  switch (phase())
  {
    case ARRIVAL :
      // schedule next customer
      c = new customer(ARRIVAL);
      sim -> schedule(c,(g1 -> exponential(1.0/5.0)));

      //
      q -> append(this);
      return OK;
    case DEPARTURE :
      // release servicepoint and leave it
      servicepoint -> release();
      sim -> terminate(this);
      return OK;
  }
  return FALSE;
}


server::server() : entity()
{
}

// the server takes customers, if possible and serves them.
int server::operator()()
{
  while ( (!(q -> empty())) && (servicepoint -> available()) )
  {
    customer* c = (customer *)q -> removefront();

    // acquire servicepoint and schedule departure
    servicepoint -> acquire();
    c -> phase(DEPARTURE);
    sim -> schedule(c,(g2 -> exponential(1.0/6.0)));
  }
  return OK;
}
    

application::application(int argc,char** argv) : session(argc,argv,"postoffice")
  {}

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
  customer* c = new customer(ARRIVAL);
  sim -> schedule(c,0.0);

  server* sr = new server();
  sim -> hold(sr);

  // run for 8 hours
  sim -> run(480.0);

  // reset environment
  q -> reset();
  delete sim;              // to delete behavior reports
  sim = new simulation();
  servicepoint -> reset();
  g1 -> reset();
  g2 -> reset();

  // run for transient phase.
  cout << "transient phase" << endl;
  c = new customer(ARRIVAL);
  sim -> schedule(c,0.0);
  sr = new server();
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
  return 0;
}


// create and start the session

main(int argc,char** argv)
{
  session* s = new application(argc,argv);
  s -> run();
  exit(0);
}

// testqueues.c

#include <sim/sim.h>

enum {ARRIVAL,DEPARTURE};

enum {M,D};

simulation* sim;
generator* g1,* g2;
resource* r;
queue* q;
double duration,lb,mu,mu1,mu2,p;
int servicetype,number=1,k;

void parameters();
double servicetime();
void analyse(analysis* a1,analysis* a2);


// read the parameters of the model

void parameters()
{
  cout << "service pattern (0(M), 1(D))" << endl;
  cin >> servicetype;
  if ( (servicetype < 0) || (servicetype > 1) )
    exit(0);
  cout << "arrival rate" << endl;
  cin >> lb;
  if (lb <= 0)
    exit(0);
  cout << "service rate" << endl;
  cin >> mu;
  if (mu <= 0)
    exit(0);
  if (servicetype == M)
  {
    cout << "number of service points" << endl;
    cin >> number;
    if (number < 1)
      exit(0);
  }
  cout << "duration" << endl;
  cin >> duration;
}

// return service time

double servicetime()
{
  switch (servicetype)
  {
    case M :
      return (g2 -> exponential(1/mu));
    case D :
      return 1/mu;
  }
}

// analyse the results

void analyse(analysis* a1,analysis* a2)
{
  if (servicetype == M)
  {
    if (number == 1)
    {
      a1 -> MM1size(lb,mu);
      a2 -> MM1waitingtime(lb,mu);
    }
    else
    {
      a1 -> MMcsize(lb,mu,number);
      a2 -> MMcwaitingtime(lb,mu,number);
    }
  }
  else // servicetype = D
  {
    a1 -> MD1size(lb,mu);
    a2 -> MD1waitingtime(lb,mu);
  }
}
   
// process definitions

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

// process implementations

customer::customer(int ph) : entity(ph)
{
}

int customer::operator()()
{
  customer* c;
  switch (phase())
  {
    case ARRIVAL :
      // schedule a new customer
      c = new customer(ARRIVAL);
      sim -> schedule(c,g1 -> exponential(1/lb));

      // append to the queue
      q -> append(this);
      return OK;
    case DEPARTURE :
      // release resource and depart
      r -> release();
      sim -> terminate(this);
      return OK;
  }
  return FALSE;
}

server::server() : entity()
{
}

int server::operator()()
{
  customer* c;
  // serve as many customers as possible
  while ( (!(q -> empty())) && (r -> available()) )
  {
    c = (customer *)q -> removefront();
    r -> acquire();

    // schedule departure after being served
    c -> phase(DEPARTURE);
    sim -> schedule(c,servicetime());
  }
  return OK;
}
    

class application : public session
{
public :
  application(int argc,char** argv);
  virtual int main();
};
  
application::application(int argc,char** argv) : session(argc,argv,"queuetest")
  {};

// set up and run the simulation

int application::main()
{
  parameters();
  sim = new simulation();
  g1 = new generator(8120,2,3333);
  g2 = new generator(434,982,1000);
  histogram* h1 = new histogram(".size","-type weighted -title size");
  histogram* h2 = new histogram(".waitingtimes","-title waitingtimes");
  tk -> pack(h1);
  tk -> pack(h2);
  q = new queue();
  r = new resource(number);
  customer* c = new customer(ARRIVAL);
  sim -> schedule(c,0.0);
  server* s = new server();
  sim -> hold(s);
  q -> reportsize(h1);
  q -> reportwaiting(h2);
  sim -> run(duration);
  analysis* a1 = new analysis(h1);
  analysis* a2 = new analysis(h2);
  analyse(a1,a2);
  delete h1;
  delete h2;
  delete a1;
  delete a2;
  delete q;
  delete sim;
  return 0;
}

// create and start the session

main(int argc,char** argv)
{
  session* s = new application(argc,argv);
  s -> run();
  exit(0);
}

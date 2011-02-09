// bank.c --
// <a href=bank.txt>Description</a>

#include <sim/sim.h>

#define MAXLOKET 10

simulation* sim;
int nservicepoint;
double between,serving;
generator* g1, *g2;
histogram* hcust; 
histogram* hservicepoint[MAXLOKET]; 
histogram* hqueue[MAXLOKET];
resource* servicepoint[MAXLOKET];
queue* q[MAXLOKET];

int findbest(void);


class arrival : public event
{
  public :
    arrival();
    virtual int operator()();
  private :
    int best;
};


class departure : public event
{
  public :
    departure(arrival* a,int i);
    virtual int operator()();
  private :
    arrival* arr;
    int ident;
};


class service : public event
{
  public :
    service();
    virtual int operator()();
};



arrival::arrival() : event()
{
  best = 0;
}


int arrival::operator()()
{
  // schedule next arrival
  arrival* arr = new arrival();
  sim -> schedule(arr,(g1 -> exponential(between)));

  // process arrival and find servicepoint
  report(hcust);
  best = findbest();

  // append 
  q[best] -> append(this);
  return OK;
}


departure::departure(arrival* a,int i) : event()
{
  arr = a;
  ident = i;
}


int departure::operator()()
{
  // release servicepoint 'ident' and leave it
  servicepoint[ident] -> release();

  // terminate arrival and departure event.
  sim -> terminate(arr);
  sim -> terminate(this);
  return OK;
}


service::service() : event()
{
}


int service::operator()()
{
  for (int i=0;i<nservicepoint;i++)
  {
    while ( (!(q[i] -> empty())) && (servicepoint[i] -> available()) )
    {
      // append and record size.
      arrival* arr = (arrival *)q[i] -> removefront();

      // acquire servicepoint, record waiting time and schedule departure
      servicepoint[i] -> acquire();
      departure* dep = new departure(arr,i);
      sim -> schedule(dep,(g2 -> exponential(serving)));
    }
  }
  return OK;
}
    

int findbest(void)
{
  int index;
  for (int i=0;i<nservicepoint;i++)
  {
    int sz = q[i] -> size();
    int min = 10000;
    if (sz == 0)
      return i;
    else if (sz < min)
    {
      min = sz;
      index = i;
    }
  }
  return index;
}


 
class application : public session
{
public :
  application(int argc,char** argv);
  virtual int main();
private :
  double duration;
};
 
 
application::application(int argc,char** argv) : session(argc,argv,"airport")
{
  cout << "arg1: number of servicepoints (1 <= number <= 10)" << endl;
  cin >> nservicepoint;
  if ( (nservicepoint < 1) || (nservicepoint > 10) )
  {
    cerr << "too low or too high" << endl;
    exit(0);
  }
  cout << "arg2: average duration between arrival customers" << endl;
  cin >> between;
  cout << "arg3: average serving duration" << endl;
  cin >> serving;
  cout << "arg4: duration" << endl;
  cin >> duration;
}
 
// set up and run the simulation and prints the results.
 
int application::main()
{
  sim = new simulation();

  g1 = new generator(18976,3214,9111);
  g2 = new generator(5,34,119);

  // histogram for customer.
  toplevel* t1 = new toplevel(".time-spent");
  hcust = new histogram(t1,".h","-title time-spent");
  tk -> pack(hcust);

  // initialize servicepoints and their queues and histogram's
  char* buf = new char[BUFSIZ];
  char* path = new char[BUFSIZ];
  toplevel* t2 = new toplevel(".sizes");
  toplevel* t3 = new toplevel(".occupation");
  for (int i=0;i<nservicepoint;i++)
  {
    q[i] = new queue();
    sprintf(buf,"-type weighted -title size-%d",i+1);
    sprintf(path,".queue-%d",i+1);
    hqueue[i] = new histogram(t2,path,buf);
    tk -> pack(hqueue[i]);
    q[i] -> reportsize(hqueue[i]);
    servicepoint[i] = new resource(1.0);
    sprintf(buf,"-columnwidth 100 -columns 2 -type weighted -title occupied(%)-%d",i+1);
    sprintf(path,".reource-%d",i+1);
    hservicepoint[i] = new histogram(t3,path,buf);
    servicepoint[i] -> report(hservicepoint[i]);
    tk -> pack(hservicepoint[i]);
  }

  // initially a customer arrives and the service event is conditional
  arrival* arr = new arrival();
  sim -> schedule(arr,0.0);

  service* sr = new service();
  sim -> hold(sr);

  sim -> run(duration);

  delete hcust;
  for (i=0;i<nservicepoint;i++)
  {
    delete q[i];
    delete hqueue[i];
    delete hservicepoint[i];
  }
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

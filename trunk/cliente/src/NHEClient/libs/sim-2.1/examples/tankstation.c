// tankstation.c --
// <a href=tankstation.txt>Description</a>

#include <sim/sim.h>

simulation* sim;
generator* g;
histogram* lpgsize, *gasolinesize, *servicesize;
histogram* lpgused, *gasolineused, *serviceused;
resource* lpgpomp, *gasolinepomp, *pompservant;
queue* lpgqueue, *gasolinequeue, *servicequeue;


// definitions of the car, station and servant entities

// phases car
enum {ARRIVAL,CONNECT,FILL,DISCONNECT,DEPARTURE};

// type fuel
enum {LPG,GASOLINE};

class car : public entity
{
public :
  car(int ph,int k);
  virtual int operator()();
  void arrive(void);
  void connect(void);
  void fill(void);
  void disconnect(void);
  void depart(void);
private :
  int kind;
};


class servant : public entity
{
public :
  servant();
  virtual int operator()();
  void connect(car* c);
  void disconnect(car* c);
};


class station : public entity
{
public :
  station();
  virtual int operator()();
  void servegasoline(void);
  void servelpg(void);
};


// implementations of the entities

// constructor takes phase, kill flag and kind of fuel.

car::car(int ph,int k) : entity(ph,0,0,OK)
{
  kind = k;
}


// behavior from a car.

int car::operator()()
{
  switch (phase())
  {
    case ARRIVAL :
      arrive();
      return OK;
    case CONNECT :
      connect();
      return OK;
    case FILL :
      fill();
      return OK;
    case DISCONNECT :
      disconnect();
      return OK;
    case DEPARTURE :
      depart();
      return OK;
  }
  return FALSE;
}


// a car arrives, a new car is scheduled and the car is put in the
// appropriate queue.

void car::arrive(void)
{
  // 70% gasoline, 30% lpg
  car* c;
  if ( g -> probability(0.70) )
    c = new car(ARRIVAL,GASOLINE);
  else
    c = new car(ARRIVAL,LPG);

  // schedule next car
  sim  -> schedule(c,g -> uniform(2.0,8.0));

  // append and record size.
  if ( kind == GASOLINE )
    gasolinequeue -> append(this);
  else 
    lpgqueue -> append(this);
}


// connecting the car can't be the current.

void car::connect(void)
{
  cout << "should be in service queue" << endl;
}


// release pompServant and wait for the fill time.

void car::fill(void)
{
  pompservant -> release(1.0);
  phase(DISCONNECT);
  sim  -> wait(g -> uniform(5.0,9.0));
}


// append again.

void car::disconnect(void)
{
  servicequeue -> append(this);
}


// release pompservant and pomp and kill entity by setting the flag

void car::depart(void)
{
  pompservant -> release();
  if ( kind == GASOLINE )
    gasolinepomp -> release();
  else
    lpgpomp -> release();
  sim  -> terminate(this);
}


servant::servant() : entity()
{
}


// connect or disconnect car, if possible.

int servant::operator()()
{
  while ( (!(servicequeue -> empty())) && (pompservant -> available()) )
  {
    // remove and record size ans record occupied.
    car* c = (car *)servicequeue -> removefront();

    // appropriate action on a swich on phase of the car
    switch ( c -> phase() )
    {
      case CONNECT :
        connect(c);
        return OK;
      case DISCONNECT :
        disconnect(c);
        return OK;
    }
  }
  return FALSE;
}


// connect car to the pomp.

void servant::connect(car* c)
{
  pompservant -> acquire();
  c -> phase(FILL);
  sim -> schedule(c,g -> uniform(2.0,4.0));
}


// disconnect car from the pomp.

void servant::disconnect(car* c)
{
  pompservant -> acquire();
  c -> phase(DEPARTURE);
  sim -> schedule(c,g -> uniform(3.0,5.0));
}


// constructor

station::station() : entity()
{
}


// event routine serves lpq and gasoline, if possible.

int station::operator()()
{
  while ( (!(lpgqueue -> empty())) && (lpgpomp -> available()) )
    servelpg();
  while ( (!(gasolinequeue -> empty())) && (gasolinepomp -> available()) )
    servegasoline();
  return OK;
}


// acquires gasolinepomp and asks for a servant.

void station::servegasoline(void)
{
  car* c = (car *)gasolinequeue -> removefront();
  gasolinepomp -> acquire();
  c -> phase(CONNECT);
  servicequeue -> append(c);
}


// acquires lpgPomp and asks for a servant.

void station::servelpg(void)
{
  car* c = (car *)lpgqueue -> removefront();
  lpgpomp -> acquire();
  c -> phase(CONNECT);
  servicequeue -> append(c);
}


class application : public session
{
public :
  application(int argc,char** argv);
  virtual int main();
private :
  int totalday;
};
 
 
application::application(int argc,char** argv) : session(argc,argv,"tankstation")
{
}
 
// set up and run the simulation and prints the results.
 
int application::main()
{
  sim = new simulation();

  g = new generator(1,20,3291);

  // record sizes
  lpgsize = new histogram(".lpg","-type weighted -form statistics -title lpg_size");
  gasolinesize = new histogram(".gasoline","-type weighted -form statistics -title gasoline_size");
  servicesize = new histogram(".service","-type weighted -form statistics -title service_size");
  lpgsize -> file("lpgsize");
  gasolinesize -> file("gasolinesize");
  servicesize -> file("servicesize");
  tk -> pack(lpgsize) -> pack(gasolinesize) -> pack(servicesize);

  // record occupied
  toplevel* t = new toplevel(".occupation(%%)");
  lpgused = new histogram(t,".lpg","-columnwidth 10 -type weighted -form statistics -title lpg_occupied(%%)");
  gasolineused = new histogram(t,".gasoline","-columnwidth 10 -type weighted -form statistics -title gasoline_occupied(%%)");
  serviceused = new histogram(t,".service","-columnwidth 10 -type weighted -form statistics -title service_occupied(%%)");
  tk -> pack(lpgused) -> pack(gasolineused) -> pack(serviceused);

  lpgqueue = new queue();
  lpgqueue -> reportsize(lpgsize);
  gasolinequeue = new queue();
  gasolinequeue -> reportsize(gasolinesize);
  servicequeue = new queue();
  servicequeue -> reportsize(servicesize);

  lpgpomp = new resource(1);
  lpgpomp -> report(lpgused);
  gasolinepomp = new resource(3);
  gasolinepomp -> report(gasolineused);
  pompservant = new resource(2);
  pompservant -> report(serviceused);

  // initially a car arrives and the station and servant are conditional
  car* c;
  if ( g -> probability(0.70) )
    c = new car(ARRIVAL,GASOLINE);
  else
    c = new car(ARRIVAL,LPG);
  sim -> schedule(c,0.0);

  station* st = new station();
  sim -> hold(st);

  servant* sv = new servant();
  sim -> hold(sv);

  // run for 10000 car's
  sim -> run(10000);

  // determine covar. and corr. between service and gasoline,lpg
  analysis* a1 = new analysis(servicesize);
  a1 -> covariance(gasolinesize);
  analysis* a2 = new analysis(servicesize);
  a2 -> correlation(gasolinesize);
  analysis* a3 = new analysis(servicesize);
  a3 -> covariance(lpgsize);
  analysis* a4 = new analysis(servicesize);
  a4 -> correlation(lpgsize);
  
  delete a1;
  delete a2;
  delete a3;
  delete a4;
  delete lpgqueue;
  delete gasolinequeue;
  delete servicequeue;
  delete lpgsize;
  delete gasolinesize;
  delete servicesize;
  delete lpgused;
  delete gasolineused;
  delete serviceused;
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

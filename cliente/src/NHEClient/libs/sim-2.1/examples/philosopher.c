// philosopher.c --
// <a href=philospopher.txt>Description</a>
 
#include <sim/sim.h>
 
const double duration = 52*7*24.0;       // a year
const int number = 5;                    // philosophers
const int eatingtime = 2;                // 2 hours
const int thinkingtime = 5;              // 5 hours
simulation* sim;
generator* g;
resource* chopstick[number];
histogram* thinking;
 
 
enum {EATING,THINKING,WAITING};      // phases of a philosopher
   
class philosopher : public entity
{
public :
  philosopher(int ph,int i);  // constructor, taking phase and id
  virtual int operator()();   // function operator
  int eat();                  // eat event
  int think();                // think event
  int await();                // await event
private :
  int identity;
  generator* g;
};
   
philosopher::philosopher(int ph,int i) : entity(ph)
{
  identity = i;                  // set phase and identity 
  g = new generator(20,10,999);
}

int philosopher::operator()()
{
  switch (phase())         // what phase is the philosopher in ?
  {
  case EATING :
    return eat();          // the philosopher eats
  case THINKING :
    return think();        // the philosopher thinks
  case WAITING :
    return await();        // the philosopher waits
  }
  return FALSE;
}
   
int philosopher::eat()
{
  double t = g -> exponential(eatingtime);    // determine eating time
  sim -> wait(t);            // schedule this philosopher thinking
  phase(THINKING);           // set phase to thinking
  return OK;
}
   
int philosopher::think()
{
  chopstick[identity] -> release();            // release left chopstick
  chopstick[(identity+1)%number] -> release(); // release right
  double t = g -> exponential(thinkingtime);   // determine thinking time
  thinking -> sample(identity,t/duration*100);       // sample (%)
  sim -> wait(t);              // schedule this philosopher waiting
  phase(WAITING);                        // set phase on waiting
  return OK;
}
 
int philosopher::await()
{
  if ( (chopstick[identity] -> available()) &&    // available ?
     (chopstick[(identity+1)%number] -> available()) )
  {
    chopstick[identity] -> acquire();          // acquire left chopstick
    chopstick[(identity+1)%number] -> acquire();      // acquire right
    sim -> passivate(this);         // make passive
    sim -> activate(this);          // activate as eating
    phase(EATING);                  // set phase on eating
  }
  else if (!conditional())
    sim -> hold(this);              // add to conditional
  return OK;
}

class application : public session
{
public :
  application(int argc,char** argv);
  virtual int main();
};

application::application(int argc,char** argv) : session(argc,argv,"philosophers")
  {}
 
int application::main()
{
  sim = new simulation();
  g = new generator(80,20,19);          // three seeds
  thinking = new histogram(".h","-columns 5 -title thinkingtime");
  tk -> pack(thinking);                 // add to display;
  tk -> update();                       // update display;
  for (int i=0;i<number;i++)
  {
    chopstick[i] = new resource(1);     // create chopsticks
    philosopher* p = new philosopher(WAITING,i); // schedule each
    sim -> schedule(p,0);               // philosopher waiting
  }
  sim -> run(duration);                 // run for duration time units
  cout << (*thinking) << endl;          // print resulting histogram
  delete thinking;
  delete sim;
  return 0;
}
 
 
main(int argc,char** argv)
{
  session* s = new application(argc,argv);        // create the session
  s -> run();                                     // start the session
  exit(0);
}

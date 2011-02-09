// philosopher-ev.c --
// <a href=philosopher.txt>Description</a>
 
#include <sim/sim.h>

const double duration = 52*7*24.0;       // a year
const int number = 5;                    // philosophers
const int eatingtime = 2;                // 2 hours
const int thinkingtime = 5;              // 5 hours
simulation* sim;
generator* g;
resource* chopstick[number];
histogram* thinking;
 
class eat : public event 
{ 
public : 
  eat(int i);                 // constructor, taking identity
  virtual int operator()();   // function operator
private : 
  int identity;               // identity of the philosopher
};

class think : public event 
{ 
public :  
  think(int i);               // constructor, taking identity
  virtual int operator()();   // function operator
private : 
  int identity;               // identity of the philosopher
};
 
class await : public event 
{ 
public : 
  await(int i);               // constructor, taking identity
  virtual int operator()();   // function operator
private : 
  int identity;               // identity of the philosopher
};
 
eat::eat(int i) : event()
{
  identity = i;                            // set identity
}
 
int eat::operator()()
{
  double t = g -> exponential(eatingtime); // determine eating time
  think* th = new think(identity);         // create a thinking event
  sim -> schedule(th,t);                   // schedule thinking
  sim -> terminate(this);                  // terminate this eat event
  return OK;
}
 

think::think(int i) : event()
{
  identity = i;                                // set identity
}
 
int think::operator()()
{ 
  chopstick[identity] -> release();            // release left chopstick
  chopstick[(identity+1)%number] -> release(); // release right
  double t = g -> exponential(thinkingtime);   // determine thinking time
  thinking -> sample(identity,t/duration*100); // add a sample (%)
  await* aw = new await(identity);             // create await event
  sim -> schedule(aw,t);                 // schedule waiting
  sim -> terminate(this);                // terminate thinking
  return OK;   
}
 

await::await(int i) : event()
{
  identity = i;                                 // set identity
}
 
int await::operator()()
{
  if ( (chopstick[identity] -> available()) &&      // available   
     (chopstick[(identity+1)%number] -> available()) )
  {
    chopstick[identity] -> acquire();               // acquire left
    chopstick[(identity+1)%number] -> acquire();    // acquire right
    eat* e = new eat(identity);
    sim -> passivate(this);      // extract from conditional list
    sim -> schedule(e,0);        // schedule eat event immediately
    sim -> terminate(this);      // terminate await event
  }
  else if (!conditional())       // not on conditional list
    sim -> hold(this);           // add to conditional list
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
  g = new generator(20,10,999);         // three seeds
  thinking = new histogram(".h","-columns 5 -title thinkingtime");
  tk -> pack(thinking);                 // add to display;
  tk -> update();                       // update display;
  for (int i=0;i<number;i++)
  {
    chopstick[i] = new resource(1);     // create chopsticks
    await* aw = new await(i);           // schedule each
    sim -> schedule(aw,0);              // philosopher waiting
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

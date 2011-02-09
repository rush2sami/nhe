// philosopher-sc.c --
// <a href=philosopher.txt>Description</a>

#include <sim/sim.h>

enum {NUMBER=5,EAT_TIME=2,THINK_TIME=5};
 
simulation* sim;
screen* s;
kit* kt;
generator* g;
resource* chopstick[NUMBER];
histogram* thinking;
double duration;
 
enum {EATING,THINKING,WAITING};


class philosopher : public entity
{
public :
  philosopher(int ph,int id);
  virtual int operator()();
  void eat();
  void think();
  void await();
  void acquirechopsticks();
  void releasechopsticks();
  void waitonchopsticks();
private :
  int ident;
};



philosopher::philosopher(int ph,int id) : entity(ph)
{
  ident = id;
}

int philosopher::operator()()
{
  switch (phase())
  {
  case EATING :
    eat();
    return OK;
  case THINKING :
    think();
    return OK;
  case WAITING :
    await();
    return OK;
  }
  return FALSE;
}

// the philosopher starts to think after t units.

void philosopher::eat()
{
  double t = g -> exponential(EAT_TIME);
  phase(THINKING);
  sim -> wait(t);
}

// the philosopher releases the chopsticks, thinks for t units and 
// waits untill he can eat again.

void philosopher::think()
{
   releasechopsticks();
   double t = g -> exponential(THINK_TIME);
   thinking -> sample(ident,t*100.0/duration);
   cout << (*thinking);
   phase(WAITING);
   sim -> wait(t);
}

// the philosopher waits untill the chopsticks are available.
// If not he's made conditional (if he wasn't already).

void philosopher::await()
{
   if ( (chopstick[ident] -> available()) &&
       (chopstick[(ident+1)%NUMBER] -> available()) )
   {
     acquirechopsticks();
     sim -> passivate(this);
     phase(EATING);
     sim -> activate(this);
   }
   else if (!conditional())
   {
     waitonchopsticks();
     sim -> hold(this);
   }
}

// release the chopsticks

void philosopher::releasechopsticks()
{
  chopstick[ident] -> release();
  chopstick[(ident+1)%NUMBER] -> release();
  switch(ident)
  {
    case 0 :
      s -> move("c1",10,-40);
      s -> move("c2",30,30);
      s -> del("t1");
      s -> tag(s -> text(150,330,"thinking"),"t1");
      break;
    case 1 :
      s -> move("c2",-50,-20);
      s -> move("c3",50,-20);
      s -> del("t2");
      s -> tag(s -> text(400,475,"thinking"),"t2");
      break;
    case 2 :
      s -> move("c3",-30,30);
      s -> move("c4",-10,-40);
      s -> del("t3");
      s -> tag(s -> text(650,330,"thinking"),"t3");
      break;
    case 3 :
      s -> move("c4",20,50);
      s -> move("c5",-40,0);
      s -> del("t4");
      s -> tag(s -> text(570,50,"thinking"),"t4");
      break;
    case 4 :
      s -> move("c5",40,0);
      s -> move("c1",-20,50);
      s -> del("t5");
      s -> tag(s -> text(230,50,"thinking"),"t5");
      break;
  }
  kt -> update();
}

// acquire the chopsticks

void philosopher::acquirechopsticks()
{
  chopstick[ident] -> acquire();
  chopstick[(ident+1)%NUMBER] -> acquire();
  switch(ident)
  {
    case 0 :
      s -> move("c1",-10,40);
      s -> move("c2",-30,-30);
      s -> del("t1");
      s -> tag(s -> text(150,330,"eating"),"t1");;
      break;
    case 1 :
      s -> move("c2",50,20);
      s -> move("c3",-50,20);
      s -> del("t2");
      s -> tag(s -> text(400,475,"eating"),"t2");
      break;
    case 2 :
      s -> move("c3",30,-30);
      s -> move("c4",10,40);
      s -> del("t3");
      s -> tag(s -> text(650,330,"eating"),"t3");
      break;
    case 3 :
      s -> move("c4",-20,-50);
      s -> move("c5",40,0);
      s -> del("t4");
      s -> tag(s -> text(570,50,"eating"),"t4");
      break;
    case 4 :
      s -> move("c5",-40,0);
      s -> move("c1",20,-50);
      s -> del("t5");
      s -> tag(s -> text(230,50,"eating"),"t5");
      break;
  }
  kt -> update();
}


// wait on the chopsticks

void philosopher::waitonchopsticks()
{
  switch(ident)
  {
    case 0 :
      s -> del("t1");
      s -> tag(s -> text(150,330,"waiting"),"t1");
      break;
    case 1 :
      s -> del("t2");
      s -> tag(s -> text(400,475,"waiting"),"t2");
      break;
    case 2 :
      s -> del("t3");
      s -> tag(s -> text(650,330,"waiting"),"t3");
      break;
    case 3 :
      s -> del("t4");
      s -> tag(s -> text(570,50,"waiting"),"t4");
      break;
    case 4 :
      s -> del("t5");
      s -> tag(s -> text(230,50,"waiting"),"t5");
      break;
  }
  kt -> update();
}


class application : public session
{
public :
  application(int argc,char** argv);
  virtual int main();
  void initscreen();
};


application::application(int argc,char** argv) : session(argc,argv,"philosophers")
  {}

// Main creates the simulation, a generator, the philosophers, the
// screen and the chopsticks, before running the simulation.

int application::main()
{
  sim = new simulation();
  kt = tk;

  g = new generator(1,20910,33);
  for (int i=0;i<NUMBER;i++)
  {
    chopstick[i] = new resource(1.0);
    philosopher* p = new philosopher(WAITING,i);
    sim -> schedule(p,0.0);
  }
  initscreen();
  sim -> run(duration);
  delete thinking;
  delete sim;
  return 0;
}

// initialize the screen

void application::initscreen()
{
  toplevel* t1 = new toplevel(".query");
  query* q = new query(t1,".query","-text duration");
  tk -> pack(q);
  tk -> update();

  char* input;
  do
    input = q -> get();
  while (!input);
  duration = atof(input);

  q -> destroy();
  delete q;

  toplevel* t = new toplevel(".thinking_times");
  thinking = new histogram(t,".h","-columns 5 -title thinkingtimes(%%) -background white -width 190 -height 180");
  tk -> pack(thinking);
  s = new screen(".s","-width 400 -height 250 -worldx 800 -worldy 500 -background lightblue");

  s -> circle(400,250,200,"-fill black");
  s -> circle(275,295,25,"-fill white");
  s -> circle(275,295,20,"-fill white");
  s -> text(275,295,"1");
  s -> circle(400,395,25,"-fill white");
  s -> circle(400,395,20,"-fill white");
  s -> text(400,395,"2");
  s -> circle(525,295,25,"-fill white");
  s -> circle(525,295,20,"-fill white");
  s -> text(525,295,"3");
  s -> circle(475,145,25,"-fill white");
  s -> circle(475,145,20,"-fill white");
  s -> text(475,145,"4");
  s -> circle(325,145,25,"-fill white");
  s -> circle(325,145,20,"-fill white");
  s -> text(325,145,"5");
  s -> tag(s -> line(270,210,320,240,"-fill white"),"c1");
  s -> tag(s -> line(300,380,340,340,"-fill white"),"c2");
  s -> tag(s -> line(460,340,500,380,"-fill white"),"c3");
  s -> tag(s -> line(530,210,480,240,"-fill white"),"c4");
  s -> tag(s -> line(400,115,400,175,"-fill white"),"c5");
  s -> tag(s -> text(150,330,"waiting"),"t1");
  s -> tag(s -> text(400,475,"waiting"),"t2");
  s -> tag(s -> text(650,330,"waiting"),"t3");
  s -> tag(s -> text(570,50,"waiting"),"t4");
  s -> tag(s -> text(230,50,"waiting"),"t5");
  tk -> pack(s);

  toplevel* t2 = new toplevel(".control");
  control* ct = new control(t2,".control","-factor 5");
  tk -> pack(ct);
  toplevel* t3 = new toplevel(".clock");
  simulationclock* cl = new simulationclock(t3,".clock","-width 100 -height 30");
  tk -> pack(cl);

  tk -> update();
}

      
// create and start the session

int main(int argc,char** argv)
{
  session* s = new application(argc,argv);
  s -> run();
  exit(0);
}

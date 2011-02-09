// reliability.c --
// <a href=reliability.txt>Description</a>

#include <sim/sim.h>

#define REPLACEMENTTIME 0.1
#define NRUNS 100

generator* g;
simulation* sim;


class stop : public event
{
  public :
    stop() : event()
      {}
    virtual int operator()()
    { sim -> quit(); return OK; }
};

class failure : public event
{
  public :
    failure() : event()
      {}
    virtual int operator()() // one part goes down.
    {
      double lifetime = g -> weibull(0.9080,1.4355);
      if (lifetime < REPLACEMENTTIME)
      {
        // both parts down.
        stop* s = new stop();
        sim -> schedule(s,lifetime);
        return FALSE;
      }
      else
      {
        // schedule next failure.
        sim -> wait(lifetime);
        return OK;
      }
    }
};
     
class application : public session
{
public :

application(int argc,char** argv) : session(argc,argv,"reliability")
  {}

int main()
{
  sim = new simulation();
  screen* sc = new screen(".screen","-background blue");
  tk -> pack(sc);
  histogram* h1 = new histogram(sc,".h1","-start 0 -columnwidth 10 -columns 20 -type frequency -form graph -title lifetime -width 490 -height 180 -background white -foreground blue");
  sc -> window(300,150,h1);
  histogram* h2 = new histogram(sc,".h2","-start 0 -columnwidth 10 -columns 20 -type frequency -form statistics -title lifetime -width 200 -height 50 -background white -foreground blue");
  sc -> window(680,100,h2);
  histogram* p = new histogram(sc,".p","-start 0 -columnwidth 5 -columns 20 -type frequency -form graph -title P(lifetime>t) -width 490 -height 180 -background white -foreground blue");
  sc -> window(300,360,p);
  analysis* a = new analysis(h1);
  sc -> window(680,200,a -> format(sc,".a","-width 200 -height 50 -background white -foreground blue"));
  analysis* ap = new analysis(p);
  sc -> window(680,360,ap -> format(sc,".ap","-width 200 -height 50 -background white -foreground blue -plot black"));

  sc -> window(400,540,sim -> format(sc,".sim","-width 200 -height 60 -background white -foreground blue"));
  double tlast = 0;
  for (int i=0;i<NRUNS;i++)
  {
    // create new generator
    int seed = (int)tlast;
    g = new generator(seed+1,seed+2,seed+3);
    double lifetime = g -> weibull(0.9080,1.4355);
    failure* f = new failure();
    sim -> schedule(f,lifetime);
    sim -> run(); // until quit is called

    // sample time run
    double t = sim -> clock() - tlast;
    tlast = sim -> clock();
    h1 -> sample(t);
    h2 -> sample(t);
    cout << (*h1);
    cout << (*h2);

    // calculate confidence interval
    a -> confidence("percent 95");

    // sample P(X>t) in %
    int j = 0;
    while(j < t)
    {
      p -> sample(j,100.0/NRUNS);
      j += 5;
    }
    cout << (*p);

    // fit exponential
    ap -> exponential();
  }

  delete h1;
  delete h2;
  delete a;
  delete ap;
  delete p;
  delete sim;
  return 0;
}
};

int main(int argc,char** argv)
{
  session* s = new application(argc,argv);
  s -> run();
  exit(0);
}

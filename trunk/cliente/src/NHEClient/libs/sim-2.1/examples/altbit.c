// altbit.c --
// <a href=altbit.txt>Description</a>

#include <sim/sim.h>

#define TD_DATA 0.25
#define TD_ACK 0.25
#define TO_VAL 0.52

enum {DATA_SIZE=1024,ACK_SIZE=1024,LINE_SPEED=4096};

enum {TX,RX,ACK,TXGARBLED,RXGARBLED};

generator* str;
simulation* sim;
kit* kt;
screen* s;
double datarxd = 0.0;
double p;
double ber,duration = 1000.0;
int txseqnum = 0;
int rxseqnum = 0;
int numberfailures = 0;
int numbersuccessful = 0;



class timeout : public entity
{
public :
  timeout();
  virtual int operator()();
};


class packet : public entity
{
public :
  packet(int ph);
  virtual int operator()();
  void show(int p);
private :
  timeout* to;
};


timeout::timeout() : entity()
{
}

int timeout::operator()()
{
  // schedule a new packet with no delay.
  packet* fr = new packet(TX);
  sim -> schedule(fr,0.0);

  // adjust counter
  numberfailures++;

  // show on the screen
  s -> del("event");
  s -> tag(s -> text(240,135,"TIMEOUT"),"event");
  char* buf = new char[BUFSIZ];
  sprintf(buf,"%d",numberfailures);
  s -> del("failures");
  s -> tag(s -> text(600,25,buf),"failures");
  kt -> update();
//  delete buf;  // NO      <----------
  delete[] buf;  // YES     <----------

  for (int i=0;i<30;i++)
  {
    s -> tag(s -> line(235,165,230,155,"-fill red"),"timeout");
    kt -> update();
    s -> del("timeout");
    s -> tag(s -> line(240,160,240,150,"-fill red"),"timeout");
    kt -> update();
    s -> del("timeout");
    s -> tag(s -> line(245,165,250,155,"-fill red"),"timeout");
    kt -> update();
    s -> del("timeout");
    kt -> update();
  }

  // terminate timeout.
  sim -> passivate(this);
  sim -> terminate(this);
  return OK;
}


packet::packet(int ph) : entity(ph)
{
  to = NULL;
}

int packet::operator()(void)
{
  switch (phase())
  {
    case TX :
      // schedule a timeout.
      to = new timeout();
      sim -> schedule(to,TO_VAL);

      // schedule a receive with probability p.
      if (str -> probability(p))
      {
        phase(RX);
        sim -> wait(TD_DATA);
        show(TX);
      }
      else
      {
        show(TXGARBLED);
        sim -> terminate(this);
      }
      return OK;
    case RX :
      // if seq. numbers agree, schedule an acknowledgement.
      if (rxseqnum == txseqnum)
      {
        rxseqnum = !rxseqnum;
        datarxd += DATA_SIZE;
        phase(ACK);
        sim -> wait(TD_ACK);
        show(RX);
      }
      else
      {
        show(RXGARBLED);
        sim -> terminate(this);
      }
      return OK;
    case ACK :
      // adjust counter 
      numbersuccessful++;

      // passivate the timeout and send another packet.
      sim -> passivate(to);
      sim -> terminate(to);
      txseqnum = !txseqnum;
      phase(TX);
      sim -> wait(0);
      show(ACK);
      return OK;
  }
  return FALSE;
}

// show on the screen

void packet::show(int p)
{
  int i = 0;
  char* buf;
  switch(p)
  {
    case TX :
      s -> del("event");
      s -> tag(s -> text(220,270,"TX"),"event");
      s -> tag(s -> square(220,250,10,"-fill black"),"packet");
      while (i<90)
      {
        s -> move("packet",4,0);
        kt -> update();
        i++;
      }
      s -> del("event");
      s -> tag(s -> text(580,230,"RX"),"event");
      kt -> update();
      break;
    case RX :
      s -> del("event");
      s -> tag(s -> text(580,270,"ACK"),"event");
      s -> tag(s -> square(580,250,10,"-fill black"),"packet");
      while (i<90)
      {
        s -> move("packet",-4,0);
        kt -> update();
        i++;
      }
      break;
    case ACK :
      buf = new char[BUFSIZ];
      sprintf(buf,"%d",numbersuccessful);
      s -> del("successes");
      s -> tag(s -> text(450,25,buf),"successes");
//      delete buf;  // NO      <----------
      delete[] buf;  // YES     <----------

      break;
    case TXGARBLED :
      s -> del("event");
      s -> tag(s -> text(220,270,"TX"),"event");
      s -> tag(s -> square(220,250,10,"-fill black"),"packet");
      while (i<45)
      {
        s -> move("packet",4,0);
        kt -> update();
        i++;
      }
      break;
    case RXGARBLED :
      s -> del("event");
      s -> tag(s -> text(580,270,"ACK"),"event");
      s -> tag(s -> square(580,250,10,"-fill black"),"packet");
      while (i<45)
      {
        s -> move("packet",-4,0);
        kt -> update();
        i++;
      }
      break;
  }
  s -> del("packet");
  kt -> update();
}


class application : public session
{
public :
  application(int argc,char** argv);
  virtual int main();
  void initscreen();
};


application::application(int argc,char** argv) : session(argc,argv,"alternating bit protocol")
  {}

// initialize and run

int application::main()
{
  // set up
  sim = new simulation();
  s = new screen(".screen","-height 500 -worldy 500");
  tk -> pack(s);
  initscreen();
  toplevel* t1 = new toplevel(".control");
  control* c = new control(t1,".control");
  toplevel* t2 = new toplevel(".clock");
  simulationclock* cl = new simulationclock(t2,".clock");
  tk -> pack(c);
  tk -> pack(cl);
  tk -> update();
  kt = tk;
  str = new generator(1,21010,209);

  // schedule a transmitted packet and run for duration units.
  packet* fr = new packet(TX);
  sim -> schedule(fr,0.0);
  sim -> run(duration);

  // results
  char* buf = new char[BUFSIZ];
  sprintf(buf,"throughput = %f bps",(datarxd/duration));
  cout << buf << endl;
  s -> text(700,25,buf);
//  delete buf;  // NO      <----------
  delete[] buf;  // YES     <----------
  delete sim;
  return 0;
}

void application::initscreen()
{
  // input
  query* q = new query(s,".query","-text bit-error-rate ( <= 0.0001 )");
  s -> tag(s -> window(400,250,q),"query");
  tk -> update();
  char* in;
  do
    in = q -> get();
  while (!in);
  ber = atof(in);
  p = 1.0 - (ber * DATA_SIZE);
  if (p < 0.0)
  {
    cout << "bit error rate too high" << endl;
    exit(0);
  }
  char* buf = new char[BUFSIZ];
  sprintf(buf,"line speed = %d  Bit Error Rate = %f  ",LINE_SPEED,ber);
  cout << buf;
  s -> text(200,25,buf);
  s -> del("query");
//  delete buf;  // NO      <----------
  delete[] buf;  // YES     <----------

  // layout
  s -> square(150,250,50,"-fill white");
  s -> text(150,250,"sender");
  s -> square(650,250,50,"-fill white");
  s -> text(650,250,"receiver");
  s -> line(200,250,600,250);
  s -> circle(240,180,20,"-fill red");
  s -> line(230,190,220,200,"-fill red -width 3");
  s -> line(250,190,260,200,"-fill red -width 3");
  s -> line(230,170,210,150,"-fill red -width 2");
  s -> line(250,170,270,150,"-fill red -width 2");
  s -> line(240,180,240,160,"-fill black");
  s -> line(240,180,260,180,"-fill black");
  s -> text(400,25,"successes : ");
  s -> text(550,25,"failures : ");
  s -> line(0,50,800,50);
}


// create and start the session

int main(int argc,char** argv)
{
  session* s = new application(argc,argv);
  s -> run();
  exit(0);
}

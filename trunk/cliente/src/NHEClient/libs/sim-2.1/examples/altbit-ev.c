// altbit-ev.c --
// <a href=altbit.txt>Description</a>

#include <sim/sim.h>

#define TD_DATA 0.25
#define TD_ACK 0.25
#define TO_VAL 0.52

enum {DATA_SIZE=1024,ACK_SIZE=1024,LINE_SPEED=4096};

generator* str;
simulation* sim;
double datarxd = 0.0;
double p;
double ber;
int txseqnum = 0;
int rxseqnum = 0;


class transmit : public event
{
public :
  transmit();
  virtual int operator()();
};


class receive : public event
{
public :
  receive(int txseqnum);
  virtual int operator()();
private :
  int sn;
};


class timeout : public event
{
public :
  timeout();
  virtual int operator()();
};


class acknowledge : public event
{
public :
  acknowledge();
  virtual int operator()();
};


// keep pointer to the timeout
timeout* to;


transmit::transmit() : event()
{
}
  
int transmit::operator()()
{
  // schedule a time out.
  to = new timeout();
  sim -> schedule(to,TO_VAL);

  // schedule a receive with probability p.
  if (str -> probability(p))
  {
    receive* r = new receive(txseqnum);
    sim -> schedule(r,TD_DATA);
  }

  sim -> terminate(this);
  return OK;
}


receive::receive(int txseqnum) : event()
{
    sn = txseqnum;
}

int receive::operator()()
{
  // if sequencenumbers agree acknowledge
  if (rxseqnum == sn)
  {
    rxseqnum = !rxseqnum;
    datarxd += DATA_SIZE;
    acknowledge* a = new acknowledge();
    sim -> schedule(a,TD_ACK);
  }
  sim -> terminate(this);
  return OK;
}


timeout::timeout() : event()
{
}

int timeout::operator()()
{
  // passivate this and transmit a new packet.
  sim -> passivate(this);
  transmit* t = new transmit();
  sim -> schedule(t,0);
  sim -> terminate(this);
  return OK;
}


acknowledge::acknowledge() : event()
{
}

int acknowledge::operator()()
{
  // passivate the timeout and transmit a new packet.
  sim -> passivate(to);
  sim -> terminate(to);
  txseqnum = !txseqnum;
  transmit* t = new transmit();
  sim -> schedule(t,0);
  sim -> terminate(this);
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
 
 
application::application(int argc,char** argv) : session(argc,argv,"airport")
{
  cout << "bit error rate ( <= 0.0001 ) : " << endl;
  cin >> ber;
  p = 1.0 - (ber * DATA_SIZE);
  if (p < 0.0)
  {
    cout << "bit error rate too high" << endl;
    exit(0);
  }
  cout << "line speed = " << LINE_SPEED << ", Bit Error Rate = " << ber << endl;
}
 
int application::main()
{
  sim = new simulation();
  str = new generator(1,21010,209);

  // schedule a transmit event and run for DURATION units.
  transmit* t = new transmit();
  sim -> schedule(t,0.0);
  sim -> run(1000.0);

  // results
  cout << "throughput = " << (datarxd/1000.0) << " bps" << endl;

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

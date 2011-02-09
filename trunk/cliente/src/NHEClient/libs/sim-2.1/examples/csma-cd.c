// csma-cd.c --
// <a href=csma-cd.txt.txt>Description</a>

#include <sim/sim.h>

// maximal number of stations on the LAN

#define MAX 200 

////////////////////////////////////////////////////////////////////////
//								      //
// globals                                                            //
//								      //
////////////////////////////////////////////////////////////////////////

histogram* wtt,*delay,*efficiency,*throughput;

generator* g1; // Arrival process
generator* g2; // For calculating backoff

simulation* sim;

double lastdelay,lastclock;
int numstations;
int numblocks;
double transient,duration;
double idletime;
double packetsize;


////////////////////////////////////////////////////////////////////////
//								      //
// definition of a station.       				      //
//								      //
////////////////////////////////////////////////////////////////////////

class packet;

// states of a station
enum {IDLE,WAIT,LISTEN,CONTENTION,FINISH};

class station : public entity
{
public :
  station(int id,int ph);
  virtual ~station();
  virtual int operator()();
  void wait(void);
  int trytransmit(void);
  int receive(void);
  void tryagain(void);
  void finish(void);
  void transmit(int dir,int arr,int dep);
  void retransmit(void);
  void reset(void);
  void zero(void);
  int finished(void);
  double retranstime(void);
  int channelclear(void);
  station* actrec;	// The entity representingcurrent tx request
  packet* end[2];	// Frames propagating from this station

  // inline members for manipulating private data.
  int nmbretrans(void)
    { return numberretrans; }
  void nmbretrans(int n)
    { numberretrans = n; }
  void passingleft(void)
    { toleft++; }
  void passingright(void)
    { toright++; }
  void leavingleft(void)
    { toleft--; }
  void leavingright(void)
    { toright--; }
  void last(int l)
    { lastok = l; }
  int last(void)
    { return lastok; }
  void go(void)
    { total++; }
  void success(void)
    { successful++; }
  void failure(void)
    { failed++; }
  int totpackets(void)
    { return total; }
  int succ(void)
    { return successful; }
  int fail(void)
    { return failed; }
private :
  int numberretrans;	// Number of retransmissions so far.
  int successful;	// Number of OK trans from this station
  int total;		// Total number of packets offered
  int failed;		// Number failed
  int toleft;		// A packet is passing this station
  int toright;		// A packet is passing this station
  int lastok;		// Whether last tx was succesful
  int identity;		// Id of the station
};


////////////////////////////////////////////////////////////////////////
//								      //
// definition of the ethernet.       				      //
//								      //
////////////////////////////////////////////////////////////////////////

class ethernet
{
public :
  ethernet(double fs,int nb,int ns,double it);
  virtual ~ethernet();
  station* which(int i);

  // inline members for manipulating private data.
  void add(station* s,int i)
    { (LANuser[i] -> actrec) = s; }
  double idle(void)
    { return (g1 -> exponential(idletime)); }
  int maxretr(void)
    { return maxretrans; }
  int maxval(void)
    { return maxvalue; }
  double neighbourdelay(void)
    { return nbdelay; }
  double delay(void)
    { return totaldelay; }
  void totdelay(double t)
    { totaldelay += t; }
  void zerodelay(void)
    { totaldelay = 0.0; }
  int numberstations(void)
    { return numstations; }
  int waiting(void)
    { return numwaiting; }
  void waiting(int n)
    { numwaiting = n; }
  double slotsz(void)
    { return slotsize; }
  double packetsz(void)
    { return packetsize; }
  double factor(void)
    { return scalefactor; }
  double packettrans(void)
    { return packettranstime; }
  double burst(void)
    { return noiseburst; }
  void success(void)
    { numsuccess++; }
  void numbersuccess(long s)
    { numsuccess = s; }
  long numbersuccess(void)
    { return numsuccess; }
  void backoff(void)
    { numbackoffs++; }
  long numberbackoffs(void)
    { return numbackoffs; }
private :
  double v;                     // signal propogation speed (m/s)
  double slotsize;              // The slot size (mseconds)
  double packetsize;             // The size of a packet (bytes)
  double LANspeed;              // LAN speed bps
  int maxretrans;               // Max nnmber of retransmissions
  int maxvalue;                 // Max retrans delay = 2^10-1 slots 
  long numbackoffs;             // Total number of retransmissions 
  int numblocks;                // Number of blocks in a run 
  double noiseburst;            // Duration of a noise burst 920 bits) 
  int buffersize;               // The size of the inpu buffers 
  double duration;              // The duration of steady state
  double transient;             // The duration of transient state
  double scalefactor;           // Assume the clock is in milliseconds
  double totaldelay;            // The total delay of all packets 
  double echodelay;             // Delay for echoing sim time to scrn 
  int numavg;                   // The number of moving averages
  int numstations;              // Number of users on the LAN
  double idletime;              // Frame inter-arrival time 
  double packettranstime;        // Time to transmit packet 
  double nbdelay;               // Time to travel between stations
  station* LANuser[MAX];        // The set of all stations on the LAN
  int numwaiting;               // The number of stations waiting to transmit
  long numsuccess;              // The number of successful transmissions
  double cablelength;           // The cable length
};

ethernet* net;    // keep global pointer


////////////////////////////////////////////////////////////////////////
//								      //
// definition of a packet.       				      //
//								      //
////////////////////////////////////////////////////////////////////////

// States of a packet 
enum {ARRIVAL,DEPARTURE};

// direction
enum {LEFT,RIGHT};


class packet : public entity
{
public :
  packet(int dir,int arr,int dep,int ph);
  virtual int operator()();
  void arriveleft(void);
  void arriveright(void);
  void departleft(void);
  void departright(void);
  void invalidate(int id);

  // inline members for manipulating private data.
  int arriving(void)
    { return arrstat; }
  void arriving(int stat)
    { arrstat = stat; }
  void corrupt(void)
    { corrupted = OK; }
private :
  int corrupted;         // Has this packet clashed with another
  int source;            // The sender of the packet 
  int direction;         // The direction of propogation 
  int arrstat;           // The station ariving at
  int depstat;           // The station leaving from
};


////////////////////////////////////////////////////////////////////////
//								      //
// implementation of the ethernet.    				      //
//								      //
////////////////////////////////////////////////////////////////////////

// ethernet constructor, taking the packet size, the number of blocks and
// stations and the mean idle time between transmitting.

ethernet::ethernet(double fs,int nb,int ns,double it)
{
  v = 201000000.0;
  slotsize = 0.0512;
  LANspeed = 10000000.0;
  maxretrans = 16;
  maxvalue   = 10;  
  scalefactor = 1000.0; 
  totaldelay  = 0.0; 
  noiseburst = 20.0/LANspeed;
  packetsize = fs;
  numblocks = nb;
  numstations = ns;
  numwaiting = 0;
  numsuccess = 0L;
  idletime = it;
  cablelength = 10000.0;
  nbdelay = scalefactor * cablelength / (numstations-1) / v;
  packettranstime = scalefactor * packetsize * 8.0 / LANspeed;
  for (int i=0;i<MAX;i++)
    LANuser[i] = new station(i,IDLE);
}


// ethernet destructor.

ethernet::~ethernet()
{
  for (int i=0;i<numstations;i++)
    if (LANuser[i])
      delete LANuser[i];
}


// which retourneert station i or NULL;

station* ethernet::which(int i)
{
  if (i<numstations && i>=0)
    return  LANuser[i];
  else 
    return NULL;
}


////////////////////////////////////////////////////////////////////////
//								      //
// implementation of a packet.        				      //
//								      //
////////////////////////////////////////////////////////////////////////

// constructor, taking direction, identity arriving and departing station
// and the phase.

packet::packet(int dir,int arr,int dep,int ph) : entity(ph)
{
  corrupted = FALSE;
  source = dep;
  direction = dir;
  arrstat = arr;
  depstat = dep;
}


// Model a packet travelling along the cable, visiting neighbouring 
// stations. 

int packet::operator()()
{
  switch (phase())
  {
    case ARRIVAL:
      // If this station is transmitting then we have a conflict. 
      if ( ((net -> which(arrstat)) -> phase() == LISTEN) &&
           ((net -> which(arrstat)) -> last()) )
      {
         // chop current packet.
         invalidate(arrstat);
      }

      // Frame going from right to left
      if (direction == LEFT)
        arriveleft();
      // Frame going from left to right. 
      else
        arriveright();
      return OK;
    case DEPARTURE:
      // Are we transmitting this packet? 
      if (source == depstat)
      {
        // Dereference pointers 
        ((net -> which(depstat)) ->  end[direction]) = NULL;

        // Has this station now just finished transmitting? 
        if ((net -> which(depstat)) -> finished())
        {
          // Was this packet ok? 
          if (!corrupted)
          {
            if ((net -> which(depstat)) -> phase() == FINISH )
              sim -> activate((net -> which(depstat)) -> actrec);
          }
        }
      }
      // going left ?
      if (direction == LEFT)
        departleft();
      else // The packet has gone right 
        departright();
      return OK;
  }
  return FALSE;
}


// a packet has arrived from the left.

void packet::arriveleft(void)
{
  // A packet is transitting this station - set flag 
  (net -> which(arrstat)) -> passingleft();

  // Send a packet left if possible.
  if (arrstat > 0)
  {
    arrstat--;
    sim -> wait(net -> neighbourdelay());
  }
  else
  // The start of the packet has reached the first station 
  {
    // Don't free packet data until last packet reaches end of cable 
    sim -> terminate(this);
  }
}


// a packet has arrived from the right.

void packet::arriveright(void)
{
  // A packet is transitting from left to right - Set flag. 
  (net -> which(arrstat)) -> passingright();

  // Send a packet right if possible 
  if (arrstat < (net -> numberstations()-1) )
  {
    arrstat++;
    sim -> wait(net -> neighbourdelay());
  }
  else
  // The start of the packet has reached the last station.
  {
  // Don't free packet data until end of packet reaches end of cable
    sim -> terminate(this);
  }
}


// a packet has gone to the left.

void packet::departleft(void)
{
  // The packet has gone now - reset flag 
  (net -> which(depstat)) -> leavingleft();

  // Send end of packet left if possible 
  if (depstat > 0)
  {
    depstat--;
    sim -> wait(net -> neighbourdelay());
  }
  else
  {
    // We've reached the end of the line 
    sim -> terminate(this);
  }
}


// a packet has gone to the right.

void packet::departright(void)
{
  (net -> which(depstat)) -> leavingright();

  // Send end of packet right if possible 
  if (depstat < (net -> numberstations()-1) )
  {
    depstat++;
    sim -> wait(net -> neighbourdelay());
  }
  else
  {
    // We've reached the end of the line 
    sim -> terminate(this);
  }
}


// Curtail the transmission of the current packet due to collision 

void packet::invalidate(int id)
{
  // This transmission has failed 
  (net -> which(id)) -> last(FALSE);

  // Flag packet as corrupted and abort transmission 
  packet* frm = (net -> which(id)) -> end[LEFT];
  if (frm)
  {
    frm -> corrupt();
    // Remove from calendar 
    sim -> passivate(frm);

    // Chop packet - add noise burst 
    sim -> schedule(frm,(net -> burst()));
  }

  frm = (net -> which(id)) -> end[RIGHT];
  if (frm)
  {
    frm -> corrupt();
    // Remove from calendar 
    sim -> passivate(frm);

    // Chop packet - add noise burst 
    sim -> schedule(frm,(net -> burst()));
  }
}


////////////////////////////////////////////////////////////////////////
//								      //
// implementation of a station.        				      //
//								      //
////////////////////////////////////////////////////////////////////////

// constructor, taking the identity and phase.

station::station(int id,int ph) : entity(ph)
{
  numberretrans = 0;
  successful = 0;
  total = 0;
  failed = 0;
  toright = FALSE;
  toleft = FALSE;
  lastok = FALSE;
  actrec = NULL;
  end[0] = NULL;
  end[1] = NULL;
  identity = id;
}


// destructor dereferences pointers.

station::~station(void)
{
  actrec = NULL;
  end[0] = NULL;
  end[1] = NULL;
}
  
  
// Model a station on the LAN in state :                     
//  Idle - doing nothing                                          
//  Wait - waiting for a moment in the traffic to transmit         
//  Finish - finished transmitting a packet                      
//  Contention - has clashed with another packet                  
//  Listen - packet transmitting, making sure received everywhere  

int station::operator()(void)
{
  switch (phase())
  {
    case IDLE:
      wait();
      return OK;
    case WAIT:
      (void)trytransmit();
      return OK;
    case LISTEN:
      (void)receive();
      return OK;
    case CONTENTION:
      tryagain();
      return OK;
    case FINISH:
      finish();
      return OK;
  }
  return FALSE;
}


// station starts to transmit.

void station::wait(void)
{
  (net -> which(identity)) -> go();
  stamp();

  // Add number waiting to tx 
  int numwaiting = net -> waiting();
  wtt -> sample(numwaiting);
  numwaiting++;
  net -> waiting(numwaiting);

  // Wait for free channel 
  phase(WAIT);
  (net -> which(identity)) -> phase(WAIT);
  sim -> passivate(this);
  sim -> hold(this);
}


// station try's to transmit, return OK, if possible.

int station::trytransmit(void)
{
  // Tramsmit only if channel free.
  if (channelclear())
  {
    // Add number waiting to tx 
    int numwaiting = net -> waiting();
    wtt -> sample(numwaiting);
    numwaiting--;
    net -> waiting(numwaiting);

    // Two packets - one propogating to the left and one to the right. 
    transmit(LEFT,identity-1,identity);
    transmit(RIGHT,identity+1,identity);

    // Listen for two slots 
    (net -> which(identity)) -> phase(LISTEN);
    (net -> which(identity)) -> last(OK);
    phase(LISTEN);
    sim -> passivate(this);
    sim -> schedule(this,2.0*net -> slotsz());
    return OK;
  }
  else
    return FALSE;
}


// receive a packet, if successfull return OK, else FALSE.

int station::receive(void)
{
  if ((net -> which(identity)) -> last())
  {
    // Transmission succesfull 
    (net -> which(identity)) -> success();
    net -> success();

    // time spent queueing 
    net -> totdelay((this -> timespent())-2.0*(net->slotsz()));

    if (finished()) 
    {
      // Transmission finished 
      phase(FINISH);
    }
    else 
    {
      // Transmission on going: wait for finish 
      (net -> which(identity)) -> phase(FINISH);
      phase(FINISH);
      sim -> passivate(this);
    }
    return OK;
  }
  else
  {
    retransmit();
    return FALSE;
  }
}


// transmit again.

void station::tryagain(void)
{
  // Add number waiting to tx 
  int numwaiting = net -> waiting();
  wtt -> sample(numwaiting);
  numwaiting++;
  net -> waiting(numwaiting);
  (net -> which(identity)) -> phase(WAIT);
  phase(WAIT);
  sim -> passivate(this);
  sim -> hold(this);
}


// finish resets station and start to transmit again.

void station::finish(void)
{
  reset();
  phase(IDLE);
  numberretrans = 0;
  sim -> wait((net -> idle()));
}


// reset station.

void station::reset(void)
{
  (net -> which(identity)) -> nmbretrans(0);
  (net -> which(identity)) -> phase(IDLE);
  (net -> which(identity)) -> end[LEFT] = NULL;
  (net -> which(identity)) -> end[RIGHT] = NULL;
}


// make #transmissions 0.

void station::zero(void)
{
  total = 0;
  successful = 0;
  failed = 0;
}


// Transmit a packet on the LAN 

void station::transmit(int dir,int arr,int dep)
{
  if (dir == LEFT)
  {
    // Is there any one to transmit to ? 
    if (dep < 1)
      return;
    else
      (net -> which(dep)) -> passingleft();
  }

  if (dir == RIGHT)
  {
    // Is there anyone to transmit to ? 
    if (dep > (net -> numberstations())-2)
      return;
    else
      (net -> which(dep)) -> passingright();
  }

  // Schedule arrival at neighbour and departure from this station
  packet* frm1 = new packet(dir,arr,dep,ARRIVAL);
  sim -> schedule(frm1,(net -> neighbourdelay()));

  packet* frm2 = new packet(dir,arr,dep,DEPARTURE);
  sim -> schedule(frm2,net -> packettrans());

  // Keep pointers in case of contention 
  ((net -> which(dep)) -> end[dir]) = frm2;
}


// Retransmit packet after collision 

void station::retransmit(void)
{
   station* stat = (net -> which(identity)) -> actrec;
   sim -> passivate(stat);

   // Have maximum number of attempts been made? 
   if (((net -> which(identity)) -> nmbretrans()) >= (net -> maxretr()))
   {
      (net -> which(identity)) -> failure();

      // The transmission failed - get rid of the transmission request. 
      stat -> phase(FINISH);

      // Schedule the next transmission attempt just after the noise burst 
      sim -> schedule(stat,(net -> burst()) * 1.005);
   }
   else //  Try again.
   {
      // Count the number of backoffs 
      net -> backoff();

      // Set up retransmission 
      stat -> phase(CONTENTION);
      (net -> which(identity)) -> phase(CONTENTION);

      // Schedule the next transmission attempt just after the noise burst 
      sim -> schedule(stat,((net ->burst()) * 1.005) + retranstime()); 
   }
}


// Has this station finished transmitting a packet left and right 

int station::finished(void)
{
   if ((net -> which(identity) -> end[LEFT]) != NULL)
     return FALSE;
   if ((net -> which(identity) -> end[RIGHT]) != NULL)
     return FALSE;
   return OK;
}


// Calculate the time to backoff - binary exponential 

double station::retranstime(void)
{
   int num = (net -> which(identity) -> nmbretrans()) + 1;
   (net -> which(identity)) -> nmbretrans(num);
   if (num > (net -> maxval()))
      num = (net -> maxval());

   // 2 to the power num 
   double maximum = (double)(1 << num);
   long t = (long)(g2 -> uniform(0.0,maximum));

   return (net -> slotsz() * t);
}


// channel clear?

int station::channelclear(void)
{
   if ((net -> which(identity) -> toleft) != 0)
     return FALSE;
   if ((net -> which(identity) -> toright) != 0)
     return FALSE;
   return OK;
}


////////////////////////////////////////////////////////////////////////
//                                                                    //
// main runs for the transient phase, for numblocks steady phases and //
// gives the results. 						      //
//                                                                    //
////////////////////////////////////////////////////////////////////////

class application : public session
{
public :
  application(int argc,char** argv);
  virtual int main();
  void log();
  void batch();
  void results();
};


// constructor gets the parameters, creates the environment and runs for the
// transient phase.

application::application(int argc,char** argv) : session(argc,argv,"csma-cd")
{
  cout << "arg1: number of stations on LAN" << endl;
  cin >> numstations;
  if ( (numstations <= 1) || (numstations > 300) )
  {
    cout << "stations between 2 and 300" << endl;
    exit(0);
  }
  cout << "arg2: mean idle time (msecs)" << endl;
  cin >> idletime;
  cout << "arg3: packet size (bytes)" << endl;
  cin >> packetsize;
  cout << "arg4: simulation duration (msecs)" << endl;
  cin >> duration;
  cout << "arg5: transient duration (msecs)" << endl;
  cin >> transient;
  cout << "arg6: number of blocks" << endl;
  cin >> numblocks;
}


int application::main()
{
  sim = new simulation();
  sim -> init("-scan yes");
   
  // statistics 
  wtt = new histogram(".wtt","-type weighted -form statistics -title Waiting_to_transmit");
  delay = new histogram(".delay","-form statistics -title Transmit_delay(msec)");
  efficiency = new histogram(".eff","-form statistics -title Protocol_efficiency(%%)");
  throughput = new histogram(".throughput","-form statistics -title Throughput(bps)");
  tk -> pack(wtt) -> pack(delay) -> pack(efficiency) -> pack(throughput);

  g1 = new generator(33,2929,10000);
  g2 = new generator(10,2109,20);

  net = new ethernet(packetsize,numblocks,numstations,idletime);

  // Initially all is peace and quiet on the LAN 
  for (int i = 0;i < numstations;i++)
  {
    station* stat = new station(i,IDLE);
    net -> add(stat,i);
    sim -> schedule(stat,net -> idle());
    stat = NULL;
  }

  // Run for transient period 
  cout << "Transient phase" << endl;
  sim -> run(transient);

  // Reset results 
  for (i=0;i < numstations;i++)
    (net -> which(i)) -> zero();
  net -> zerodelay();
  wtt -> reset();

  cout << "Steady-state phase" << endl;
  for (i = 0;i < numblocks;i++)
  {
    batch();
    cout << "BATCH " << i+1 << "..." << endl;;
    sim -> run(duration);
    log();
  }
  results();
  return 0;
}


// log results of each batch.

void application::log()
{
  double batch = (sim -> clock()) - lastclock;
  double slots = batch / (net -> packettrans());
  double del = (net -> delay()) - lastdelay;

  delay -> sample(del/(double)(net -> numbersuccess()));
  efficiency -> sample(100.0*(net -> numbersuccess())/slots);
  throughput -> sample(net -> numbersuccess()*8*(net->packetsz())*(net->factor())/batch);
}


// reset variables after each batch.

void application::batch()
{
  net -> numbersuccess(0L);
  lastclock = sim -> clock();
  lastdelay = net -> delay();
}


// show results,

void application::results()
{
  // Take last sample
  wtt -> sample(net -> waiting());

  // Get total simulated time excluding transient phase 
  double totaltime = sim -> clock() - transient;

  // Initialise 
  double numslots = totaltime/(net -> packettrans());
  long totalsuccessful = 0L;
  long total = 0L;
  long totalfailed = 0L;

  cout << setprecision(8);
  cout << endl << "*** PARAMETERS ***" << endl << endl;
  cout << "Number of stations       = " << numstations << endl;
  cout << "Inter transmission time  = " << idletime << endl;
  cout << "Frame size               = " << packetsize << endl;
  cout << "Time to transmit frame   = " << (net -> packettrans()) << endl;
  cout << "Number of tx intervals   = " << numslots << endl;
  cout << "Number of backoffs       = " << (net -> numberbackoffs()) << endl;

  cout << endl << "*** OVERALL RESULTS ***" << endl << endl;
  cout << "              Requests    Successfull Failed      " << endl;
  char* buf = new char[BUFSIZ];
  for (int i = 0;i < numstations;i++)
  {
    station* stat = (net -> which(i));
    total += (long)(stat -> totpackets());
    totalsuccessful += (long)(stat -> succ());
    totalfailed += (long)(stat -> fail());
    sprintf(buf,"station %d  -> %-11d %-11d %-11d \n",i,stat -> totpackets(),stat -> succ(),stat -> fail());
    cout << buf;
    stat = NULL;
  }
  sprintf(buf,"TOTALS     -> %-11ld %-11ld %-11ld \n\n",total,totalsuccessful,totalfailed);
  cout << buf;
  delete buf;
  cout << "Total delay              = " << (net -> delay()) << endl;
  cout << "Mean total delay         = " << (net -> delay())/((double)totalsuccessful) << " ms/frame" << endl;
  cout << "Mean normalised delay    = " << (net -> delay())/((double)totalsuccessful)/(net -> packettrans()) << endl;
  cout << "Efficiency (%%)          = " << (100.0*(double)totalsuccessful/(double)numslots) << endl;
  cout << "Successful (%%)          = " << (100.0*(double)totalsuccessful/(double)total) << endl;
  cout << "Throughput (bps)         = " << ((double)totalsuccessful*8.0*(net -> packetsz())*(net->factor())/totaltime) << endl;

  cout << (*wtt);
  cout << endl << "*** BATCH RESULTS ***" << endl << endl;
  cout << (*delay);
  cout << (*efficiency);
  cout << (*throughput);

  delete wtt;
  delete delay;
  delete efficiency;
  delete throughput;
  delete net;
  delete sim;
}

int main(int argc,char** argv)
{
  session* s = new application(argc,argv);
  s -> run();
  exit(0);
}

// testgenerator.c

#include <sim/sim.h>

// probabilities for the coupon collector test
 
static double pcc[16] = { 0.03840000, 0.07680000, 0.09984000, 0.10752000, 
                           0.10450944, 0.09547776, 0.08381645, 0.07163904,
                           0.06011299, 0.04979157, 0.04086200, 0.03331007, 
                           0.02702163, 0.02184196, 0.01760857, 0.07144851
                         };
 


class application : public session
{
public :
  application(int argc,char** argv);
  virtual int main();
  void uniformitytest();
  void uniformity2test();
  void uniformity3test();
  void gaptest(double lower,double upper);
  void pokertest();
  void couponcollectortest();
private :
  int number;
  generator* g;
};


application::application(int argc,char** argv) : session(argc,argv,"generatortest")
{
  int seed1,seed2,seed3;
  cout << "arg1 - seed 1 : " << endl;
  cin >> seed1;
  cout << "arg2 - seed 2 : " << endl;
  cin >> seed2;
  cout << "arg3 - seed 3 : " << endl;
  cin >> seed3;
  cout << "arg4 - number of random observations : " << endl;
  cin >> number;
  g = new generator(seed1,seed2,seed3);
}

// initialize and choose a test

int application::main()
{
  // create objects and perform a test
  simulation* sim = new simulation();
  int choice;
  cout << "arg :  choise between 0 and 7, where " << endl;
  cout << "   1 : the uniformity test" << endl;
  cout << "   2 : the uniformity test (dimension 2)" << endl;
  cout << "   3 : the uniformity test (dimension 3)" << endl;
  cout << "   4 : the gap test (interval (0,0.5))" << endl;
  cout << "   5 : the gap test (interval (0.5,1))" << endl;
  cout << "   6 : the poker test" << endl;
  cout << "   7 : the coupon collector test" << endl;
  cin >> choice;
  switch (choice)
   {
    case 1 :
      uniformitytest();
      break;
    case 2 :
      uniformity2test();
      break;
    case 3 :
      uniformity3test();
      break;
    case 4 :
      gaptest(0,0.5);
      break;
    case 5 :
      gaptest(0.5,1);
      break;
    case 6 :
      pokertest();
      break;
    case 7 :
      couponcollectortest();
      break;
    default :
      cerr << "wrong choice" << endl;
      exit(0);
  }
  delete sim;
  return 0;
}

 
// The uniforimity test on numbers in the range 0.0 -> 1.0.        
 
void application::uniformitytest()
{
  cout << "UNIFORMITY TEST..." << endl;
  histogram* h = new histogram(".h","-columnwidth 0.1 -columns 10 -form table -title uniformitytest");
  tk -> pack(h);
  analysis* a = new analysis(h);
  double* expected = new double[10];
 
  // add samples and fill expected
  for (int i=0;i<number;i++)
    h -> sample(g -> uniform(0,1));
  for (i = 0;i < 10;i++) 
    expected[i] = (double)number/10;
 
  // perform chisquare test
  cout << (*h);
  a -> chisquaretest(expected);
}
 

// The uniforimity (dimension 2) test on pairs in the range 0.0 -> 10.0
 
void application::uniformity2test()
{
  cout << "UNIFORMITY TEST (dimension 2)..." << endl;
  histogram* h = new histogram(".h","-columnwidth 1 -columns 100 -form table -title uniformitytest(dim.2)");
  tk -> pack(h);
  analysis* a = new analysis(h);
  double* expected = new double[100];
 
  // add samples and fill expected
  int i=0;
  while (i < number)
  {
    // cast to integers (0,..,9) and sample pairs
    int x = 10*(int)(10 * g -> uniform(0,1)) + (int)(10 * g -> uniform(0,1));
    h -> sample(x);
    i += 2;
  }
  for (i = 0;i < 100;i++) 
    expected[i] = (double)number/2/100;
 
  // perform chisquare test
  cout << (*h);
  a -> chisquaretest(expected);
}
 

// The uniforimity (dimension 3) test on triples in the range 0.0 -> 10.0
 
void application::uniformity3test()
{
  cout << "UNIFORMITY TEST (dimension 3)..." << endl;
  histogram* h = new histogram(".h","-columnwidth 1 -columns 125 -form table -title uniformitytest(dim.3)");
  tk -> pack(h);
  analysis* a = new analysis(h);
  double* expected = new double[125];
 
  // add samples and fill expected
  int i=0;
  while (i < number)
  {
    // cast to integers (0,..,4) and sample triples
    int x = 25*(int)(5 * g -> uniform(0,1)) + 5*(int)(5 * g -> uniform(0,1)) + (int)(5 * g -> uniform(0,1));
    h -> sample(x);
    i += 3;
  }
  for (i = 0;i < 125;i++)
    expected[i] = (double)number/3/125;
 
  // perform chisquare test
  cout << (*h);
  a -> chisquaretest(expected);
}


// The gap test on a gap from lower to upper        
 
void application::gaptest(double lower,double upper)
{
  cout << "GAP TEST: " << lower << " TO " << upper << " ..." << endl;
  histogram* h = new histogram(".h","-form table -title gaptest");
  tk -> pack(h);
  analysis* a = new analysis(h);
  double* expected = new double[10];
 
  // sample the gaps
  int gs = 0;
  int i;
  for (i = 0;i < number;i++) 
  {
    double x = g -> uniform(0,1);
    if ((x < lower) || (upper <= x)) 
      gs++;
    else 
    {
      // found a gap, make sure it is not too big
      if (gs >= 10) 
        gs = 10 - 1;
      h -> sample(gs);
      gs = 0;
    }
  }
 
  // create and fill expected
  double p = upper - lower;
  for (i = 0;i < 10;i++) 
    expected[i] = p * p * pow(1-p,i) * number;
 
  // results
  cout << (*h);
  a -> chisquaretest(expected);
}
 
 
// The poker test. Categories denote the number of different    
// integers in the 5 selected. The results for 1 different and 2 
// different are combined.        
 
void application::pokertest()
{
  cout << "POKER TEST..." << endl;
  histogram* h = new histogram(".h","-start 1 -columns 4 -form table -title pokertest");
  tk -> pack(h);
  analysis* a = new analysis(h);
  double* expected = new double[4];
 
  // sample poker values
  int i,old = 0;int j = 0;
  for (i = 0,j = 0;i < number;i++,j++) 
  {
    int u[5];
    u[j] = (int)(g -> uniform(0,1) * 10.0);
    if (j == 4) 
    {
      // reached a group of five
      int n = 5;
      for (int k = 4;k > 0;k--)
      {
        for (int l = k-1;l >= 0;l--)
          if (u[k] == u[l])
          {
            // count the number of different integers (=n)
            n--;
            u[l] = k*100 + l*10;
          }
      }
      h -> sample(n-1);
      j = -1;
      old++;
      i = old-1;
    }
  }
 
  // fill expected and perform the chi square test
  cout << (*h);
  expected[0] = 0.0136*number;
  expected[1] = 0.1800*number;
  expected[2] = 0.5040*number;
  expected[3] = 0.3024*number;
  a -> chisquaretest(expected);
}
 
 
// The Coupon Collectors Test, based on a run of 5 and
// categories of 5, 6, ... 20 or more.
 
void application::couponcollectortest()
{
  cout << "COUPON COLLECTORS TEST" << endl;
  histogram* h = new histogram(".h","-columns 16 -form table -title couponcollectortest");
  tk -> pack(h);
  analysis* a = new analysis(h);
  double* expected = new double[16];
 
  // sample the values
  int u[5],full,old;
  int i;
  for (i = 0;i < 5;i++)
    u[i] = 0;
  full = 1;
  old = 0;
  int j = 0;
  for (i = 0;i < number;i++,j++)
  {
    u[(int)(g -> uniform(0,1) * 5)]++;
    if (j > 3)
    {
      for (int k = 0;full && (k < 5);k++)
        if (!u[k])
          full = FALSE;
      if (full)
      {
        if (j > 19)
          j = 19;
        // sample  the category
        h -> sample(j-4);
        for (j = 0;j < 5;j++)
          u[j] = 0;
        j = -1;
        old++;
        i = old-1;
      }
      full = 1;
    }
  }
 
  // fill expected and perform the chi square test
  cout << (*h);
  for (i = 0;i < 16;i++)
    expected[i] = number * pcc[i];
  a -> chisquaretest(expected);
}
 
 
// create and start the session

int main(int argc,char** argv)
{
  session* s = new application(argc,argv);
  s -> run();
  exit(0);
}

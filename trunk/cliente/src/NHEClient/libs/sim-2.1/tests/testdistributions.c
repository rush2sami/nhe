// testdistributions.c

#include <sim/sim.h>


class application : public session
{
public :
  application(int argc,char** argv);
  virtual int main();
  void uniform();
  void exponential();
  void hyperexponential();
  void gamma();
  void erlang();
  void binomial();
  void geometric();
  void hypergeometric();
  void negativebinomial();
  void poisson();
  void normal();
  void lognormal();
  void beta();
  void laplace();
  void fdistribution();
  void chisquare();
  void student();
  void triangular();
  void weibull();

private :
  int number;
  generator* g;
  histogram* h;
  analysis* a;

};


application::application(int argc,char** argv) : session(argc,argv,"distributionstest")
{
  int seed1,seed2,seed3;

  cout << "arg1 : seed 1" << endl;
  cin >> seed1;
  cout << "arg2 : seed 2" << endl;
  cin >> seed2;
  cout << "arg3 : seed 3" << endl;
  cin >> seed3;
  g = new generator(seed1,seed2,seed3);
}


// initialize and choose a test

int application::main()
{
  simulation* sim = new simulation();
  int columns;
  double start,width;
  cout << "arg4 : start value of the histogram" << endl;
  cin >> start;
  cout << "arg5 : column width of the histogram" << endl;
  cin >> width;
  cout << "arg6 : number of columns" << endl;
  cin >> columns;
  char options[BUFSIZ];
  sprintf(options,"-start %f -columnwidth %f -columns %d",start,width,columns);
  h = new histogram(".histogram",options);
  a = new analysis(h);
  tk -> pack(h);

  cout << "arg7 : number of random observations" << endl;
  cin >> number;
  cout << "arg8 :  choice between 1 and 19, where " << endl;
  cout << "   1 : the uniform distribution" << endl;
  cout << "   2 : the exponential distribution" << endl;
  cout << "   3 : the hyperexponential distribution" << endl;
  cout << "   4 : the gamma distribution" << endl;
  cout << "   5 : the erlang distribution" << endl;
  cout << "   6 : the binomial distribution" << endl;
  cout << "   7 : the geometric distribution" << endl;
  cout << "   8 : the hypergeometric distribution" << endl;
  cout << "   9 : the negative binomial distribution" << endl;
  cout << "   10 : the poisson distribution" << endl;
  cout << "   11 : the normal distribution" << endl;
  cout << "   12 : the lognormal distribution" << endl;
  cout << "   13 : the beta distribution" << endl;
  cout << "   14 : the laplace distribution" << endl;
  cout << "   15 : the F distribution" << endl;
  cout << "   16 : the chisquare distribution" << endl;
  cout << "   17 : the student's t distribution" << endl;
  cout << "   18 : the triangular distribution" << endl;
  cout << "   19 : the weibull distribution" << endl;
  int choice;
  cin >> choice;
  switch (choice)
  {
    case 1 :
      uniform();
      break;
    case 2 :
      exponential();
      break;
    case 3 :
      hyperexponential();
      break;
    case 4 :
      gamma();
      break;
    case 5 :
      erlang();
      break;
    case 6 :
      binomial();
      break;
    case 7 :
      geometric();
      break;
    case 8 :
      hypergeometric();
      break;
    case 9 :
      negativebinomial();
      break;
    case 10 :
      poisson();
      break;
    case 11 :
      normal();
      break;
    case 12 :
      lognormal();
      break;
    case 13 :
      beta();
      break;
    case 14 :
      laplace();
      break;
    case 15 :
      fdistribution();
      break;
    case 16 :
      chisquare();
      break;
    case 17 :
      student();
      break;
    case 18 :
      triangular();
      break;
    case 19 :
      weibull();
      break;
    default :
      cerr << "wrong choice" << endl;
      exit(0);
  }
  delete sim;
  return 0;
}


// perform test on the uniform distribution
 
void application::uniform()
{
  cout << "uniform" << endl;
  double a1,a2;
  cout << "arg 1 : " << endl;
  cin >> a1;
  cout << "arg 2 : " << endl;
  cin >> a2;
 
  for (int i=0;i<number;i++)
    h -> sample(g -> uniform(a1,a2));
  cout << (*h) << endl;
  a -> uniform(a1,a2);
}
 
 
// perform test on the exponential distribution
 
void application::exponential()
{
  double a1;
  cout << "exponential" << endl;
  cout << "arg 1 : " << endl;
  cin >> a1;
 
  for (int i=0;i<number;i++)
    h -> sample(g -> exponential(a1));
  cout << (*h) << endl;
  a -> exponential(a1);
}
 
 
// perform test on the hyperexponential distribution
 
void application::hyperexponential()
{
  double a1,a2,a3;
  cout << "hyperexponential" << endl;
  cout << "arg 1 : " << endl;
  cin >> a1;
  cout << "arg 2 : " << endl;
  cin >> a2;
  cout << "arg 3 : " << endl;
  cin >> a3;
 
  for (int i=0;i<number;i++)
    h -> sample(g -> hyperexponential(a1,a2,a3));
  cout << (*h) << endl;
  a -> hyperexponential(a1,a2,a3);
}
 
 
// perform test on the gamma distribution
 
void application::gamma()
{
  double a1,a2;
  cout << "gamma" << endl;
  cout << "arg 1 : " << endl;
  cin >> a1;
  cout << "arg 2 : " << endl;
  cin >> a2;
 
  for (int i=0;i<number;i++)
    h -> sample(g -> gamma(a1,a2));
  cout << (*h) << endl;
  a -> gamma(a1,a2);
}
 
 
// perform test on the erlang distribution
 
void application::erlang()
{
  int a1;
  double a2;
  cout << "erlang" << endl;
  cout << "arg 1 : " << endl;
  cin >> a1;
  cout << "arg 2 : " << endl;
  cin >> a2;
 
  for (int i=0;i<number;i++)
    h -> sample(g -> erlang(a1,a2));
  cout << (*h) << endl;
  a -> erlang(a1,a2);
}
 
 
// perform test on the binomial distribution
 
void application::binomial()
{
  int a2;
  double a1;
  cout << "binomial" << endl;
  cout << "arg 1 : " << endl;
  cin >> a1;
  cout << "arg 2 : " << endl;
  cin >> a2;
 
  for (int i=0;i<number;i++)
    h -> sample(g -> binomial(a1,a2));
  cout << (*h) << endl;
  a -> binomial(a1,a2);
}
 
 
// perform test on the geometric distribution
 
void application::geometric()
{
  double a1;
  cout << "geometric" << endl;
  cout << "arg 1 : " << endl;
  cin >> a1;
 
  for (int i=0;i<number;i++)
    h -> sample(g -> geometric(a1));
  cout << (*h) << endl;
  a -> geometric(a1);
}
 
 
// perform test on the hypergeometric distribution
 
void application::hypergeometric()
{
  int a1,a2;
  double a3;
  cout << "hypergeometric" << endl;
  cout << "arg 1 : " << endl;
  cin >> a1;
  cout << "arg 2 : " << endl;
  cin >> a2;
  cout << "arg 3 : " << endl;
  cin >> a3;
 
  for (int i=0;i<number;i++)
    h -> sample(g -> hypergeometric(a1,a2,a3));
  cout << (*h) << endl;
  a -> hypergeometric(a1,a2,a3);
}
 
 
// perform test on the negative binomial distribution
 
void application::negativebinomial()
{
  int a2;
  double a1;
  cout << "negative binomial" << endl;
  cout << "arg 1 : " << endl;
  cin >> a1;
  cout << "arg 2 : " << endl;
  cin >> a2;
 
  for (int i=0;i<number;i++)
    h -> sample(g -> negativebinomial(a1,a2));
  cout << (*h) << endl;
  a -> negativebinomial(a1,a2);
}
 
 
// perform test on the poisson distribution
 
void application::poisson()
{
  double a1,a2;
  cout << "poisson" << endl;
  cout << "arg 1 : " << endl;
  cin >> a1;
  cout << "arg 2 : " << endl;
  cin >> a2;
 
  for (int i=0;i<number;i++)
    h -> sample(g -> poisson(a1));
  cout << (*h) << endl;
  a -> poisson(a1);
}
 
 
// perform test on the normal distribution
 
void application::normal()
{
  double a1,a2;
  cout << "normal" << endl;
  cout << "arg 1 : " << endl;
  cin >> a1;
  cout << "arg 2 : " << endl;
  cin >> a2;
 
  for (int i=0;i<number;i++)
    h -> sample(g -> normal(a1,a2));
  cout << (*h) << endl;
  a -> normal(a1,a2);
}
 
 
// perform test on the lognormal distribution
 
void application::lognormal()
{
  double a1,a2;
  cout << "lognormal" << endl;
  cout << "arg 1 : " << endl;
  cin >> a1;
  cout << "arg 2 : " << endl;
  cin >> a2;

  for (int i=0;i<number;i++)
    h -> sample(g -> lognormal(a1,a2));
  cout << (*h) << endl;
  a -> lognormal(a1,a2);
}
 
 
// perform test on the beta distribution
 
void application::beta()
{
  double a1,a2;
  cout << "beta" << endl;
  cout << "arg 1 : " << endl;
  cin >> a1;
  cout << "arg 2 : " << endl;
  cin >> a2;
 
  for (int i=0;i<number;i++)
    h -> sample(g -> beta(a1,a2));
  cout << (*h) << endl;
  a -> beta(a1,a2);
}
 
 
// perform test on the laplace distribution
 
void application::laplace()
{
  double a1;
  cout << "laplace" << endl;
  cout << "arg 1 : " << endl;
  cin >> a1;
 
  for (int i=0;i<number;i++)
    h -> sample(g -> laplace(a1));
  cout << (*h) << endl;
  a -> laplace(a1);
}
 
 
// perform test on the F distribution
 
void application::fdistribution()
{
  int a1,a2;
  cout << "F distribution" << endl;
  cout << "arg 1 : " << endl;
  cin >> a1;
  cout << "arg 2 : " << endl;
  cin >> a2;
 
  for (int i=0;i<number;i++)
    h -> sample(g -> fdistribution(a1,a2));
  cout << (*h) << endl;
  a -> fdistribution(a1,a2);
}
 
 
// perform test on the chisquare distribution
 
void application::chisquare()
{
  int a1;
  cout << "chi-square" << endl;
  cout << "arg 1 : " << endl;
  cin >> a1;
 
  for (int i=0;i<number;i++)
    h -> sample(g -> chisquare(a1));
  cout << (*h) << endl;
  a -> chisquare(a1);
}
 
 
// perform test on the student distribution
 
void application::student()
{
  int a1;
  cout << "student" << endl;
  cout << "arg 1 : " << endl;
  cin >> a1;
 
  for (int i=0;i<number;i++)
    h -> sample(g -> student(a1));
  cout << (*h) << endl;
  a -> student(a1);
}
 
 
// perform test on the triangular distribution
 
void application::triangular()
{
  double a1;
  cout << "triangular" << endl;
  cout << "arg 1 : " << endl;
  cin >> a1;
 
  for (int i=0;i<number;i++)
    h -> sample(g -> triangular(a1));
  cout << (*h) << endl;
  a -> triangular(a1);
}
 
 
// perform test on the weibull distribution
 
void application::weibull()
{
  double a1,a2;
  cout << "weibull" << endl;
  cout << "arg 1 : " << endl;
  cin >> a1;
  cout << "arg 2 : " << endl;
  cin >> a2;
 
  for (int i=0;i<number;i++)
    h -> sample(g -> weibull(a1,a2));
  cout << (*h) << endl;
  a -> weibull(a1,a2);
}


// create and start the session

int main(int argc,char** argv)
{
  session* s = new application(argc,argv);
  s -> run();
  exit(0);
}

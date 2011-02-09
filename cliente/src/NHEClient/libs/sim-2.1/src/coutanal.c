////////////////////////////////////////////////////////////////////////
//                                                                    //
// coutanalysis.c                                                     //
//                                                                    //
// this class takes care of outputting the results of an analysis     //
// object.							      //
//                                                                    //
// (c) 1995 D. Bolier, A. Eliens                                      //
//                                                                    //
////////////////////////////////////////////////////////////////////////


#include <string.h>
#include <stdio.h>
#include <math.h>
#include <iostream.h>
#include <sim/defs.h>
#include <sim/coutanalysis.h>
#include <sim/error.h>
#include <sim/histogram.h>
#include <sim/analysis.h>
#include <sim/simulation.h>

#ifdef HUSH

#include <sim/hush.h>

static int numberprints = 0;

enum {SCREENWIDTH = 400,TEXTHEIGHT = 20};
enum {BORDERWIDTH = 50,BARWIDTH = 40,BORDERHEIGHT = 50,BARHEIGHT = 260};

extern kit* thekit();

static double lngamma(double xx);
static double cof[6]   = {76.18009173, -86.50532033, 24.01409822, -1.231739516,0.120858003e-2, -0.536382e-5};

#endif


// constructor, taking the histogram and analysis object

coutanalysis::coutanalysis(histogram* hi,analysis* a)
{
  h = hi;
  an = a;
  output = UNDEFINED;
  lev = UNDEFINED;
  conf = UNDEFINED;
  degree = UNDEFINED;
  chisq = UNDEFINED;
  pv = UNDEFINED;
  estimated = UNDEFINED;
  av = UNDEFINED;
  bv = UNDEFINED;
  cv = UNDEFINED;
  qv = UNDEFINED;
  mv = UNDEFINED;
  nv = UNDEFINED;
  tp = NULL;
  tt1 = NULL;
  tt2 = NULL;
#ifdef HUSH
  s = NULL;
#endif
}


coutanalysis::~coutanalysis()
{
  h = NULL;
  an = NULL;
  if (tp)
    delete tp;
#ifdef HUSH
  if (s)
    delete s;
#endif
}


// sets output structure for the confidence interval of the mean

void coutanalysis::confidence(int l,double cnf,double a)
{
  output = CONFIDENCE;
  lev = l;
  conf = cnf;
  av = a;
}


// sets output structure for the confidence interval of the difference
// of means

void coutanalysis::difference(int l,double cnf,double est)
{
  output = DIFFERENCE;
  lev = l;
  conf = cnf;
  estimated = est;
}


// sets ouput structure for the covariance

void coutanalysis::covariance(char* fn1,char* fn2,double cov)
{
  output = COVARIANCE;
  cv = cov;
  tt1 = fn1;
  tt2 = fn2;
}


// sets ouput structure for the correlation

void coutanalysis::correlation(char* fn1,char* fn2,double cor)
{
  output = CORRELATION;
  cv = cor;
  tt1 = fn1;
  tt2 = fn2;
}


// sets output structure for the chi square test

void coutanalysis::chisquaretest(int dof,double ch,double prob)
{
  output = CHISQUARETEST;
  pv = prob;
  chisq = ch;
  degree = dof;
}


// sets output structure for the fitting of a linear curve

void coutanalysis::linear(double a,double b,int dof,double ch,double prob)
{
  output = LINEAR;
  av = a;
  bv = b;
  pv = prob;
  chisq = ch;
  degree = dof;
}


// sets output structure for the fitting of a quadratic curve

void coutanalysis::quadratic(double a,double b,double c,int dof,double ch,double prob)
{
  output = QUADRATIC;
  av = a;
  bv = b;
  cv = c;
  pv = prob;
  chisq = ch;
  degree = dof;
}


// sets output structure for the fitting of the uniform probability
// distribution

void coutanalysis::uniform(double a,double b,int dof,double ch,double prob)
{
  output = UNIFORM;
  av = a;
  bv = b;
  pv = prob;
  chisq = ch;
  degree = dof;
}


// sets output structure for the fitting of the exponential probability
// distribution

void coutanalysis::exponential(double a,int dof,double ch,double prob)
{
  output = EXPONENTIAL;
  av = a;
  pv = prob;
  chisq = ch;
  degree = dof;
}


// sets output structure for the fitting of the hyperexponential 
// probability distribution

void coutanalysis::hyperexponential(double p,double a,double b,int dof,double ch,double prob)
{
  output = HYPEREXPONENTIAL;
  qv = p;
  av = a;
  bv = b;
  pv = prob;
  chisq = ch;
  degree = dof;
}


// sets output structure for the fitting of the gamma probability
// distribution

void coutanalysis::gamma(double a,double b,int dof,double ch,double prob)
{
  output = GAMMA;
  av = a;
  bv = b;
  pv = prob;
  chisq = ch;
  degree = dof;
}


// sets output structure for the fitting of the beta probability
// distribution

void coutanalysis::beta(double a,double b,int dof,double ch,double prob)
{
  output = BETA;
  av = a;
  bv = b;
  pv = prob;
  chisq = ch;
  degree = dof;
}


// sets output structure for the fitting of the erlang probability
// distribution

void coutanalysis::erlang(double a,int n,int dof,double ch,double prob)
{
  output = ERLANG;
  av = a;
  nv = n;
  pv = prob;
  chisq = ch;
  degree = dof;
}


// sets output structure for the fitting of the binomial probability
// distribution

void coutanalysis::binomial(int n,double p,int dof,double ch,double prob)
{
  output = BINOMIAL;
  nv = n;
  qv = p;
  pv = prob;
  chisq = ch;
  degree = dof;
}


// sets output structure for the fitting of the geometric probability
// distribution

void coutanalysis::geometric(double p,int dof,double ch,double prob)
{
  output = GEOMETRIC;
  qv = p;
  pv = prob;
  chisq = ch;
  degree = dof;
}


// sets output structure for the fitting of the hypergeometric 
// probability distribution

void coutanalysis::hypergeometric(int m,int n,double p,int dof,double ch,double prob)
{
  output = HYPERGEOMETRIC;
  mv = m;
  nv = n;
  qv = p;
  pv = prob;
  chisq = ch;
  degree = dof;
}


// sets output structure for the fitting of the negative binomial 
// probability distribution

void coutanalysis::negativebinomial(double p,int n,int dof,double ch,double prob)
{
  output = NEGBINOMIAL;
  nv = n;
  qv = p;
  pv = prob;
  chisq = ch;
  degree = dof;
}


// sets output structure for the fitting of the poisson probability
// distribution

void coutanalysis::poisson(double a,int dof,double ch,double prob)
{
  output = POISSON;
  av = a;
  pv = prob;
  chisq = ch;
  degree = dof;
}


// sets output structure for the fitting of the normal probability
// distribution

void coutanalysis::normal(double a,double b,int dof,double ch,double prob)
{
  output = NORMAL;
  av = a;
  bv = b;
  pv = prob;
  chisq = ch;
  degree = dof;
}


// sets output structure for the fitting of the lognormal probability
// distribution

void coutanalysis::lognormal(double a,double b,int dof,double ch,double prob)
{
  output = LOGNORMAL;
  av = a;
  bv = b;
  pv = prob;
  chisq = ch;
  degree = dof;
}


// sets output structure for the fitting of the laplace probability
// distribution

void coutanalysis::laplace(double a,int dof,double ch,double prob)
{
  output = LAPLACE;
  av = a;
  pv = prob;
  chisq = ch;
  degree = dof;
}


// sets output structure for the fitting of the chisquare probability
// distribution

void coutanalysis::chisquare(int n,int dof,double ch,double prob)
{
  output = CHISQUARE;
  nv = n;
  pv = prob;
  chisq = ch;
  degree = dof;
}


// sets output structure for the fitting of the student probability
// distribution

void coutanalysis::student(int n,int dof,double ch,double prob)
{
  output = STUDENT;
  nv = n;
  pv = prob;
  chisq = ch;
  degree = dof;
}


// sets output structure for the fitting of the triangular probability
// distribution

void coutanalysis::triangular(double a,int dof,double ch,double prob)
{
  output = TRIANGULAR;
  av = a;
  pv = prob;
  chisq = ch;
  degree = dof;
}


// sets output structure for the fitting of the f probability
// distribution

void coutanalysis::fdistribution(int n,int m,int dof,double ch,double prob)
{
  output = FDISTRIBUTION;
  nv = n;
  mv = m;
  pv = prob;
  chisq = ch;
  degree = dof;
}


// sets output structure for the fitting of the weibull probability
// distribution

void coutanalysis::weibull(double a,double b,int dof,double ch,double prob)
{
  output = WEIBULL;
  av = a;
  bv = b;
  pv = prob;
  chisq = ch;
  degree = dof;
}


// sets output structure for the size of a queue

void coutanalysis::queuevalues(char* t,int kd,double lb,double mu,double vl,double mn,double cf,int c)
{
  output = kd;
  tp = t;
  av = lb;
  bv = mu;
  nv = c;
  cv = vl;
  pv = mn;
  conf = cf;
}


#ifdef HUSH

// generates postscript from the screen

void coutanalysis::postscript(char* fn)
{
  if (s)
    s -> postscript(fn);
}


// formats the screen

screen* coutanalysis::format(char* p,char* options)
{
  s = new screen(p,options);
  return s;
}

screen* coutanalysis::format(widget* w,char* p,char* options)
{
  s = new screen(w,p,options);
  return s;
}

#endif


// fill the ostream and/or canvas at the hand of the output type and 
// the compiler directive

ostream& operator<<(ostream& os,coutanalysis& ca)
{
#ifdef HUSH
  kit* tk = thekit();

  // if the screen allready exists clear it
  if (ca.s)
  {
    ca.s -> del(ca.s -> path());
    ca.h -> del("plot");  // and the plot
    tk -> update();
  }
  else
  {
    // for the geometry
    int x;
    if (ca.output <= CHISQUARETEST)
      x = TEXTHEIGHT*4;
    else if (ca.output <= CORRELATION)
      x = TEXTHEIGHT*2;
    else
      x = TEXTHEIGHT*5;
    numberprints++;
    char* buf = new char[BUFSIZ];
    sprintf(buf,".analysis-%d",numberprints);
    toplevel* t = new toplevel(buf);
    ca.s = new screen(t,".screen");
    ca.s -> geometry(SCREENWIDTH,x);
    tk -> pack(ca.s);
//    delete buf;  // NO      <----------
    delete[] buf;  // YES     <----------
  }
#endif
  switch (ca.output)
  {
    case UNDEFINED :
      return os;
    case CONFIDENCE :
#ifdef HUSH
      ca.confidence();
#else
      confidence(os,ca);
#endif
      break;
    case DIFFERENCE :
#ifdef HUSH
      ca.difference();
#else
      difference(os,ca);
#endif
      break;
    case COVARIANCE :
#ifdef HUSH
      ca.covariance();
#else
      covariance(os,ca);
#endif
      break;
    case CORRELATION :
#ifdef HUSH
      ca.correlation();
#else
      correlation(os,ca);
#endif
      break;
    case CHISQUARETEST :
#ifdef HUSH
      ca.chisquaretest();
#else
      chisquaretest(os,ca);
#endif
      break;
    case LINEAR :
#ifdef HUSH
      ca.linear();
#else
      linear(os,ca);
#endif
      break;
    case QUADRATIC :
#ifdef HUSH
      ca.quadratic();
#else
      quadratic(os,ca);
#endif
      break;
    case UNIFORM :
#ifdef HUSH
      ca.uniform();
#else
      uniform(os,ca);
#endif
      break;
    case EXPONENTIAL :
#ifdef HUSH
      ca.exponential();
#else
      exponential(os,ca);
#endif
      break; 
    case HYPEREXPONENTIAL :
#ifdef HUSH
      ca.hyperexponential();
#else
      hyperexponential(os,ca);
#endif
      break; 
    case GAMMA : 
#ifdef HUSH
      ca.gamma();
#else
      gamma(os,ca); 
#endif
      break; 
    case ERLANG :
#ifdef HUSH
      ca.erlang();
#else
      erlang(os,ca);
#endif
      break;
    case BINOMIAL :
#ifdef HUSH
      ca.binomial();
#else
      binomial(os,ca);
#endif
      break;
    case GEOMETRIC :
#ifdef HUSH
      ca.geometric();
#else
      geometric(os,ca);
#endif
      break;
    case HYPERGEOMETRIC :
#ifdef HUSH
      ca.hypergeometric();
#else
      hypergeometric(os,ca);
#endif
      break;
    case NEGBINOMIAL :
#ifdef HUSH
      ca.negativebinomial();
#else
      negativebinomial(os,ca);
#endif
      break;
    case POISSON :
#ifdef HUSH
      ca.poisson();
#else
      poisson(os,ca);
#endif
      break;
    case NORMAL :
#ifdef HUSH
      ca.normal();
#else
      normal(os,ca);
#endif
      break;
    case LOGNORMAL :
#ifdef HUSH
      ca.lognormal();
#else
      lognormal(os,ca);
#endif
      break;
    case LAPLACE :
#ifdef HUSH
      ca.laplace();
#else
      laplace(os,ca);
#endif
      break;
    case CHISQUARE :
#ifdef HUSH
      ca.chisquare();
#else
      chisquare(os,ca);
#endif
      break;
    case STUDENT :
#ifdef HUSH
      ca.student();
#else
      student(os,ca);
#endif
      break;
    case TRIANGULAR :
#ifdef HUSH
      ca.triangular();
#else
      triangular(os,ca);
#endif
      break;
    case BETA :
#ifdef HUSH
      ca.beta();
#else
      beta(os,ca);
#endif
      break;
    case FDISTRIBUTION :
#ifdef HUSH
      ca.fdistribution();
#else
      fdistribution(os,ca);
#endif
      break;
    case WEIBULL :
#ifdef HUSH
      ca.weibull();
#else
      weibull(os,ca);
#endif
      break;
    default :
#ifdef HUSH
      ca.queuevalues();
#else
      queuevalues(os,ca);
#endif
      break;
  }
#ifdef HUSH
  ca.s -> line(0,TEXTHEIGHT,SCREENWIDTH,TEXTHEIGHT,"-fill black");
  tk -> update();
#endif
  return os;
}


#ifdef HUSH

// fill the screen with output of the specified analysis

void coutanalysis::confidence()
{
  char* buf = new char[BUFSIZ];
  sprintf(buf,"CONFIDENCE : %s",h -> histogramtitle());
  int id = s -> text(SCREENWIDTH/2,TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
  if (lev == 0)
    sprintf(buf,"level : 90");
  else if (lev == 1)
    sprintf(buf,"level : 95");
  else
    sprintf(buf,"level : 99");
  id = s -> text(SCREENWIDTH/2,3*TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
  sprintf(buf,"estimated mean : %f",av);
  id = s -> text(SCREENWIDTH/2,5*TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
  sprintf(buf,"interval : +/-%f",conf);
  id = s -> text(SCREENWIDTH/2,7*TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
//  delete buf;  // NO      <----------
  delete[] buf;  // YES     <----------
}


void coutanalysis::difference()
{
  char* buf = new char[BUFSIZ];
  sprintf(buf,"CONFIDENCE : %s",h -> histogramtitle());
  int id = s -> text(SCREENWIDTH/2,TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
  if (lev == 0)
    sprintf(buf,"level : 90");
  else if (lev == 1)
    sprintf(buf,"level : 95");
  else
    sprintf(buf,"level : 99");
  id = s -> text(SCREENWIDTH/2,3*TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
  sprintf(buf,"estimated difference : %f",estimated);
  id = s -> text(SCREENWIDTH/2,5*TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
  sprintf(buf,"interval : +/-%f",conf);
  id = s -> text(SCREENWIDTH/2,7*TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
//  delete buf;  // NO      <----------
  delete[] buf;  // YES     <----------
}


void coutanalysis::covariance()
{
  char* buf = new char[BUFSIZ];
  sprintf(buf,"COVARIANCE %s and %s",tt1,tt2);
  int id = s -> text(SCREENWIDTH/2,TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
  sprintf(buf,"cov(X,Y) : %f",cv);
  id = s -> text(SCREENWIDTH/2,3*TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
//  delete buf;  // NO      <----------
  delete[] buf;  // YES     <----------
}


void coutanalysis::correlation()
{
  char* buf = new char[BUFSIZ];
  sprintf(buf,"CORRELATION COEFFICIENT %s and %s",tt1,tt2);
  int id = s -> text(SCREENWIDTH/2,TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
  sprintf(buf,"p(X,Y) : %f",cv);
  id = s -> text(SCREENWIDTH/2,3*TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
//  delete buf;  // NO      <----------
  delete[] buf;  // YES     <----------
}


void coutanalysis::test()
{
  char* buf = new char[BUFSIZ];
  sprintf(buf,"chi-square : %.3f",chisq);
  int id = s -> text(SCREENWIDTH/2,5*TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
  sprintf(buf,"degree of freedom : %d",degree);
  id = s -> text(SCREENWIDTH/2,7*TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
  sprintf(buf,"P(chi-square > %.3f) = %.3f",chisq,pv);
  id = s -> text(SCREENWIDTH/2,9*TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
//  delete buf;  // NO      <----------
  delete[] buf;  // YES     <----------
}

  
void coutanalysis::chisquaretest()
{
  char* buf = new char[BUFSIZ];
  sprintf(buf,"CHI SQUARE : %s",h -> histogramtitle());
  int id = s -> text(SCREENWIDTH/2,TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
  sprintf(buf,"chi-square : %.3f",chisq);
  id = s -> text(SCREENWIDTH/2,3*TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
  sprintf(buf,"degree of freedom : %d",degree);
  id = s -> text(SCREENWIDTH/2,5*TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
  sprintf(buf,"P(chi-square > %.3f) = %.3f",chisq,pv);
  id = s -> text(SCREENWIDTH/2,7*TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
//  delete buf;  // NO      <----------
  delete[] buf;  // YES     <----------
}

  
void coutanalysis::linear()
{
  char* buf = new char[BUFSIZ];
  sprintf(buf,"LINEAR FIT : %s",h -> histogramtitle());
  int id = s -> text(SCREENWIDTH/2,TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
  sprintf(buf,"curve : %.2fx + %.2f",av,bv);
  id = s -> text(SCREENWIDTH/2,3*TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
  test();
//  delete buf;  // NO      <----------
  delete[] buf;  // YES     <----------
  if (h -> histogramform() == GRAPH)
  {
    int k = 0;
    int nc = h -> columns();
    double wd = h -> widthcolumn();
    double i = h -> startvalue();
    double st = h -> startvalue();
    double* curve = new double[BUFSIZ];
    while (i < st+nc*wd)
    {
      curve[k] = av*(st+i) + bv;
      i += wd/10;
      k++;
    }
    plotcontinu(curve);
//    delete curve;  // NO      <----------
    delete[] curve;  // YES     <----------
  }
}


void coutanalysis::quadratic()
{
  char* buf = new char[BUFSIZ];
  sprintf(buf,"QUADRATIC FIT : %s",h -> histogramtitle());
  int id = s -> text(SCREENWIDTH/2,TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
  sprintf(buf,"curve : %.2fx*x + %.2fx + %.2f",av,bv,cv);
  id = s -> text(SCREENWIDTH/2,3*TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
  test();
//  delete buf;  // NO      <----------
  delete[] buf;  // YES     <----------
  if (h -> histogramform() == GRAPH)
  {
    int k = 0;
    int nc = h -> columns();
    double wd = h -> widthcolumn();
    double i = h -> startvalue();
    double st = h -> startvalue();
    double* curve = new double[BUFSIZ];
    while (i < st+nc*wd)
    {
      curve[k] = av*(st+i)*(st+i) + bv*(st+i) + cv;
      i += wd/10;
      k++;
    }
    plotcontinu(curve);
//    delete curve;  // NO      <----------
    delete[] curve;  // YES     <----------
  }
}


void coutanalysis::uniform()
{
  char* buf = new char[BUFSIZ];
  sprintf(buf,"UNIFORM FIT : %s",h -> histogramtitle());
  int id = s -> text(SCREENWIDTH/2,TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
  sprintf(buf,"a : %.2f,b : %.2f",av,bv);
  id = s -> text(SCREENWIDTH/2,3*TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
  test();
//  delete buf;  // NO      <----------
  delete[] buf;  // YES     <----------
  if (h -> histogramform() == GRAPH)
  {
    int k = 0;
    double sumf = h -> sumfrequencies();
    int nc = h -> columns();
    double wd = h -> widthcolumn();
    double i = h -> startvalue();
    double st = h -> startvalue();
    double* curve = new double[BUFSIZ];
    while (i < st+nc*wd)
    {
      if ( (i >= av) && (i <= bv) )
        curve[k] = sumf / (bv-av) * wd;
      else
        curve[k] = 0;
      i += wd/10;
      k++;
    }
    plotcontinu(curve);
//    delete curve;  // NO      <----------
    delete[] curve;  // YES     <----------
  }
}


void coutanalysis::exponential()
{
  char* buf = new char[BUFSIZ];
  sprintf(buf,"EXPONENTIAL FIT : %s",h -> histogramtitle());
  int id = s -> text(SCREENWIDTH/2,TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
  sprintf(buf,"a : %.2f",av);
  id = s -> text(SCREENWIDTH/2,3*TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
  test();
//  delete buf;  // NO      <----------
  delete[] buf;  // YES     <----------
  if (h -> histogramform() == GRAPH)
  {
    int k = 0;
    double sumf = h -> sumfrequencies();
    int nc = h -> columns();
    double wd = h -> widthcolumn();
    double i = h -> startvalue();
    double st = h -> startvalue();
    double* curve = new double[BUFSIZ];
    while (i < st+nc*wd)
    {
      if (i >= 0)
        curve[k] = 1/av * exp(-i/av) * sumf *wd;
      else
        curve[k] = 0;
      i += wd/10;
      k++;
    }
    plotcontinu(curve);
//    delete curve;  // NO      <----------
    delete[] curve;  // YES     <----------
  }
}


void coutanalysis::hyperexponential()
{
  char* buf = new char[BUFSIZ];
  sprintf(buf,"HYPEREXPONENTIAL FIT : %s",h -> histogramtitle());
  int id = s -> text(SCREENWIDTH/2,TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
  sprintf(buf,"p : %.2f, a : %.2f, b : %.2f",qv,av,bv);
  id = s -> text(SCREENWIDTH/2,3*TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
  test();
//  delete buf;  // NO      <----------
  delete[] buf;  // YES     <----------
  if (h -> histogramform() == GRAPH)
  {
    int k = 0;
    double sumf = h -> sumfrequencies();
    int nc = h -> columns();
    double wd = h -> widthcolumn();
    double i = h -> startvalue();
    double st = h -> startvalue();
    double* curve = new double[BUFSIZ];
    while (i < st+nc*wd)
    {
      if (i >= 0)
        curve[k] =  ( qv/av * exp(-i/av) + (1-qv)/bv * exp(-i/bv) ) * sumf *wd;
      else
        curve[k] = 0;
      i += wd/10;
      k++;
    }
    plotcontinu(curve);
//    delete curve;  // NO      <----------
    delete[] curve;  // YES     <----------
  }
}


void coutanalysis::gamma()
{
  char* buf = new char[BUFSIZ];
  sprintf(buf,"GAMMA FIT : %s",h -> histogramtitle());
  int id = s -> text(SCREENWIDTH/2,TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
  sprintf(buf,"a : %.2f, b : %.2f",av,bv);
  id = s -> text(SCREENWIDTH/2,3*TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
  test();
//  delete buf;  // NO      <----------
  delete[] buf;  // YES     <----------
  if (h -> histogramform() == GRAPH)
  {
    int k = 0;
    double sumf = h -> sumfrequencies();
    int nc = h -> columns();
    double wd = h -> widthcolumn();
    double i = h -> startvalue();
    double st = h -> startvalue();
    double* curve = new double[BUFSIZ];
    while (i < st+nc*wd)
    {
      if (i >= 0)
      {
        double p = pow(bv,av) * pow(i,av-1) * exp(-i*bv) / exp(lngamma(av));
        curve[k] = sumf * p * wd;
      }
      else
        curve[k] = 0;
      i += wd/10;
      k++;
    }
    plotcontinu(curve);
//    delete curve;  // NO      <----------
    delete[] curve;  // YES     <----------
  }
}


void coutanalysis::erlang()
{
  char* buf = new char[BUFSIZ];
  sprintf(buf,"ERLANG FIT : %s",h -> histogramtitle());
  int id = s -> text(SCREENWIDTH/2,TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
  sprintf(buf,"a : %.2f, n : %d",av,nv);
  id = s -> text(SCREENWIDTH/2,3*TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
  test();
//  delete buf;  // NO      <----------
  delete[] buf;  // YES     <----------
  if (h -> histogramform() == GRAPH)
  {
    int k = 0;
    double sumf = h -> sumfrequencies();
    int nc = h -> columns();
    double wd = h -> widthcolumn();
    double i = h -> startvalue();
    double st = h -> startvalue();
    double* curve = new double[BUFSIZ];
    while (i < st+nc*wd)
    {
      if (i >= 0)
      {
        double p = av * pow(av*i,nv-1) * exp(-i*av) / exp(lngamma(nv));
        curve[k] = sumf * p * wd;
      }
      else
        curve[k] = 0;
      i += wd/10;
      k++;
    }
    plotcontinu(curve);
//    delete curve;  // NO      <----------
    delete[] curve;  // YES     <----------
  }
}


void coutanalysis::binomial()
{
  char* buf = new char[BUFSIZ];
  sprintf(buf,"BINOMIAL FIT : %s",h -> histogramtitle());
  int id = s -> text(SCREENWIDTH/2,TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
  sprintf(buf,"p : %.2f, n : %d",qv,nv);
  id = s -> text(SCREENWIDTH/2,3*TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
  test();
//  delete buf;  // NO      <----------
  delete[] buf;  // YES     <----------
  if (h -> histogramform() == GRAPH)
  {
    int k = 0;
    double sumf = h -> sumfrequencies();
    int nc = h -> columns();
    double wd = h -> widthcolumn();
    double st = h -> startvalue();
    double i = (int)(ceil(st));
    double* curve = new double[BUFSIZ];
    while (i < st+nc*wd)
    {
      if ( (i >= 0) && (i <= nv) )
      {
        double p = exp(lngamma(nv+1)) / exp(lngamma(i+1)) / exp(lngamma(nv-i+1)) * pow(qv,i) * pow(1-qv,nv-i);
        curve[k] = sumf * p * wd;
      }
      else
        curve[k] = 0;
      i++;
      k++;
    }
    plotdiscrete(curve);
//    delete curve;  // NO      <----------
    delete[] curve;  // YES     <----------
  }
}


void coutanalysis::geometric()
{
  char* buf = new char[BUFSIZ];
  sprintf(buf,"GEOMETRIC FIT : %s",h -> histogramtitle());
  int id = s -> text(SCREENWIDTH/2,TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
  sprintf(buf,"p : %.2f",qv);
  id = s -> text(SCREENWIDTH/2,3*TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
  test();
//  delete buf;  // NO      <----------
  delete[] buf;  // YES     <----------
  if (h -> histogramform() == GRAPH)
  {
    int k = 0;
    double sumf = h -> sumfrequencies();
    int nc = h -> columns();
    double wd = h -> widthcolumn();
    double st = h -> startvalue();
    double i = (int)(ceil(st));
    double* curve = new double[BUFSIZ];
    while (i < st+nc*wd)
    {
      if (i >= 1)
      {
        double p = pow(1-qv,i-1) * qv;
        curve[k] = sumf * p * wd;
      }
      else
        curve[k] = 0;
      i++;
      k++;
    }
    plotdiscrete(curve);
//    delete curve;  // NO      <----------
    delete[] curve;  // YES     <----------
  }
}


void coutanalysis::hypergeometric()
{
  char* buf = new char[BUFSIZ];
  sprintf(buf,"HYPERGEOMETRIC FIT : %s",h -> histogramtitle());
  int id = s -> text(SCREENWIDTH/2,TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
  sprintf(buf,"m : %d, n : %d, p : %.2f",mv,nv,qv);
  id = s -> text(SCREENWIDTH/2,3*TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
  test();
//  delete buf;  // NO      <----------
  delete[] buf;  // YES     <----------
  if (h -> histogramform() == GRAPH)
  {
    int k = 0;
    double sumf = h -> sumfrequencies();
    int nc = h -> columns();
    double wd = h -> widthcolumn();
    double st = h -> startvalue();
    double i = (int)(ceil(st));
    double* curve = new double[BUFSIZ];
    while (i < st+nc*wd)
    {
      if ( (i >= 0) && (i <= mv*qv) && (i <= nv) )
      {
        // fac(i) = lngamma(i+1)
        double c1 = exp(lngamma(mv*qv+1)) / exp(lngamma(i+1)) / exp(lngamma(mv*qv-i+1));
        double c2 = exp(lngamma(mv-mv*qv+1)) / exp(lngamma(nv-i+1)) / exp(lngamma(mv-mv*qv-nv+i+1));
        double c3 = exp(lngamma(mv+1)) / exp(lngamma(nv+1)) / exp(lngamma(mv-nv+1));
        double p = c1 * c2 / c3;
        curve[k] = sumf * p * wd;
      }
      else
        curve[k] = 0;
      i++;
      k++;
    }
    plotdiscrete(curve);
//    delete curve;  // NO      <----------
    delete[] curve;  // YES     <----------
  }
}


void coutanalysis::negativebinomial()
{
  char* buf = new char[BUFSIZ];
  sprintf(buf,"NEGATIVE BINOMIAL FIT : %s",h -> histogramtitle());
  int id = s -> text(SCREENWIDTH/2,TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
  sprintf(buf,"p : %.2f, n : %d",qv,nv);
  id = s -> text(SCREENWIDTH/2,3*TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
  test();
//  delete buf;  // NO      <----------
  delete[] buf;  // YES     <----------
  if (h -> histogramform() == GRAPH)
  {
    int k = 0;
    double sumf = h -> sumfrequencies();
    int nc = h -> columns();
    double wd = h -> widthcolumn();
    double st = h -> startvalue();
    double i = (int)ceil(st);
    double* curve = new double[BUFSIZ];
    while (i < st+nc*wd)
    {
      if (i >= 0)
      {
        double p = exp(lngamma(nv+i)) / exp(lngamma(i+1)) / exp(lngamma(nv)) * pow(qv,nv) * pow(1-qv,i);
        curve[k] = sumf * p * wd;
      }
      else
        curve[k] = 0;
      i++;
      k++;
    }
    plotdiscrete(curve);
//    delete curve;  // NO      <----------
    delete[] curve;  // YES     <----------
  }
}


void coutanalysis::poisson()
{
  char* buf = new char[BUFSIZ];
  sprintf(buf,"%.2f",av);
  sprintf(buf,"POISSON FIT : %s",h -> histogramtitle());
  int id = s -> text(SCREENWIDTH/2,TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
  sprintf(buf,"a : %.2f",av);
  id = s -> text(SCREENWIDTH/2,3*TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
  test();
//  delete buf;  // NO      <----------
  delete[] buf;  // YES     <----------
  if (h -> histogramform() == GRAPH)
  {
    int k = 0;
    double sumf = h -> sumfrequencies();
    int nc = h -> columns();
    double wd = h -> widthcolumn();
    double st = h -> startvalue();
    double i = (int)ceil(st);
    double* curve = new double[BUFSIZ];
    while (i < st+nc*wd)
    {
      if (i >= 0)
      {
        double p = pow(av,i) * exp(-av) / exp(lngamma(i+1));
        curve[k] = sumf * p * wd;
      }
      else
        curve[k] = 0;
      i++;
      k++;
    }
    plotdiscrete(curve);
//    delete curve;  // NO      <----------
    delete[] curve;  // YES     <----------
  }
}


void coutanalysis::normal()
{
  char* buf = new char[BUFSIZ];
  sprintf(buf,"NORMAL FIT : %s",h -> histogramtitle());
  int id = s -> text(SCREENWIDTH/2,TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
  sprintf(buf,"a : %.2f, b : %.2f",av,bv);
  id = s -> text(SCREENWIDTH/2,3*TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
  test();
//  delete buf;  // NO      <----------
  delete[] buf;  // YES     <----------
  if (h -> histogramform() == GRAPH)
  {
    int k = 0;
    double sumf = h -> sumfrequencies();
    int nc = h -> columns();
    double wd = h -> widthcolumn();
    double i = h -> startvalue();
    double st = h -> startvalue();
    double* curve = new double[BUFSIZ];
    while (i < st+nc*wd)
    {
      double p = exp(-(i-av)*(i-av)/2/bv/bv) / sqrt(2*PI) / bv;
      curve[k] = sumf * p * wd;
      i += wd/10;
      k++;
    }
    plotcontinu(curve);
//    delete curve;  // NO      <----------
    delete[] curve;  // YES     <----------
  }
}


void coutanalysis::lognormal()
{
  char* buf = new char[BUFSIZ];
  sprintf(buf,"LOGNORMAL FIT : %s",h -> histogramtitle());
  int id = s -> text(SCREENWIDTH/2,TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
  sprintf(buf,"a : %.2f, b : %.2f",av,bv);
  id = s -> text(SCREENWIDTH/2,3*TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
  test();
//  delete buf;  // NO      <----------
  delete[] buf;  // YES     <----------
  if (h -> histogramform() == GRAPH)
  {
    int k = 0;
    double sumf = h -> sumfrequencies();
    int nc = h -> columns();
    double wd = h -> widthcolumn();
    double i = h -> startvalue();
    double st = h -> startvalue();

    // to avoid log(0)
    if (!i)
      i = wd/10;
    double* curve = new double[BUFSIZ];
    while (i < st+nc*wd)
    {
      if (i >= 0)
      {
        double p = exp(-(log(i)-av)*(log(i)-av)/2/bv/bv) / sqrt(2*PI) / bv / i;
        curve[k] = sumf * p * wd;
      }
      else
        curve[k] = 0;
      i += wd/10;
      k++;
    }
    plotcontinu(curve);
//    delete curve;  // NO      <----------
    delete[] curve;  // YES     <----------
  }
}


void coutanalysis::laplace()
{
  char* buf = new char[BUFSIZ];
  sprintf(buf,"LAPLACE FIT : %s",h -> histogramtitle());
  int id = s -> text(SCREENWIDTH/2,TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
  sprintf(buf,"a : %.2f",av);
  id = s -> text(SCREENWIDTH/2,3*TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
  test();
//  delete buf;  // NO      <----------
  delete[] buf;  // YES     <----------
  if (h -> histogramform() == GRAPH)
  {
    int k = 0;
    double sumf = h -> sumfrequencies();
    int nc = h -> columns();
    double wd = h -> widthcolumn();
    double i = h -> startvalue();
    double st = h -> startvalue();
    double* curve = new double[BUFSIZ];
    while (i < st+nc*wd)
    {
      double p = 1/(2*av) * exp(-fabs(i)/av);
      curve[k] = sumf * p * wd;
      i += wd/10;
      k++;
    }
    plotcontinu(curve);
//    delete curve;  // NO      <----------
    delete[] curve;  // YES     <----------
  }
}


void coutanalysis::chisquare()
{
  char* buf = new char[BUFSIZ];
  sprintf(buf,"CHISQUARE FIT : %s",h -> histogramtitle());
  int id = s -> text(SCREENWIDTH/2,TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
  sprintf(buf,"n : %d",nv);
  id = s -> text(SCREENWIDTH/2,3*TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
  test();
//  delete buf;  // NO      <----------
  delete[] buf;  // YES     <----------
  if (h -> histogramform() == GRAPH)
  {
    int k = 0;
    double sumf = h -> sumfrequencies();
    int nc = h -> columns();
    double wd = h -> widthcolumn();
    double i = h -> startvalue();
    double st = h -> startvalue();

    // to avoid pow(0,0)
    if ( (nv == 2) && (i == 0) )
      i = wd/10;
    double* curve = new double[BUFSIZ];
    while (i < st+nc*wd)
    {
      if (i >= 0)
      {
        double p = pow(i,nv*0.5-1) * exp(-i*0.5) / pow(2,nv*0.5) / exp(lngamma(nv*0.5));
        curve[k] = sumf * p *wd;
      }
      else
        curve[k] = 0;
      i += wd/10;
      k++;
    }
    plotcontinu(curve);
//    delete curve;  // NO      <----------
    delete[] curve;  // YES     <----------
  }
}


void coutanalysis::student()
{
  char* buf = new char[BUFSIZ];
  sprintf(buf,"STUDENT'S T FIT : %s",h -> histogramtitle());
  int id = s -> text(SCREENWIDTH/2,TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
  sprintf(buf,"n : %d",nv);
  id = s -> text(SCREENWIDTH/2,3*TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
  test();
//  delete buf;  // NO      <----------
  delete[] buf;  // YES     <----------
  if (h -> histogramform() == GRAPH)
  {
    int k = 0;
    double sumf = h -> sumfrequencies();
    int nc = h -> columns();
    double wd = h -> widthcolumn();
    double i = h -> startvalue();
    double st = h -> startvalue();
    double* curve = new double[BUFSIZ];
    while (i < st+nc*wd)
    {
      double p = exp(lngamma(nv*0.5+0.5)) / sqrt(nv*PI) / exp(lngamma(nv*0.5)) / pow(1+i*i/nv,(nv+1)*0.5);
      curve[k] = sumf * p *wd;
      i += wd/10;
      k++;
    }
    plotcontinu(curve);
//    delete curve;  // NO      <----------
    delete[] curve;  // YES     <----------
  }
}


void coutanalysis::triangular()
{
  char* buf = new char[BUFSIZ];
  sprintf(buf,"TRIANGULAR FIT : %s",h -> histogramtitle());
  int id = s -> text(SCREENWIDTH/2,TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
  sprintf(buf,"a : %.2f",av);
  id = s -> text(SCREENWIDTH/2,3*TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
  test();
//  delete buf;  // NO      <----------
  delete[] buf;  // YES     <----------
  if (h -> histogramform() == GRAPH)
  {
    int k = 0;
    double sumf = h -> sumfrequencies();
    int nc = h -> columns();
    double wd = h -> widthcolumn();
    double i = h -> startvalue();
    double st = h -> startvalue();
    double* curve = new double[BUFSIZ];
    while (i < st+nc*wd)
    {
      double p;
      if ( (i < av) && (i >= 0) && (av < 1) )
        p = 2.0/av * i;
      else if ( (i >= av) && (i <= 1) )
        p = 2.0 / (1-av) * (1-i);
      else
        p = 0;
      curve[k] = sumf * p *wd;
      i += wd/10;
      k++;
    }
    plotcontinu(curve);
//    delete curve;  // NO      <----------
    delete[] curve;  // YES     <----------
  }
}


void coutanalysis::beta()
{
  char* buf = new char[BUFSIZ];
  sprintf(buf,"BETA FIT : %s",h -> histogramtitle());
  int id = s -> text(SCREENWIDTH/2,TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
  sprintf(buf,"a : %.2f,b : %.2f",av,bv);
  id = s -> text(SCREENWIDTH/2,3*TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
  test();
//  delete buf;  // NO      <----------
  delete[] buf;  // YES     <----------
  if (h -> histogramform() == GRAPH)
  {
    int k = 0;
    double sumf = h -> sumfrequencies();
    int nc = h -> columns();
    double wd = h -> widthcolumn();
    double i = h -> startvalue();
    double st = h -> startvalue();
    double* curve = new double[BUFSIZ];
    while (i < st+nc*wd)
    {
      double p;
      if ( (i>=0) && (i<=1) )
        p = exp(lngamma(av+bv)) / exp(lngamma(av)) / exp(lngamma(bv)) * pow(i,av-1) * pow(1-i,bv-1);
      else
        p = 0;
      curve[k] = sumf * p * wd;
      i += wd/10;
      k++;
    }
    plotcontinu(curve);
//    delete curve;  // NO      <----------
    delete[] curve;  // YES     <----------
  }
}


void coutanalysis::fdistribution()
{
  char* buf = new char[BUFSIZ];
  sprintf(buf,"FDISTRIBUTION FIT : %s",h -> histogramtitle());
  int id = s -> text(SCREENWIDTH/2,TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
  sprintf(buf,"n : %d,b : %d",nv,mv);
  id = s -> text(SCREENWIDTH/2,3*TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
  test();
//  delete buf;  // NO      <----------
  delete[] buf;  // YES     <----------
  if (h -> histogramform() == GRAPH)
  {
    int k = 0;
    double sumf = h -> sumfrequencies();
    int nc = h -> columns();
    double wd = h -> widthcolumn();
    double i = h -> startvalue();
    double st = h -> startvalue();
    double* curve = new double[BUFSIZ];
    while (i < st+nc*wd)
    {
      if (i >= 0)
      {
        double p = exp(lngamma((nv+mv)*0.5)) * pow((double)nv/(double)mv,nv*0.5) / exp(lngamma(nv*0.5)) / exp(lngamma(mv*0.5)) * pow(i,nv*0.5-1) * pow(1+nv*i/mv,(nv+mv)*-0.5);
        curve[k] = sumf * p * wd;
      }
      else
        curve[k] = 0;
      i += wd/10;
      k++;
    }
    plotcontinu(curve);
//    delete curve;  // NO      <----------
    delete[] curve;  // YES     <----------
  }
}


void coutanalysis::weibull()
{
  char* buf = new char[BUFSIZ];
  sprintf(buf,"WEIBULL FIT : %s",h -> histogramtitle());
  int id = s -> text(SCREENWIDTH/2,TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
  sprintf(buf,"a : %.2f,b : %.2f",av,bv);
  id = s -> text(SCREENWIDTH/2,3*TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
  test();
//  delete buf;  // NO      <----------
  delete[] buf;  // YES     <----------
  if (h -> histogramform() == GRAPH)
  {
    int k = 0;
    double sumf = h -> sumfrequencies();
    int nc = h -> columns();
    double wd = h -> widthcolumn();
    double i = h -> startvalue();
    double st = h -> startvalue();
    double* curve = new double[BUFSIZ];
    while (i < st+nc*wd)
    {
      if (i >= 0)
      {
        double p = bv / av * pow(i,bv-1) * exp(pow(i,bv)/-av);
        curve[k] = sumf * p * wd;
      }
      else
        curve[k] = 0;
      i += wd/10;
      k++;
    }
    plotcontinu(curve);
//    delete curve;  // NO      <----------
    delete[] curve;  // YES     <----------
  }
}


void coutanalysis::queuevalues()
{
  char* buf = new char[BUFSIZ];
  sprintf(buf,"%s : %s",tp,h -> histogramtitle());
  int id = s -> text(SCREENWIDTH/2,TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
  if ( (output == MMcSIZE) || (output == MMcWAITING) )
    sprintf(buf,"arrival rate : %.2f, service rate : %.2f, #servers : %.d",av,bv,nv);
  else
    sprintf(buf,"arrival rate : %.2f, service rate : %.2f",av,bv);
  id = s -> text(SCREENWIDTH/2,3*TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
  sprintf(buf,"theoretical mean : %f",cv);
  id = s -> text(SCREENWIDTH/2,5*TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
  sprintf(buf,"estimated mean : %f",pv);
  id = s -> text(SCREENWIDTH/2,7*TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
  sprintf(buf,"95 confidence interval : %f",conf);
  id = s -> text(SCREENWIDTH/2,9*TEXTHEIGHT/2,buf,an -> foreground());
  s -> tag(id,s -> path());
}
  

// plots the continu curve on the screen of the histogram

void coutanalysis::plotcontinu(double* curve)
{
  double* dt = h -> values();
  int nc = h -> columns();

  int i = 0;

  // maximal column value
  double max = 0.0;
  for (i=0;i<nc+2;i++)
    if (max < dt[i])
      max = dt[i];

  // underflow ?
  int x;
  if (dt[0])
    x = BORDERWIDTH + BARWIDTH;
  else
    x = BORDERWIDTH;

  for (i=0;i<nc*10-1;i++)
  {
    int y1 = BORDERHEIGHT + (int)((max-curve[i]) / max * BARHEIGHT);
    int y2 = BORDERHEIGHT + (int)((max-curve[i+1]) / max * BARHEIGHT);
    int id = h -> line(x+BARWIDTH/10*i,y1,x+BARWIDTH/10*(i+1),y2,an -> fitcolor());
    h  -> tag(id,"plot");
  }
}


// plots the discrete curve on the screen of the histogram

void coutanalysis::plotdiscrete(double* curve)
{
  double* dt = h -> values();
  int nc = h -> columns();
  double wd = h -> widthcolumn();
  double st = h -> startvalue();

  int j = 0;

  // maximal column value
  double max = 0.0;
  for (j=0;j<nc+2;j++)
    if (max < dt[j])
      max = dt[j];

  // underflow ?
  double y,x;
  if (dt[0])
    y = x = BORDERWIDTH + BARWIDTH + (ceil(st) - st) / wd * BARWIDTH;
  else
    y = x = BORDERWIDTH + (ceil(st) - st) / wd * BARWIDTH;

  j = 0;
  while (y < (x + BARWIDTH * nc))
  {
    int z = BORDERHEIGHT + (int)((max-curve[j]) / max * BARHEIGHT);
    int id = h -> oval((int)y-4,z-4,(int)y+4,z+4,an -> fitcolor());
    h -> tag(id,"plot");
    y += BARWIDTH/wd;
    j++;
  }
}


// return log of the gamma function

static double lngamma(double xx)
{
  double x = xx - 1.0;
  double tmp = x + 5.5;
  tmp -= (x + 0.5) * log(tmp);
  double ser = 1.0;
  for (int j = 0;j < 6;j++) 
  {
    x += 1.0;
    ser += cof[j] / x;
  }
  return (-tmp + log(2.50662827465 * ser));
} 


#else // ASCII


// fill the ostream with output of the taken confidence interval
// of the mean of the histogram

ostream& confidence(ostream& os,coutanalysis& ca)
{
  char* buf = new char[BUFSIZ];
  os << "-------------------------------------------------------------------" << endl;
  sprintf(buf,"CONFIDENCE : %s",ca.h -> histogramtitle());
  os << buf << endl;
  if (ca.lev == 0)
    sprintf(buf,"level : 90");
  else if (ca.lev == 1)
    sprintf(buf,"level : 95");
  else
    sprintf(buf,"level : 99");
  os << buf << endl;
  sprintf(buf,"estimated mean : %f",ca.av);
  os << buf << endl;
  sprintf(buf,"interval : +/-%f",ca.conf);
  os << buf << endl;
//  delete buf;  // NO      <----------
  delete[] buf;  // YES     <----------

  return os;
}


// fill the ostream with output of the taken confidence interval
// of the difference of means

ostream& difference(ostream& os,coutanalysis& ca)
{
  char* buf = new char[BUFSIZ];
  os << "-------------------------------------------------------------------" << endl;
  sprintf(buf,"CONFIDENCE : %s",ca.h -> histogramtitle());
  os << buf << endl;
  if (ca.lev == 0)
    sprintf(buf,"level : 90");
  else if (ca.lev == 1)
    sprintf(buf,"level : 95");
  else
    sprintf(buf,"level : 99");
  os << buf << endl;
  sprintf(buf,"estimated difference : %f",ca.estimated);
  os << buf << endl;
  sprintf(buf,"interval : +/-%f",ca.conf);
  os << buf << endl;
//  delete buf;  // NO      <----------
  delete[] buf;  // YES     <----------
  return os;
}


// fill the ostream with the covariance output

ostream& covariance(ostream& os,coutanalysis& ca)
{
  char* buf = new char[BUFSIZ];
  os << "-------------------------------------------------------------------" << endl;
  sprintf(buf,"COVARIANCE %s and %s",ca.tt1,ca.tt2);
  os << buf << endl;
  sprintf(buf,"cov(X,Y) : %f",ca.cv);
  os << buf << endl;
//  delete buf;  // NO      <----------
  delete[] buf;  // YES     <----------
  return os;
}


// fill the ostream with the correlation output

ostream& correlation(ostream& os,coutanalysis& ca)
{
  char* buf = new char[BUFSIZ];
  os << "-------------------------------------------------------------------" << endl;
  sprintf(buf,"CORRELATION COEFFICIENT %s and %s",ca.tt1,ca.tt2);
  os << buf << endl;
  sprintf(buf,"p(X,Y) : %f",ca.cv);
  os << buf << endl;
//  delete buf;  // NO      <----------
  delete[] buf;  // YES     <----------
  return os;
}


// fill the ostream with output of the taken chi square test

ostream& test(ostream& os,coutanalysis& ca)
{
  char* buf = new char[BUFSIZ];
  sprintf(buf,"chi-square : %.3f",ca.chisq);
  os << buf << endl;
  sprintf(buf,"degree of freedom : %d",ca.degree);
  os << buf << endl;
  sprintf(buf,"P(chi-square > %.3f) = %.3f",ca.chisq,ca.pv);
  os << buf << endl;
//  delete buf;  // NO      <----------
  delete[] buf;  // YES     <----------
  return os;
}

  
// fill the ostream with output of the taken chi square test

ostream& chisquaretest(ostream& os,coutanalysis& ca)
{
  char* buf = new char[BUFSIZ];
  os << "-------------------------------------------------------------------" << endl;
  sprintf(buf,"CHISQUARE TEST : %s",ca.h -> histogramtitle());
  os << buf << endl;
//  delete buf;  // NO      <----------
  delete[] buf;  // YES     <----------
  test(os,ca);
  return os;
}

  
// fill the ostream with output of the linear fit

ostream& linear(ostream& os,coutanalysis& ca)
{
  char* buf = new char[BUFSIZ];
  os << "-------------------------------------------------------------------" << endl;
  sprintf(buf,"LINEAR FIT : %s",ca.h -> histogramtitle());
  os << buf << endl;
  sprintf(buf,"a : %.2f, b : %.2f",ca.av,ca.bv);
  os << buf << endl;
//  delete buf;  // NO      <----------
  delete[] buf;  // YES     <----------
  test(os,ca);
  return os;
}


// fill the ostream with output of the quadratic fit

ostream& quadratic(ostream& os,coutanalysis& ca)
{
  char* buf = new char[BUFSIZ];
  os << "-------------------------------------------------------------------" << endl;
  sprintf(buf,"QUADRATIC FIT : %s",ca.h -> histogramtitle());
  os << buf << endl;
  sprintf(buf,"a : %.2f, b : %.2f, c : %.2f",ca.av,ca.bv,ca.cv);
  os << buf << endl;
//  delete buf;  // NO      <----------
  delete[] buf;  // YES     <----------
  test(os,ca);
  return os;
}


// fill the ostream with output of the taken uniform fit

ostream& uniform(ostream& os,coutanalysis& ca)
{
  char* buf = new char[BUFSIZ];
  os << "-------------------------------------------------------------------" << endl;
  sprintf(buf,"UNIFORM FIT : %s",ca.h -> histogramtitle());
  os << buf << endl;
  sprintf(buf,"a : %.2f",ca.av);
  os << buf << endl;
//  delete buf;  // NO      <----------
  delete[] buf;  // YES     <----------
  test(os,ca);
  return os;
}


// fill the ostream with output of the taken exponential fit

ostream& exponential(ostream& os,coutanalysis& ca)
{
  char* buf = new char[BUFSIZ];
  os << "-------------------------------------------------------------------" << endl;
  sprintf(buf,"EXPONENTIAL FIT : %s",ca.h -> histogramtitle());
  os << buf << endl;
  sprintf(buf,"a : %.2f",ca.av);
  os << buf << endl;
//  delete buf;  // NO      <----------
  delete[] buf;  // YES     <----------
  test(os,ca);
  return os;
}


// fill the ostream with output of the taken hyperexponential fit

ostream& hyperexponential(ostream& os,coutanalysis& ca)
{
  char* buf = new char[BUFSIZ];
  os << "-------------------------------------------------------------------" << endl;
  sprintf(buf,"HYPEREXPONENTIAL FIT : %s",ca.h -> histogramtitle());
  os << buf << endl;
  sprintf(buf,"p : %.2f, a : %.2f, b : %.2f",ca.qv,ca.av,ca.bv);
  os << buf << endl;
//  delete buf;  // NO      <----------
  delete[] buf;  // YES     <----------
  test(os,ca);
  return os;
}


// fill the ostream with output of the taken laplace fit

ostream& laplace(ostream& os,coutanalysis& ca)
{
  char* buf = new char[BUFSIZ];
  os << "-------------------------------------------------------------------" << endl;
  sprintf(buf,"LAPLACE FIT : %s",ca.h -> histogramtitle());
  os << buf << endl;
  sprintf(buf,"a : %.2f",ca.av);
  os << buf << endl;
//  delete buf;  // NO      <----------
  delete[] buf;  // YES     <----------
  test(os,ca);
  return os;
}


// fill the ostream with output of the taken gamma fit

ostream& gamma(ostream& os,coutanalysis& ca)
{
  char* buf = new char[BUFSIZ];
  os << "-------------------------------------------------------------------" << endl;
  sprintf(buf,"GAMMA FIT : %s",ca.h -> histogramtitle());
  os << buf << endl;
  sprintf(buf,"a : %.2f, b : %.2f",ca.av,ca.bv);
  os << buf << endl;
//  delete buf;  // NO      <----------
  delete[] buf;  // YES     <----------
  test(os,ca);
  return os;
}


// fill the ostream with output of the taken beta fit

ostream& beta(ostream& os,coutanalysis& ca)
{
  char* buf = new char[BUFSIZ];
  os << "-------------------------------------------------------------------" << endl;
  sprintf(buf,"BETA FIT : %s",ca.h -> histogramtitle());
  os << buf << endl;
  sprintf(buf,"a : %.2f, b : %.2f",ca.av,ca.bv);
  os << buf << endl;
//  delete buf;  // NO      <----------
  delete[] buf;  // YES     <----------
  test(os,ca);
  return os;
}


// fill the ostream with output of the taken normal fit

ostream& normal(ostream& os,coutanalysis& ca)
{
  char* buf = new char[BUFSIZ];
  os << "-------------------------------------------------------------------" << endl;
  sprintf(buf,"NORMAL FIT : %s",ca.h -> histogramtitle());
  os << buf << endl;
  sprintf(buf,"a : %.2f, b : %.2f",ca.av,ca.bv);
  os << buf << endl;
//  delete buf;  // NO      <----------
  delete[] buf;  // YES     <----------
  test(os,ca);
  return os;
}


// fill the ostream with output of the taken lognormal fit

ostream& lognormal(ostream& os,coutanalysis& ca)
{
  char* buf = new char[BUFSIZ];
  os << "-------------------------------------------------------------------" << endl;
  sprintf(buf,"LOGNORMAL FIT : %s",ca.h -> histogramtitle());
  os << buf << endl;
  sprintf(buf,"a : %.2f, b : %.2f",ca.av,ca.bv);
  os << buf << endl;
//  delete buf;  // NO      <----------
  delete[] buf;  // YES     <----------
  test(os,ca);
  return os;
}


// fill the ostream with output of the taken weibull fit

ostream& weibull(ostream& os,coutanalysis& ca)
{
  char* buf = new char[BUFSIZ];
  os << "-------------------------------------------------------------------" << endl;
  sprintf(buf,"WEIBULL FIT : %s",ca.h -> histogramtitle());
  os << buf << endl;
  sprintf(buf,"a : %.2f, b : %.2f",ca.av,ca.bv);
  os << buf << endl;
//  delete buf;  // NO      <----------
  delete[] buf;  // YES     <----------
  test(os,ca);
  return os;
}


// fill the ostream with output of the taken erlang fit

ostream& erlang(ostream& os,coutanalysis& ca)
{
  char* buf = new char[BUFSIZ];
  os << "-------------------------------------------------------------------" << endl;
  sprintf(buf,"ERLANG FIT : %s",ca.h -> histogramtitle());
  os << buf << endl;
  sprintf(buf,"a : %.2f, n : %d",ca.av,ca.nv);
  os << buf << endl;
//  delete buf;  // NO      <----------
  delete[] buf;  // YES     <----------
  test(os,ca);
  return os;
}


// fill the ostream with output of the taken binomial fit

ostream& binomial(ostream& os,coutanalysis& ca)
{
  char* buf = new char[BUFSIZ];
  os << "-------------------------------------------------------------------" << endl;
  sprintf(buf,"BINOMIAL FIT : %s",ca.h -> histogramtitle());
  os << buf << endl;
  sprintf(buf,"p : %.2f, n : %d",ca.qv,ca.nv);
  os << buf << endl;
//  delete buf;  // NO      <----------
  delete[] buf;  // YES     <----------
  test(os,ca);
  return os;
}


// fill the ostream with output of the taken negativebinomial fit

ostream& negativebinomial(ostream& os,coutanalysis& ca)
{
  char* buf = new char[BUFSIZ];
  os << "-------------------------------------------------------------------" << endl;
  sprintf(buf,"NEGATIVEBINOMIAL FIT : %s",ca.h -> histogramtitle());
  os << buf << endl;
  sprintf(buf,"p : %.2f, n : %d",ca.qv,ca.nv);
  os << buf << endl;
//  delete buf;  // NO      <----------
  delete[] buf;  // YES     <----------
  test(os,ca);
  return os;
}


// fill the ostream with output of the taken geometric fit

ostream& geometric(ostream& os,coutanalysis& ca)
{
  char* buf = new char[BUFSIZ];
  os << "-------------------------------------------------------------------" << endl;
  sprintf(buf,"GEOMETRIC FIT : %s",ca.h -> histogramtitle());
  os << buf << endl;
  sprintf(buf,"p : %.2f",ca.qv);
  os << buf << endl;
//  delete buf;  // NO      <----------
  delete[] buf;  // YES     <----------
  test(os,ca);
  return os;
}


// fill the ostream with output of the taken poisson fit

ostream& poisson(ostream& os,coutanalysis& ca)
{
  char* buf = new char[BUFSIZ];
  os << "-------------------------------------------------------------------" << endl;
  sprintf(buf,"POISSON FIT : %s",ca.h -> histogramtitle());
  os << buf << endl;
  sprintf(buf,"a : %.2f",ca.av);
  os << buf << endl;
//  delete buf;  // NO      <----------
  delete[] buf;  // YES     <----------
  test(os,ca);
  return os;
}


// fill the ostream with output of the taken triangular fit

ostream& triangular(ostream& os,coutanalysis& ca)
{
  char* buf = new char[BUFSIZ];
  os << "-------------------------------------------------------------------" << endl;
  sprintf(buf,"TRIANGULAR FIT : %s",ca.h -> histogramtitle());
  os << buf << endl;
  sprintf(buf,"a : %.2f",ca.av);
  os << buf << endl;
//  delete buf;  // NO      <----------
  delete[] buf;  // YES     <----------
  test(os,ca);
  return os;
}


// fill the ostream with output of the taken hypergeometric fit

ostream& hypergeometric(ostream& os,coutanalysis& ca)
{
  char* buf = new char[BUFSIZ];
  os << "-------------------------------------------------------------------" << endl;
  sprintf(buf,"HYPERGEOMETRIC FIT : %s",ca.h -> histogramtitle());
  os << buf << endl;
  sprintf(buf,"m : %d, n : %d, p : %.2f",ca.mv,ca.nv,ca.qv);
  os << buf << endl;
//  delete buf;  // NO      <----------
  delete[] buf;  // YES     <----------
  test(os,ca);
  return os;
}


// fill the ostream with output of the taken fdistribution fit

ostream& fdistribution(ostream& os,coutanalysis& ca)
{
  char* buf = new char[BUFSIZ];
  os << "-------------------------------------------------------------------" << endl;
  sprintf(buf,"FDISTRIBUTION FIT : %s",ca.h -> histogramtitle());
  os << buf << endl;
  sprintf(buf,"n : %d,m : %d",ca.nv,ca.mv);
  os << buf << endl;
//  delete buf;  // NO      <----------
  delete[] buf;  // YES     <----------
  test(os,ca);
  return os;
}


// fill the ostream with output of the taken student fit

ostream& student(ostream& os,coutanalysis& ca)
{
  char* buf = new char[BUFSIZ];
  os << "-------------------------------------------------------------------" << endl;
  sprintf(buf,"STUDENT'S T FIT : %s",ca.h -> histogramtitle());
  os << buf << endl;
  sprintf(buf,"n : %d",ca.nv);
  os << buf << endl;
//  delete buf;  // NO      <----------
  delete[] buf;  // YES     <----------
  test(os,ca);
  return os;
}


// fill the ostream with output of the taken chisquare fit

ostream& chisquare(ostream& os,coutanalysis& ca)
{
  char* buf = new char[BUFSIZ];
  os << "-------------------------------------------------------------------" << endl;
  sprintf(buf,"CHISQUARE FIT : %s",ca.h -> histogramtitle());
  os << buf << endl;
  sprintf(buf,"n : %d",ca.nv);
  os << buf << endl;
//  delete buf;  // NO      <----------
  delete[] buf;  // YES     <----------
  test(os,ca);
  return os;
}


// fill the ostream with output of the queue values

ostream& queuevalues(ostream& os,coutanalysis& ca)
{
  char* buf = new char[BUFSIZ];
  os << "-------------------------------------------------------------------" << endl;
  sprintf(buf,"%s : %s",ca.tp,ca.h -> histogramtitle());
  os << buf << endl;
  if ( (ca.output == MMcSIZE) || (ca.output == MMcWAITING) )
    sprintf(buf,"arrival rate : %.2f, service rate : %.2f, #servers : %.d",ca.av,ca.bv,ca.nv);
  else
    sprintf(buf,"arrival rate : %.2f, service rate : %.2f",ca.av,ca.bv);
  os << buf << endl;
  sprintf(buf,"theoretical mean : %f",ca.cv);
  os << buf << endl;
  sprintf(buf,"estimated mean : %f",ca.pv);
  os << buf << endl;
  sprintf(buf,"95 confidence interval : %f",ca.conf);
  os << buf << endl;
  return os;
}
  
#endif

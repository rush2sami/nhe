////////////////////////////////////////////////////////////////////////
//                                                                    //
// analysis.h                                                         //
//                                                                    //
// contains methods for analyzing a histogram. Methods for taking     //
// confidence intervals, for taking a chi square test, for analyzing  //
// simulation behavior and for fitting curves and probability         //
// distributions are provided.                                        //
//                                                                    //
// (c) 1995 D. Bolier, A. Eliens                                      //
//                                                                    //
////////////////////////////////////////////////////////////////////////


#ifndef __ANALYSIS_H
#define __ANALYSIS_H

#include <iostream.h>

class histogram;
class widget;
class screen;
class coutanalysis;

// levels of confidence
typedef enum {PERCENT90,PERCENT95,PERCENT99} level;

class analysis
{
public :
  analysis(histogram* hi);
  virtual ~analysis();
  double confidence(char* options = "");
  double confidence(histogram* hi,char* options = "");
  double covariance(histogram* h2);
  double correlation(histogram* h2);
  double chisquaretest(double* expected);
  double linear();
  double linear(double a,double b);
  double quadratic();
  double quadratic(double a,double b,double c);
  double uniform();
  double uniform(double a,double b);
  double exponential();
  double exponential(double a);
  double hyperexponential(double p);
  double hyperexponential(double p,double a,double b);
  double gamma();
  double gamma(double a,double b);
  double beta();
  double beta(double a,double b);
  double erlang();
  double erlang(unsigned int n,double a);
  double binomial();
  double binomial(double p,unsigned int n);
  double geometric();
  double geometric(double p);
  double hypergeometric(unsigned int n);
  double hypergeometric(unsigned int m,unsigned int n,double p);
  double negativebinomial();
  double negativebinomial(double p,unsigned int n);
  double poisson();
  double poisson(double a);
  double normal();
  double normal(double a,double b);
  double lognormal();
  double lognormal(double a,double b);
  double fdistribution();
  double fdistribution(unsigned int n,unsigned int m);
  double laplace();
  double laplace(double a);
  double chisquare();
  double chisquare(unsigned int n);
  double student();
  double student(unsigned int n);
  double triangular();
  double triangular(double a);
  double weibull();
  double weibull(double a,double b);
  int behavior(histogram* hist,int buffersize);
  int MM1size(double lb,double mu);
  int MM1waitingtime(double lb,double mu);
  int MMcsize(double lb,double mu,unsigned int c);
  int MMcwaitingtime(double lb,double mu,unsigned int c);
  int MD1size(double lb,double mu);
  int MD1waitingtime(double lb,double mu);
  void postscript(char* fn);
  screen* format(char* p,char* options = "");
  screen* format(widget* w,char* p,char* options = "");
#ifdef HUSH
  char* foreground()
    { return textoption; }
  char* fitcolor()
    { return fit; }
#endif

private :
  histogram* h;                   // histogram to be analyzed
  coutanalysis* ou;               // output structure
#ifdef HUSH
  char* textoption;		  // foreground screen
  char* fit;			  // fit color
  char* parse(char* options);  
#endif

  double confidencenormal(level cl);
  double confidencenormal(level cl,histogram* hi);
  double confidencestudent(level cl);
  double confidencestudent(level cl,histogram* hi);
  double covar(histogram *h2);
  int stindex(int dof);
  int chindex(int dof);
  double prob(int index);
  double chisquareprobability(double chisq,int index);
  double chisquaretest(double* expected,int& dof,double& chisq);
};


#endif

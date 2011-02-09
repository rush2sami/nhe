////////////////////////////////////////////////////////////////////////
//                                                                    //
// coutanalysis.h                                                     //
//                                                                    //
// this class takes care of outputting the results of an analysis     //
// object.                                                            //
//                                                                    //
// (c) 1995 D. Bolier, A. Eliens                                      //
//                                                                    //
////////////////////////////////////////////////////////////////////////


#ifndef __OUTPUTANALYSIS_H
#define __OUTPUTANALYSIS_H

#include <iostream.h>

class histogram;
class analysis;
#ifdef HUSH
class screen;
class widget;
#endif

enum {UNDEFINED,CONFIDENCE,DIFFERENCE,CHISQUARETEST,COVARIANCE,CORRELATION,LINEAR,QUADRATIC,UNIFORM,HYPEREXPONENTIAL,EXPONENTIAL,GAMMA,BETA,ERLANG,BINOMIAL,GEOMETRIC,HYPERGEOMETRIC,NEGBINOMIAL,POISSON,NORMAL,LOGNORMAL,LAPLACE,CHISQUARE,STUDENT,TRIANGULAR,FDISTRIBUTION,WEIBULL,MM1SIZE,MM1WAITING,MMcSIZE,MMcWAITING,MD1SIZE,MD1WAITING};

class coutanalysis
{
public :
  coutanalysis(histogram* hi,analysis* a);
  virtual ~coutanalysis();
  int type()
    { return output; }
  void confidence(int l,double cnf,double a);
  void difference(int l,double cnf,double est);
  void chisquaretest(int dof,double ch,double prob);
  void covariance(char* fn1,char* fn2,double cov);
  void correlation(char* fn1,char* fn2,double cor);
  void linear(double a,double b,int dof,double ch,double prob);
  void quadratic(double a,double b,double c,int dof,double ch,double prob);
  void uniform(double a,double b,int dof,double ch,double prob);
  void exponential(double a,int dof,double ch,double prob);
  void hyperexponential(double p,double a,double b,int dof,double ch,double prob);
  void gamma(double a,double b,int dof,double ch,double prob);
  void beta(double a,double b,int dof,double ch,double prob);
  void erlang(double a,int n,int dof,double ch,double prob);
  void binomial(int n,double p,int dof,double ch,double prob);
  void geometric(double p,int dof,double ch,double prob);
  void hypergeometric(int m,int n,double p,int dof,double ch,double prob);
  void negativebinomial(double p,int n,int dof,double ch,double prob);
  void poisson(double m,int dof,double ch,double prob);
  void normal(double m,double v,int dof,double ch,double prob);
  void lognormal(double m,double v,int dof,double ch,double prob);
  void laplace(double lb,int dof,double ch,double prob);
  void chisquare(int n,int dof,double ch,double prob);
  void student(int n,int dof,double ch,double prob);
  void fdistribution(int n,int m,int dof,double ch,double prob);
  void weibull(double a,double b,int dof,double ch,double prob);
  void triangular(double m,int dof,double ch,double prob);
  void queuevalues(char* t,int kd,double lb,double mu,double vl,double mn,double cf,int c=1);
#ifdef HUSH
  void postscript(char* fn);
  screen* format(char* p,char* options);
  screen* format(widget* w,char* p,char* options);
#endif
  friend ostream& operator<<(ostream& os,coutanalysis& ca);

private :
  histogram* h;            // histogram, that has been analyzed
  analysis* an;            // the analysis object
  int output;              // analysis type
  int lev;                 // for the level of confidence
  double conf;             // the confidence interval
  int degree;              // degree of freedom
  double chisq;            // the chi square statistic
  double pv;               // chi square probability
  double estimated;        // estimated value
  double av;               // a value 
  double bv;               // b value
  double cv;               // c value 
  double qv;               // chance on success
  int mv;                  // m value
  int nv;                  // n value
  char* tp;                // kind of analysis
  char* tt1;               // histogram titles
  char* tt2;
#ifdef HUSH
  screen* s;               // the screen for the results
  void plotcontinu(double* curve);
  void plotdiscrete(double* curve);
  void confidence();
  void difference();
  void test();
  void chisquaretest();
  void covariance();
  void correlation();
  void linear();
  void quadratic();
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
  void chisquare();
  void student();
  void fdistribution();
  void triangular();
  void weibull();
  void queuevalues();
#else
  friend ostream& confidence(ostream& os,coutanalysis& ca);
  friend ostream& difference(ostream& os,coutanalysis& ca);
  friend ostream& test(ostream& os,coutanalysis& ca);
  friend ostream& chisquaretest(ostream& os,coutanalysis& ca);
  friend ostream& covariance(ostream& os,coutanalysis& ca);
  friend ostream& correlation(ostream& os,coutanalysis& ca);
  friend ostream& linear(ostream& os,coutanalysis& ca);
  friend ostream& quadratic(ostream& os,coutanalysis& ca);
  friend ostream& uniform(ostream& os,coutanalysis& ca);
  friend ostream& exponential(ostream& os,coutanalysis& ca);
  friend ostream& hyperexponential(ostream& os,coutanalysis& ca);
  friend ostream& gamma(ostream& os,coutanalysis& ca);
  friend ostream& erlang(ostream& os,coutanalysis& ca);
  friend ostream& binomial(ostream& os,coutanalysis& ca);
  friend ostream& geometric(ostream& os,coutanalysis& ca);
  friend ostream& hypergeometric(ostream& os,coutanalysis& ca);
  friend ostream& negativebinomial(ostream& os,coutanalysis& ca);
  friend ostream& poisson(ostream& os,coutanalysis& ca);
  friend ostream& normal(ostream& os,coutanalysis& ca);
  friend ostream& lognormal(ostream& os,coutanalysis& ca);
  friend ostream& beta(ostream& os,coutanalysis& ca);
  friend ostream& laplace(ostream& os,coutanalysis& ca);
  friend ostream& chisquare(ostream& os,coutanalysis& ca);
  friend ostream& student(ostream& os,coutanalysis& ca);
  friend ostream& fdistribution(ostream& os,coutanalysis& ca);
  friend ostream& triangular(ostream& os,coutanalysis& ca);
  friend ostream& weibull(ostream& os,coutanalysis& ca);
  friend ostream& queuevalues(ostream& os,coutanalysis& ca);
#endif

};

#endif
